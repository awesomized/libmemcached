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
      dream(0, 50000);
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

bool Server::wait_for_pidfile() const
{
  Wait wait(pid_file(), 4);

  return wait.successful();
}

bool Server::start()
{
  // If we find that we already have a pid then kill it.
  if (has_pid() and kill(_pid) == false)
  {
    Error << "Could not kill() existing server during start() pid:" << _pid;
    return false;
  }

  if (has_pid() == false)
  {
    fatal_message("has_pid() failed, programer error");
  }

  Application app(executable(), is_libtool());

  if (is_debug())
  {
    app.use_gdb();
  }
  else if (getenv("TESTS_ENVIRONMENT"))
  {
    if (strstr(getenv("TESTS_ENVIRONMENT"), "gdb"))
    {
      app.use_gdb();
    }
  }

  if (args(app) == false)
  {
    Error << "Could not build command()";
    return false;
  }

  Application::error_t ret;
  if (Application::SUCCESS !=  (ret= app.run()))
  {
    Error << "Application::run() " << ret;
    return false;
  }
  _running= app.print();

  if (Application::SUCCESS !=  (ret= app.wait()))
  {
    Error << "Application::wait() " << app.print() << " " << ret;
    return false;
  }

  if (is_helgrind() or is_valgrind())
  {
    dream(5, 50000);
  }

  if (pid_file().empty() == false)
  {
    Wait wait(pid_file(), 8);

    if (wait.successful() == false)
    {
      libtest::fatal(LIBYATL_DEFAULT_PARAM,
                     "Unable to open pidfile for: %s",
                     _running.c_str());
    }
  }

  int counter= 0;
  bool pinged= false;
  while ((pinged= ping()) == false and
         counter < (is_helgrind() or is_valgrind() ? 20 : 5))
  {
    dream(counter++, 50000);
  }

  if (pinged == false)
  {
    // If we happen to have a pid file, lets try to kill it
    if (pid_file().empty() == false)
    {
      if (kill_file(pid_file()) == false)
      {
        fatal_message("Failed to kill off server after startup occurred, when pinging failed");
      }
      Error << "Failed to ping() server started, having pid_file. exec:" << _running;
    }
    else
    {
      Error << "Failed to ping() server started. exec:" << _running;
    }
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

void Server::add_option(const std::string& arg)
{
  _options.push_back(std::make_pair(arg, std::string()));
}

void Server::add_option(const std::string& name, const std::string& value)
{
  _options.push_back(std::make_pair(name, value));
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
    libtest::fatal(LIBYATL_DEFAULT_PARAM, "mkstemp() failed on %s with %s", file_buffer, strerror(errno));
  }
  close(fd);

  _log_file= file_buffer;

  return true;
}

bool Server::args(Application& app)
{

  // Set a log file if it was requested (and we can)
  if (has_log_file_option())
  {
    set_log_file();
    log_file_option(app, _log_file);
  }

  if (getenv("LIBTEST_SYSLOG") and has_syslog())
  {
    app.add_option("--syslog");
  }

  // Update pid_file
  {
    if (_pid_file.empty() and set_pid_file() == false)
    {
      return false;
    }

    pid_file_option(app, pid_file());
  }

  assert(daemon_file_option());
  if (daemon_file_option() and not is_valgrind() and not is_helgrind())
  {
    app.add_option(daemon_file_option());
  }

  if (has_socket_file_option())
  {
    if (set_socket_file() == false)
    {
      return false;
    }

    socket_file_option(app, _socket);
  }

  if (has_port_option())
  {
    port_option(app, _port);
  }

  for (Options::const_iterator iter= _options.begin(); iter != _options.end(); iter++)
  {
    if ((*iter).second.empty() == false)
    {
      app.add_option((*iter).first, (*iter).second);
    }
    else
    {
      app.add_option((*iter).first);
    }
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
    if (broken_pid_file() and pid_file().empty() == false)
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

} // namespace libtest
