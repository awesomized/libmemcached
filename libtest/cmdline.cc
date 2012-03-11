/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <config.h>
#include <libtest/common.h>

using namespace libtest;

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <fstream>
#include <memory>
#include <spawn.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

extern "C" {
  static int exited_successfully(int status)
  {
    if (status == 0)
    {
      return EXIT_SUCCESS;
    }

    if (WIFEXITED(status) == true)
    {
      return WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status) == true)
    {
      return WTERMSIG(status);
    }

    return EXIT_FAILURE;
  }
}

namespace {

  std::string print_argv(char * * & built_argv, const size_t& argc)
  {
    std::stringstream arg_buffer;

    for (size_t x= 0; x < argc; x++)
    {
      arg_buffer << built_argv[x] << " ";
    }

    return arg_buffer.str();
  }

  std::string print_argv(char** argv)
  {
    std::stringstream arg_buffer;

    for (char** ptr= argv; *ptr; ptr++)
    {
      arg_buffer << *ptr << " ";
    }

    return arg_buffer.str();
  }

  static Application::error_t int_to_error_t(int arg)
  {
    switch (arg)
    {
    case 127:
      return Application::INVALID;

    case 0:
      return Application::SUCCESS;

    default:
    case 1:
      return Application::FAILURE;
    }
  }
}

