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


#include <libtest/common.h>

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <iostream>

#include <algorithm> 
#include <functional> 
#include <locale>

// trim from end 
static inline std::string &rtrim(std::string &s)
{ 
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end()); 
  return s; 
}

#include <libtest/server.h>
#include <libtest/stream.h>
#include <libtest/killpid.h>

extern "C" {
  static bool exited_successfully(int status)
  {
    if (WEXITSTATUS(status) == 0)
    {
      return true;
    }

    return true;
  }
}


namespace libtest {

std::ostream& operator<<(std::ostream& output, const Server &arg)
{
  if (arg.is_socket())
  {
    output << arg.hostname();
  }
  else
  {
    output << arg.hostname() << ":" << arg.port();
  }

  if (arg.has_pid())
  {
    output << " Pid:" <<  arg.pid();
  }

  if (arg.has_socket())
  {
    output << " Socket:" <<  arg.socket();
  }

  if (not arg.running().empty())
  {
    output << " Exec:" <<  arg.running();
  }


  return output;  // for multiple << operators
}

void Server::nap(void)
{
#ifdef WIN32
  sleep(1);
#else
  struct timespec global_sleep_value= { 0, 50000 };
  nanosleep(&global_sleep_value, NULL);
#endif
}

Server::Server(const std::string& host_arg, const in_port_t port_arg, bool is_socket_arg) :
  _is_socket(is_socket_arg),
  _pid(-1),
  _port(port_arg),
  _hostname(host_arg)
{
}

Server::~Server()
{
  if (has_pid() and not kill(_pid))
  {
    Error << "Unable to kill:" << *this;
  }
}

std::string server_startup_st::option_string() const
{
  std::string temp= server_list;
  rtrim(temp);
  return temp;
}

// If the server exists, kill it
bool Server::cycle()
{
  uint32_t limit= 3;

  // Try to ping, and kill the server #limit number of times
  pid_t current_pid;
  while (--limit and is_pid_valid(current_pid= get_pid()))
  {
    if (kill(current_pid))
    {
      Log << "Killed existing server," << *this << " with pid:" << current_pid;
      nap();
      continue;
    }
  }

  // For whatever reason we could not kill it, and we reached limit
  if (limit == 0)
  {
    Error << "Reached limit, could not kill server pid:" << current_pid;
    return false;
  }

  return true;
}

// Grab a one off command
bool Server::command(std::string& command_arg)
{
  rebuild_base_command();

  command_arg+= _base_command;

  if (args(command_arg))
  {
    return true;
  }

  return false;
}

bool Server::start()
{
  // If we find that we already have a pid then kill it.
  if (has_pid() and not kill(_pid))
  {
    Error << "Could not kill() existing server during start() pid:" << _pid;
    return false;
  }
  assert(not has_pid());

  _running.clear();
  if (not command(_running))
  {
    Error << "Could not build command()";
    return false;
  }

  if (is_valgrind() or is_helgrind())
  {
    _running+= " &";
  }

  int ret= system(_running.c_str());
  if (not exited_successfully(ret))
  {
    Error << "system() failed:" << strerror(errno);
    _running.clear();
    return false;
  }

  if (is_helgrind() or is_valgrind())
  {
    sleep(4);
  }

  if (pid_file_option() and not pid_file().empty())
  {
    Wait wait(pid_file(), 8);

    if (not wait.successful())
    {
      Error << "Unable to open pidfile for: " << _running;
    }
  }

  int count= is_helgrind() or is_valgrind() ? 20 : 5;
  while (not ping() and --count)
  {
    nap();
  }

  if (count == 0)
  {
    // If we happen to have a pid file, lets try to kill it
    if (pid_file_option() and not pid_file().empty())
    {
      kill_file(pid_file());
    }
    Error << "Failed to ping() server started with:" << _running;
    _running.clear();
    return false;
  }

  // A failing get_pid() at this point is considered an error
  _pid= get_pid(true);

  return has_pid();
}

void Server::reset_pid()
{
  _running.clear();
  _pid_file.clear();
  _pid= -1;
}

pid_t Server::pid()
{
  return _pid;
}

bool Server::set_socket_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  if (broken_pid_file())
  {
    snprintf(file_buffer, sizeof(file_buffer), "/tmp/%s.socketXXXXXX", name());
  }
  else
  {
    snprintf(file_buffer, sizeof(file_buffer), "var/run/%s.socketXXXXXX", name());
  }

  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    perror(file_buffer);
    return false;
  }
  close(fd);
  unlink(file_buffer);

  _socket= file_buffer;

  return true;
}

bool Server::set_pid_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  if (broken_pid_file())
  {
    snprintf(file_buffer, sizeof(file_buffer), "/tmp/%s.pidXXXXXX", name());
  }
  else
  {
    snprintf(file_buffer, sizeof(file_buffer), "var/run/%s.pidXXXXXX", name());
  }

  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    perror(file_buffer);
    return false;
  }
  close(fd);
  unlink(file_buffer);

  _pid_file= file_buffer;

  return true;
}

