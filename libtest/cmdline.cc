/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
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
#include <poll.h>
#include <spawn.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef __USE_GNU
static char **environ= NULL;
#endif

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
  _use_ptrcheck(false),
  _will_fail(false),
  _argc(0),
  _exectuble(arg),
  stdin_fd(STDIN_FILENO),
  stdout_fd(STDOUT_FILENO),
  stderr_fd(STDERR_FILENO),
  built_argv(NULL),
  _pid(-1)
  { 
    if (_use_libtool)
    {
      if (libtool() == NULL)
      {
        fatal_message("libtool requested, but know libtool was found");
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
  murder();
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

  stdin_fd.dup_for_spawn(file_actions);
  stdout_fd.dup_for_spawn(file_actions);
  stderr_fd.dup_for_spawn(file_actions);

  posix_spawnattr_t spawnattr;
  posix_spawnattr_init(&spawnattr);

  sigset_t set;
  sigemptyset(&set);
  fatal_assert(posix_spawnattr_setsigmask(&spawnattr, &set) == 0);
  
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

      spawn_ret= posix_spawnp(&_pid, libtool(), &file_actions, &spawnattr, argv, environ);
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
      spawn_ret= posix_spawnp(&_pid, "gdb", &file_actions, &spawnattr, argv, environ);
    }
  }
  else
  {
    if (_use_libtool)
    {
      spawn_ret= posix_spawn(&_pid, built_argv[0], &file_actions, &spawnattr, built_argv, NULL);
    }
    else
    {
      spawn_ret= posix_spawnp(&_pid, built_argv[0], &file_actions, &spawnattr, built_argv, NULL);
    }
  }

  posix_spawn_file_actions_destroy(&file_actions);
  posix_spawnattr_destroy(&spawnattr);

  stdin_fd.close(Application::Pipe::READ);
  stdout_fd.close(Application::Pipe::WRITE);
  stderr_fd.close(Application::Pipe::WRITE);

  if (spawn_ret != 0)
  {
    if (_will_fail == false)
    {
      Error << strerror(spawn_ret) << "(" << spawn_ret << ")";
    }
    _pid= -1;
    return Application::INVALID;
  }

  return Application::SUCCESS;
}

bool Application::check() const
{
  if (_pid > 1 and kill(_pid, 0) == 0)
  {
    return true;
  }

  return false;
}

void Application::murder()
{
  if (check())
  {
    int count= 5;
    while ((count--) > 0 and check())
    {
      int kill_ret= kill(_pid, SIGTERM);
      if (kill_ret == 0)
      {
        int status= 0;
        pid_t waitpid_ret;
        if ((waitpid_ret= waitpid(_pid, &status, WNOHANG)) == -1)
        {
          switch (errno)
          {
          case ECHILD:
          case EINTR:
            break;

          default:
            Error << "waitpid() failed after kill with error of " << strerror(errno);
            break;
          }
        }

        if (waitpid_ret == 0)
        {
          libtest::dream(1, 0);
        }
      }
      else
      {
        Error << "kill(pid, SIGTERM) failed after kill with error of " << strerror(errno);
        continue;
      }

      break;
    }

    // If for whatever reason it lives, kill it hard
    if (check())
    {
      (void)kill(_pid, SIGKILL);
    }
  }
  slurp();
}

// false means that no data was returned
bool Application::slurp()
{
  struct pollfd fds[2];
  fds[0].fd= stdout_fd.fd();
  fds[0].events= POLLRDNORM;
  fds[0].revents= 0;
  fds[1].fd= stderr_fd.fd();
  fds[1].events= POLLRDNORM;
  fds[1].revents= 0;

  int active_fd;
  if ((active_fd= poll(fds, 2, 0)) == -1)
  {
    int error;
    switch ((error= errno))
    {
#ifdef TARGET_OS_LINUX
    case ERESTART:
#endif
    case EINTR:
      break;

    case EFAULT:
    case ENOMEM:
      fatal_message(strerror(error));
      break;

    case EINVAL:
      fatal_message("RLIMIT_NOFILE exceeded, or if OSX the timeout value was invalid");
      break;

    default:
      fatal_message(strerror(error));
      break;
    }

    return false;
  }

  if (active_fd == 0)
  {
    return false;
  }

  bool data_was_read= false;
  if (fds[0].revents & POLLRDNORM)
  {
    if (stdout_fd.read(_stdout_buffer) == true)
    {
      data_was_read= true;
    }
  }

  if (fds[1].revents & POLLRDNORM)
  {
    if (stderr_fd.read(_stderr_buffer) == true)
    {
      data_was_read= true;
    }
  }

  return data_was_read;
}