namespace libtest {

Application::Application(const std::string& arg, const bool _use_libtool_arg) :
  _use_libtool(_use_libtool_arg),
  _use_valgrind(false),
  _use_gdb(false),
  _argc(0),
  _exectuble(arg),
  built_argv(NULL),
  _pid(-1)
  { 
    if (_use_libtool)
    {
      if (libtool() == NULL)
      {
        throw "libtool requested, but know libtool was found";
      }
    }

    // Find just the name of the application with no path
    {
      size_t found= arg.find_last_of("/\\");
      if (found)
      {
        _exectuble_name= arg.substr(found +1);
      }
      else
      {
        _exectuble_name= arg;
      }
    }

    if (_use_libtool and getenv("PWD"))
    {
      _exectuble_with_path+= getenv("PWD");
      _exectuble_with_path+= "/";
    }
    _exectuble_with_path+= _exectuble;
  }

Application::~Application()
{
  delete_argv();
}

Application::error_t Application::run(const char *args[])
{
  stdin_fd.reset();
  stdout_fd.reset();
  stderr_fd.reset();
  _stdout_buffer.clear();
  _stderr_buffer.clear();

  posix_spawn_file_actions_t file_actions;
  posix_spawn_file_actions_init(&file_actions);

  stdin_fd.dup_for_spawn(Application::Pipe::READ, file_actions, STDIN_FILENO);
  stdout_fd.dup_for_spawn(Application::Pipe::WRITE, file_actions, STDOUT_FILENO);
  stderr_fd.dup_for_spawn(Application::Pipe::WRITE, file_actions, STDERR_FILENO);
  
  create_argv(args);

  int spawn_ret;
  if (_use_gdb)
  {
    std::string gdb_run_file= create_tmpfile(_exectuble_name);
    std::fstream file_stream;
    file_stream.open(gdb_run_file.c_str(), std::fstream::out | std::fstream::trunc);

    _gdb_filename= create_tmpfile(_exectuble_name);
    file_stream 
      << "set logging redirect on" << std::endl
      << "set logging file " << _gdb_filename << std::endl
      << "set logging overwrite on" << std::endl
      << "set logging on" << std::endl
      << "set environment LIBTEST_IN_GDB=1" << std::endl
      << "run " << arguments() << std::endl
      << "thread apply all bt" << std::endl
      << "quit" << std::endl;

    fatal_assert(file_stream.good());
    file_stream.close();

    if (_use_libtool)
    {
      // libtool --mode=execute gdb -f -x binary
      char *argv[]= {
        const_cast<char *>(libtool()),
        const_cast<char *>("--mode=execute"),
        const_cast<char *>("gdb"),
        const_cast<char *>("-batch"),
        const_cast<char *>("-f"),
        const_cast<char *>("-x"),
        const_cast<char *>(gdb_run_file.c_str()), 
        const_cast<char *>(_exectuble_with_path.c_str()), 
        0};

      spawn_ret= posix_spawnp(&_pid, libtool(), &file_actions, NULL, argv, NULL);
    }
    else
    {
      // gdb binary
      char *argv[]= {
        const_cast<char *>("gdb"),
        const_cast<char *>("-batch"),
        const_cast<char *>("-f"),
        const_cast<char *>("-x"),
        const_cast<char *>(gdb_run_file.c_str()), 
        const_cast<char *>(_exectuble_with_path.c_str()), 
        0};
      spawn_ret= posix_spawnp(&_pid, "gdb", &file_actions, NULL, argv, NULL);
    }
  }
  else
  {
    if (_use_libtool)
    {
      spawn_ret= posix_spawn(&_pid, built_argv[0], &file_actions, NULL, built_argv, NULL);
    }
    else
    {
      spawn_ret= posix_spawnp(&_pid, built_argv[0], &file_actions, NULL, built_argv, NULL);
    }
  }

  posix_spawn_file_actions_destroy(&file_actions);

  stdin_fd.close(Application::Pipe::READ);
  stdout_fd.close(Application::Pipe::WRITE);
  stderr_fd.close(Application::Pipe::WRITE);

  if (spawn_ret)
  {
    return Application::INVALID;
  }

  return Application::SUCCESS;
}

Application::error_t Application::wait()
{
  if (_pid == -1)
  {
    Error << "wait() got an invalid pid_t";
    return Application::INVALID;
  }

  {
    ssize_t read_length;
    char buffer[1024]= { 0 };
    bool bail= false;
    while (((read_length= ::read(stdout_fd.fd()[0], buffer, sizeof(buffer))) != 0) or bail)
    {
      if (read_length == -1)
      {
        switch(errno)
        {
        case EAGAIN:
          continue;

        default:
          Error << strerror(errno);
          bail= true;
        }
      }
      _stdout_buffer.reserve(read_length +1);
      for (size_t x= 0; x < read_length; x++)
      {
        _stdout_buffer.push_back(buffer[x]);
      }
      // @todo Suck up all output code here
    }
  }

  {
    ssize_t read_length;
    char buffer[1024]= { 0 };
    bool bail= false;
    while (((read_length= ::read(stderr_fd.fd()[0], buffer, sizeof(buffer))) != 0) or bail)
    {
      if (read_length == -1)
      {
        switch(errno)
        {
        case EAGAIN:
          continue;

        default:
          Error << strerror(errno);
          bail= true;
        }
      }
      _stderr_buffer.reserve(read_length +1);
      for (size_t x= 0; x < read_length; x++)
      {
        _stderr_buffer.push_back(buffer[x]);
      }
      // @todo Suck up all errput code here
    }
  }

  error_t exit_code= FAILURE;
  {
    int status= 0;
    pid_t waited_pid;
    if ((waited_pid= waitpid(_pid, &status, 0)) == -1)
    {
      Error << "Error occured while waitpid(" << strerror(errno) << ") on pid " << int(_pid);
    }
    else
    {
      if (waited_pid != _pid)
      {
        throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "Pid mismatch, %d != %d", int(waited_pid), int(_pid));
      }
      exit_code= int_to_error_t(exited_successfully(status));
    }
  }

#if 0
  if (exit_code == Application::INVALID)
  {
    Error << print_argv(built_argv, _argc);
  }
#endif

  return exit_code;
}

void Application::add_option(const std::string& arg)
{
  _options.push_back(std::make_pair(arg, std::string()));
}

void Application::add_option(const std::string& name, const std::string& value)
{
  _options.push_back(std::make_pair(name, value));
}

Application::Pipe::Pipe()
{
  _fd[0]= -1;
  _fd[1]= -1;
  _open[0]= false;
  _open[1]= false;
}

void Application::Pipe::reset()
{
  close(READ);
  close(WRITE);

  int ret;
  if (pipe(_fd) == -1)
  {
    throw strerror(errno);
  }
  _open[0]= true;
  _open[1]= true;

  {
    if ((ret= fcntl(_fd[0], F_GETFL, 0)) == -1)
    {
      Error << "fcntl(F_GETFL) " << strerror(errno);
      throw strerror(errno);
    }

    if ((ret= fcntl(_fd[0], F_SETFL, ret | O_NONBLOCK)) == -1)
    {
      Error << "fcntl(F_SETFL) " << strerror(errno);
      throw strerror(errno);
    }
  }
}