bool Server::set_log_file()
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  snprintf(file_buffer, sizeof(file_buffer), "var/log/%s.logXXXXXX", name());
  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    perror(file_buffer);
    return false;
  }
  close(fd);

  _log_file= file_buffer;

  return true;
}

void Server::rebuild_base_command()
{
  _base_command.clear();
  if (is_libtool())
  {
    _base_command+= libtool();
  }

  if (is_debug() and getenv("GDB_COMMAND"))
  {
    _base_command+= getenv("GDB_COMMAND");
    _base_command+= " ";
  }
  else if (is_valgrind() and getenv("VALGRIND_COMMAND"))
  {
    _base_command+= getenv("VALGRIND_COMMAND");
    _base_command+= " ";
  }
  else if (is_helgrind() and getenv("HELGRIND_COMMAND"))
  {
    _base_command+= getenv("HELGRIND_COMMAND");
    _base_command+= " ";
  }

  _base_command+= executable();
}

void Server::set_extra_args(const std::string &arg)
{
  _extra_args= arg;
}

bool Server::args(std::string& options)
{
  std::stringstream arg_buffer;

  // Set a log file if it was requested (and we can)
  if (getenv("LIBTEST_LOG") and log_file_option())
  {
    if (not set_log_file())
    {
      return false;
    }

    arg_buffer << " " << log_file_option() << _log_file;
  }

  // Update pid_file
  if (pid_file_option())
  {
    if (_pid_file.empty() and not set_pid_file())
    {
      return false;
    }

    arg_buffer << " " << pid_file_option() << pid_file(); 
  }

  assert(daemon_file_option());
  if (daemon_file_option() and not is_valgrind() and not is_helgrind())
  {
    arg_buffer << " " << daemon_file_option();
  }

  if (_is_socket and socket_file_option())
  {
    if (not set_socket_file())
    {
      return false;
    }

    arg_buffer << " " << socket_file_option() << "\"" <<  _socket << "\"";
  }

  assert(port_option());
  if (port_option() and _port > 0)
  {
    arg_buffer << " " << port_option() << _port;
  }

  options+= arg_buffer.str();

  if (not _extra_args.empty())
  {
    options+= _extra_args;
  }

  return true;
}

bool Server::is_debug() const
{
  return bool(getenv("LIBTEST_MANUAL_GDB"));
}

bool Server::is_valgrind() const
{
  return bool(getenv("LIBTEST_MANUAL_VALGRIND"));
}

bool Server::is_helgrind() const
{
  return bool(getenv("LIBTEST_MANUAL_HELGRIND"));
}

bool Server::kill(pid_t pid_arg)
{
  if (check_pid(pid_arg) and kill_pid(pid_arg)) // If we kill it, reset
  {
    if (broken_pid_file() and not pid_file().empty())
    {
      unlink(pid_file().c_str());
    }

    if (broken_socket_cleanup() and has_socket() and not socket().empty())
    {
      unlink(socket().c_str());
    }

    reset_pid();

    return true;
  }

  return false;
}

void server_startup_st::push_server(Server *arg)
{
  servers.push_back(arg);

  char port_str[NI_MAXSERV];
  snprintf(port_str, sizeof(port_str), "%u", int(arg->port()));

  std::string server_config_string;
  if (arg->has_socket())
  {
    server_config_string+= "--socket=";
    server_config_string+= '"';
    server_config_string+= arg->socket();
    server_config_string+= '"';
    server_config_string+= " ";
  }
  else
  {
    server_config_string+= "--server=";
    server_config_string+= arg->hostname();
    server_config_string+= ":";
    server_config_string+= port_str;
    server_config_string+= " ";
  }

  server_list+= server_config_string;

}

Server* server_startup_st::pop_server()
{
  Server *tmp= servers.back();
  servers.pop_back();
  return tmp;
}

void server_startup_st::shutdown(bool remove)
{
  if (remove)
  {
    for (std::vector<Server *>::iterator iter= servers.begin(); iter != servers.end(); iter++)
    {
      delete *iter;
    }
    servers.clear();
  }
  else
  {
    for (std::vector<Server *>::iterator iter= servers.begin(); iter != servers.end(); iter++)
    {
      if ((*iter)->has_pid() and not (*iter)->kill((*iter)->pid()))
      {
        Error << "Unable to kill:" <<  *(*iter);
      }
    }
  }
}

server_startup_st::~server_startup_st()
{
  shutdown(true);
}

bool server_startup_st::is_debug() const
{
  return bool(getenv("LIBTEST_MANUAL_GDB"));
}

bool server_startup_st::is_valgrind() const
{
  return bool(getenv("LIBTEST_MANUAL_VALGRIND"));
}

bool server_startup_st::is_helgrind() const
{
  return bool(getenv("LIBTEST_MANUAL_HELGRIND"));
}


