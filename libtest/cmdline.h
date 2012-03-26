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

#pragma once

#include <spawn.h>

namespace libtest {

class Application {
private:
  typedef std::vector< std::pair<std::string, std::string> > Options;

public:

  enum error_t {
    SUCCESS= EXIT_SUCCESS,
    FAILURE= EXIT_FAILURE,
    INVALID= 127
  };

  class Pipe {
  public:
    Pipe();
    ~Pipe();

    int* fd()
    {
      return _fd;
    }

    enum close_t {
      READ,
      WRITE
    };

    void reset();
    void close(const close_t& arg);
    void dup_for_spawn(const close_t& arg,
                       posix_spawn_file_actions_t& file_actions,
                       const int newfildes);

    void nonblock();

  private:
    int _fd[2];
    bool _open[2];
  };

public:
  Application(const std::string& arg, const bool _use_libtool_arg= false);

  virtual ~Application();

  void add_option(const std::string&);
  void add_option(const std::string&, const std::string&);
  void add_long_option(const std::string& option_name, const std::string& option_value);
  error_t run(const char *args[]= NULL);
  error_t wait(bool nohang= true);

  libtest::vchar_t stdout_result() const
  {
    return _stdout_buffer;
  }

  size_t stdout_result_length() const
  {
    return _stdout_buffer.size();
  }

  libtest::vchar_t stderr_result() const
  {
    return _stderr_buffer;
  }

  size_t stderr_result_length() const
  {
    return _stderr_buffer.size();
  }

  std::string print();

  void use_valgrind(bool arg= true)
  {
    _use_valgrind= arg;
  }

  bool check() const;

  bool slurp();
  void murder();

  void use_gdb(bool arg= true)
  {
    _use_gdb= arg;
  }

  std::string arguments();

  std::string gdb_filename()
  {
    return  _gdb_filename;
  }

  pid_t pid() const
  {
    return _pid;
  }

private:
  void create_argv(const char *args[]);
  void delete_argv();

private:
  const bool _use_libtool;
  bool _use_valgrind;
  bool _use_gdb;
  size_t _argc;
  std::string _exectuble_name;
  std::string _exectuble;
  std::string _exectuble_with_path;
  std::string _gdb_filename;
  Options _options;
  Pipe stdin_fd;
  Pipe stdout_fd;
  Pipe stderr_fd;
  char * * built_argv;
  pid_t _pid;
  libtest::vchar_t _stdout_buffer;
  libtest::vchar_t _stderr_buffer;
};

static inline std::ostream& operator<<(std::ostream& output, const enum Application::error_t &arg)
{
  switch (arg)
  {
    case Application::SUCCESS:
      output << "EXIT_SUCCESS";
      break;

    case Application::FAILURE:
      output << "EXIT_FAILURE";
      break;

    case Application::INVALID:
      output << "127";
      break;

    default:
      output << "EXIT_UNKNOWN";
  }

  return output;
}

int exec_cmdline(const std::string& executable, const char *args[], bool use_libtool= false);

const char *gearmand_binary(); 

}