Application::Pipe::~Pipe()
{
  close(READ);
  close(WRITE);
}

void Application::Pipe::dup_for_spawn(const close_t& arg, posix_spawn_file_actions_t& file_actions, const int newfildes)
{
  int type= int(arg);

  int ret;
  if ((ret= posix_spawn_file_actions_adddup2(&file_actions, _fd[type], newfildes )) < 0)
  {
    Error << "posix_spawn_file_actions_adddup2(" << strerror(ret) << ")";
    throw strerror(ret);
  }

  if ((ret= posix_spawn_file_actions_addclose(&file_actions, _fd[type])) < 0)
  {
    Error << "posix_spawn_file_actions_adddup2(" << strerror(ret) << ")";
    throw strerror(ret);
  }
}

void Application::Pipe::close(const close_t& arg)
{
  int type= int(arg);

  if (_open[type])
  {
    int ret;
    if (::close(_fd[type]) == -1)
    {
      Error << "close(" << strerror(errno) << ")";
    }
    _open[type]= false;
    _fd[type]= -1;
  }
}

void Application::create_argv(const char *args[])
{
  _argc= 2 +_use_libtool ? 2 : 0; // +1 for the command, +2 for libtool/mode=execute, +1 for the NULL

  if (_use_libtool)
  {
    _argc+= 2; // +2 for libtool --mode=execute
  }

  /*
    valgrind --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE
  */
  if (_use_valgrind)
  {
    _argc+= 7;
  }
  else if (_use_gdb) // gdb
  {
    _argc+= 1;
  }

  for (Options::const_iterator iter= _options.begin(); iter != _options.end(); iter++)
  {
    _argc++;
    if ((*iter).second.empty() == false)
    {
      _argc++;
    }
  }

  if (args)
  {
    for (const char **ptr= args; *ptr; ++ptr)
    {
      _argc++;
    }
  }

  delete_argv();
  built_argv= new char * [_argc];

  size_t x= 0;
  if (_use_libtool)
  {
    assert(libtool());
    built_argv[x++]= strdup(libtool());
    built_argv[x++]= strdup("--mode=execute");
  }

  if (_use_valgrind)
  {
    /*
      valgrind --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE
    */
    built_argv[x++]= strdup("valgrind");
    built_argv[x++]= strdup("--error-exitcode=1");
    built_argv[x++]= strdup("--leak-check=yes");
    built_argv[x++]= strdup("--show-reachable=yes");
    built_argv[x++]= strdup("--track-fds=yes");
    built_argv[x++]= strdup("--malloc-fill=A5");
    built_argv[x++]= strdup("--free-fill=DE");
  }
  else if (_use_gdb)
  {
    built_argv[x++]= strdup("gdb");
  }

  built_argv[x++]= strdup(_exectuble_with_path.c_str());

  for (Options::const_iterator iter= _options.begin(); iter != _options.end(); iter++)
  {
    built_argv[x++]= strdup((*iter).first.c_str());
    if ((*iter).second.empty() == false)
    {
      built_argv[x++]= strdup((*iter).second.c_str());
    }
  }

  if (args)
  {
    for (const char **ptr= args; *ptr; ++ptr)
    {
      built_argv[x++]= strdup(*ptr);
    }
  }
  built_argv[_argc -1]= NULL;
}

std::string Application::print()
{
  return print_argv(built_argv, _argc);
}

std::string Application::arguments()
{
  std::stringstream arg_buffer;

  for (size_t x= 1 + _use_libtool ? 2 : 0;
       x < _argc and built_argv[x];
       x++)
  {
    arg_buffer << built_argv[x] << " ";
  }

  return arg_buffer.str();
}

void Application::delete_argv()
{
  if (built_argv)
  {
    for (size_t x= 0; x < _argc; x++)
    {
      if (built_argv[x])
      {
        ::free(built_argv[x]);
      }
    }
    delete[] built_argv;
    built_argv= NULL;
    _argc= 0;
  }
}


int exec_cmdline(const std::string& command, const char *args[], bool use_libtool)
{
  Application app(command, use_libtool);

  Application::error_t ret= app.run(args);

  if (ret != Application::SUCCESS)
  {
    return int(ret);
  }

  return int(app.wait());
}

const char *gearmand_binary() 
{
  return GEARMAND_BINARY;
}

} // namespace exec_cmdline