bool server_startup(server_startup_st& construct, const std::string& server_type, in_port_t try_port, int argc, const char *argv[])
{
  Outn();
  (void)try_port;

  // Look to see if we are being provided ports to use
  {
    char variable_buffer[1024];
    snprintf(variable_buffer, sizeof(variable_buffer), "LIBTEST_PORT_%lu", (unsigned long)construct.count());

    char *var;
    if ((var= getenv(variable_buffer)))
    {
      in_port_t tmp= in_port_t(atoi(var));

      if (tmp > 0)
        try_port= tmp;
    }
  }

  libtest::Server *server= NULL;
  if (0)
  { }
  else if (server_type.compare("gearmand") == 0)
  {
    if (GEARMAND_BINARY)
    {
      if (HAVE_LIBGEARMAN)
      {
        server= build_gearmand("localhost", try_port);
      }
      else
      {
        Error << "Libgearman was not found";
      }
    } 
    else
    {
      Error << "No gearmand binary is available";
    }
  }
  else if (server_type.compare("blobslap_worker") == 0)
  {
    if (GEARMAND_BINARY and GEARMAND_BLOBSLAP_WORKER)
    {
      if (HAVE_LIBGEARMAN)
      {
        server= build_blobslap_worker(try_port);
      }
      else
      {
        Error << "Libgearman was not found";
      }
    }
    else
    {
      Error << "No gearmand binary is available";
    }
  }
  else if (server_type.compare("memcached-sasl") == 0)
  {
    if (MEMCACHED_SASL_BINARY)
    {
      if (HAVE_LIBMEMCACHED)
      {
        server= build_memcached_sasl("localhost", try_port, construct.username(), construct.password());
      }
      else
      {
        Error << "Libmemcached was not found";
      }
    }
    else
    {
      Error << "No memcached binary that was compiled with sasl is available";
    }
  }
  else if (server_type.compare("memcached") == 0)
  {
    if (MEMCACHED_BINARY)
    {
      if (HAVE_LIBMEMCACHED)
      {
        server= build_memcached("localhost", try_port);
      }
      else
      {
        Error << "Libmemcached was not found";
      }
    }
    else
    {
      Error << "No memcached binary is available";
    }
  }
  else
  {
    Error << "Failed to start " << server_type << ", no support was found to be compiled in for it.";
  }

  if (server == NULL)
  {
    Error << "Failure occured while creating server: " <<  server_type;
    return false;
  }

  /*
    We will now cycle the server we have created.
  */
  if (not server->cycle())
  {
    Error << "Could not start up server " << *server;
    delete server;
    return false;
  }

  server->build(argc, argv);

  if (construct.is_debug())
  {
    Out << "Pausing for startup, hit return when ready.";
    std::string gdb_command= server->base_command();
    std::string options;
    Out << "run " << server->args(options);
    getchar();
  }
  else if (not server->start())
  {
    Error << "Failed to start " << *server;
    delete server;
    return false;
  }
  else
  {
    Out << "STARTING SERVER(pid:" << server->pid() << "): " << server->running();
  }

  construct.push_server(server);

  if (default_port() == 0)
  {
    assert(server->has_port());
    set_default_port(server->port());
  }

  Outn();

  return true;
}

bool server_startup_st::start_socket_server(const std::string& server_type, const in_port_t try_port, int argc, const char *argv[])
{
  (void)try_port;
  Outn();

  Server *server= NULL;
  if (0)
  { }
  else if (server_type.compare("gearmand") == 0)
  {
    Error << "Socket files are not supported for gearmand yet";
  }
  else if (server_type.compare("memcached-sasl") == 0)
  {
    if (MEMCACHED_SASL_BINARY)
    {
      if (HAVE_LIBMEMCACHED)
      {
        server= build_memcached_sasl_socket("localhost", try_port, username(), password());
      }
      else
      {
        Error << "Libmemcached was not found";
      }
    }
    else
    {
      Error << "No memcached binary is available";
    }
  }
  else if (server_type.compare("memcached") == 0)
  {
    if (MEMCACHED_BINARY)
    {
      if (HAVE_LIBMEMCACHED)
      {
        server= build_memcached_socket("localhost", try_port);
      }
      else
      {
        Error << "Libmemcached was not found";
      }
    }
    else
    {
      Error << "No memcached binary is available";
    }
  }
  else
  {
    Error << "Failed to start " << server_type << ", no support was found to be compiled in for it.";
  }

  if (server == NULL)
  {
    Error << "Failure occured while creating server: " <<  server_type;
    return false;
  }

  /*
    We will now cycle the server we have created.
  */
  if (not server->cycle())
  {
    Error << "Could not start up server " << *server;
    delete server;
    return false;
  }

  server->build(argc, argv);

  if (is_debug())
  {
    Out << "Pausing for startup, hit return when ready.";
    std::string gdb_command= server->base_command();
    std::string options;
    Out << "run " << server->args(options);
    getchar();
  }
  else if (not server->start())
  {
    Error << "Failed to start " << *server;
    delete server;
    return false;
  }
  else
  {
    Out << "STARTING SERVER(pid:" << server->pid() << "): " << server->running();
  }

  push_server(server);

  set_default_socket(server->socket().c_str());

  Outn();

  return true;
}

} // namespace libtest