Application::error_t Application::wait(bool nohang)
{
  if (_pid == -1)
  {
    return Application::INVALID;
  }

  slurp();

  error_t exit_code= FAILURE;
  {
    int status= 0;
    pid_t waited_pid;
    if ((waited_pid= waitpid(_pid, &status, nohang ? WNOHANG : 0)) == -1)
    {
      switch (errno)
      {
      case ECHILD:
        exit_code= Application::SUCCESS;
        break;

      case EINTR:
        break;

      default:
        Error << "Error occured while waitpid(" << strerror(errno) << ") on pid " << int(_pid);
        break;
      }
    }
    else if (waited_pid == 0)
    {
      exit_code= Application::SUCCESS;
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

  slurp();

#if 0
  if (exit_code == Application::INVALID)
  {
    Error << print_argv(built_argv, _argc);
  }
#endif

  return exit_code;
}

void Application::add_long_option(const std::string& name, const std::string& option_value)
{
  std::string arg(name);
  arg+= option_value;
  _options.push_back(std::make_pair(arg, std::string()));
}

void Application::add_option(const std::string& arg)
{
  _options.push_back(std::make_pair(arg, std::string()));
}

void Application::add_option(const std::string& name, const std::string& value)
{
  _options.push_back(std::make_pair(name, value));
}

Application::Pipe::Pipe(int arg) :
  _std_fd(arg)
{
  _pipe_fd[READ]= -1;
  _pipe_fd[WRITE]= -1;
  _open[READ]= false;
  _open[WRITE]= false;
}

int Application::Pipe::Pipe::fd()
{
  if (_std_fd == STDOUT_FILENO)
  {
    return _pipe_fd[READ];
  }
  else if (_std_fd == STDERR_FILENO)
  {
    return _pipe_fd[READ];
  }

  return _pipe_fd[WRITE]; // STDIN_FILENO
}


bool Application::Pipe::read(libtest::vchar_t& arg)
{
  fatal_assert(_std_fd == STDOUT_FILENO or _std_fd == STDERR_FILENO);

  bool data_was_read= false;

  ssize_t read_length;
  char buffer[1024]= { 0 };
  while ((read_length= ::read(_pipe_fd[READ], buffer, sizeof(buffer))))
  {
    if (read_length == -1)
    {
      switch(errno)
      {
      case EAGAIN:
        break;

      default:
        Error << strerror(errno);
        break;
      }

      break;
    }

    data_was_read= true;
    arg.reserve(read_length +1);
    for (size_t x= 0; x < read_length; x++)
    {
      arg.push_back(buffer[x]);
    }
    // @todo Suck up all errput code here
  }

  return data_was_read;
}

void Application::Pipe::nonblock()
{
  int ret;
  if ((ret= fcntl(_pipe_fd[READ], F_GETFL, 0)) == -1)
  {
    Error << "fcntl(F_GETFL) " << strerror(errno);
    throw strerror(errno);
  }

  if ((ret= fcntl(_pipe_fd[READ], F_SETFL, ret | O_NONBLOCK)) == -1)
  {
    Error << "fcntl(F_SETFL) " << strerror(errno);
    throw strerror(errno);
  }
}

void Application::Pipe::reset()
{
  close(READ);
  close(WRITE);

#if HAVE_PIPE2
  if (pipe2(_pipe_fd, O_NONBLOCK) == -1)
#else
  if (pipe(_pipe_fd) == -1)
#endif
  {
    fatal_message(strerror(errno));
  }
  _open[0]= true;
  _open[1]= true;

  if (true)
  {
    nonblock();
    cloexec();
  }
}

void Application::Pipe::cloexec()
{
  int ret;
  if ((ret= fcntl(_pipe_fd[WRITE], F_GETFD, 0)) == -1)
  {
    Error << "fcntl(F_GETFD) " << strerror(errno);
    throw strerror(errno);
  }

  if ((ret= fcntl(_pipe_fd[WRITE], F_SETFD, ret | FD_CLOEXEC)) == -1)
  {
    Error << "fcntl(F_SETFD) " << strerror(errno);
    throw strerror(errno);
  }
}

Application::Pipe::~Pipe()
{
  if (_pipe_fd[0] != -1)
  {
    ::close(_pipe_fd[0]);
  }

  if (_pipe_fd[1] != -1)
  {
    ::close(_pipe_fd[1]);
  }
}

void Application::Pipe::dup_for_spawn(posix_spawn_file_actions_t& file_actions)
{
  int type= STDIN_FILENO == _std_fd ? 0 : 1;

  int ret;
  if ((ret= posix_spawn_file_actions_adddup2(&file_actions, _pipe_fd[type], _std_fd )) < 0)
  {
    Error << "posix_spawn_file_actions_adddup2(" << strerror(ret) << ")";
    fatal_message(strerror(ret));
  }

  if ((ret= posix_spawn_file_actions_addclose(&file_actions, _pipe_fd[type])) < 0)
  {
    Error << "posix_spawn_file_actions_adddup2(" << strerror(ret) << ")";
    fatal_message(strerror(ret));
  }
}

void Application::Pipe::close(const close_t& arg)
{
  int type= int(arg);

  if (_open[type])
  {
    int ret;
    if (::close(_pipe_fd[type]) == -1)
    {
      Error << "close(" << strerror(errno) << ")";
    }
    _open[type]= false;
    _pipe_fd[type]= -1;
  }
}

void Application::create_argv(const char *args[])
{
  delete_argv();
  fatal_assert(_argc == 0);

  if (_use_libtool)
  {
    _argc+= 2; // +2 for libtool --mode=execute
  }

  _argc+= 1; // For the command

  /*
    valgrind --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --track-origin=yes --malloc-fill=A5 --free-fill=DE --log-file=
  */
  if (_use_valgrind)
  {
    _argc+= 8;
  }
  else if (_use_ptrcheck)
  {
    /*
      valgrind --error-exitcode=1 --tool=exp-ptrcheck --log-file= 
    */
    _argc+= 4;
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

  _argc+= 1; // for the NULL

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
#if 0
    built_argv[x++]= strdup("--track-origin=yes");
#endif
    built_argv[x++]= strdup("--malloc-fill=A5");
    built_argv[x++]= strdup("--free-fill=DE");

    std::string log_file= create_tmpfile("valgrind");
    char buffer[1024];
    int length= snprintf(buffer, sizeof(buffer), "--log-file=%s", log_file.c_str());
    fatal_assert(length > 0 and length < sizeof(buffer));
    built_argv[x++]= strdup(buffer);
  }
  else if (_use_ptrcheck)
  {
    /*
      valgrind --error-exitcode=1 --tool=exp-ptrcheck --log-file= 
    */
    built_argv[x++]= strdup("valgrind");
    built_argv[x++]= strdup("--error-exitcode=1");
    built_argv[x++]= strdup("--tool=exp-ptrcheck");
    _argc+= 4;
    std::string log_file= create_tmpfile("ptrcheck");
    char buffer[1024];
    int length= snprintf(buffer, sizeof(buffer), "--log-file=%s", log_file.c_str());
    fatal_assert(length > 0 and length < sizeof(buffer));
    built_argv[x++]= strdup(buffer);
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
  built_argv[x++]= NULL;
  fatal_assert(x == _argc);
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

  return int(app.wait(false));
}

const char *gearmand_binary() 
{
  return GEARMAND_BINARY;
}

const char *drizzled_binary() 
{
  return DRIZZLED_BINARY;
}

} // namespace exec_cmdline
