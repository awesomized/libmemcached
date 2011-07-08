/*
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once

#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <vector>

#define SERVERS_TO_CREATE 5

struct server_st;

typedef pid_t (test_server_getpid)(server_st &);
typedef bool (test_server_ping)(server_st &);

struct server_st {
private:
  bool _used;
  pid_t _pid;
  in_port_t _port;
  char pid_file[FILENAME_MAX]; // Did we start it, or was it just sitting there?
  std::string _command;
  test_server_getpid *__get_pid;
  test_server_ping *__ping;
  std::string _hostname;

public:
  server_st(in_port_t port_arg, test_server_getpid *, test_server_ping *);

  server_st(const std::string &socket_file, test_server_getpid *, test_server_ping *);

  void set_methods(test_server_getpid *get_pid_arg, test_server_ping *ping_arg)
  {
    __get_pid= get_pid_arg;
    __ping= ping_arg;
  }

  const char *hostname() const
  {
    if (_hostname.empty())
      return "";

    return _hostname.c_str();
  }

  bool ping()
  {
    if (__ping)
      return __ping(*this);

    return false;
  }

  pid_t get_pid()
  {
    if (__get_pid)
      return _pid= __get_pid(*this);

    return -1;
  }

  void set_port(in_port_t arg)
  {
    _port= arg;
  }

  in_port_t port() const
  {
    return _port;
  }

  bool has_port() const
  {
    return (_port != 0);
  }

  void set_command(const char *arg)
  {
    _command= arg;
  }

  void set_used()
  {
    _used= true;
  }

  pid_t pid();

  bool is_used() const
  {
    return _used;
  }

  ~server_st();

  bool has_pid()
  {
    return (_pid > 1);
  }

  bool is_socket() const
  {
    return _hostname[0] == '/';
  }

  bool kill();
  bool start();

private:
  void reset_pid();
};

std::ostream& operator<<(std::ostream& output, const server_st &arg);

struct server_startup_st
{
  uint32_t count;
  uint8_t udp;
  std::string server_list;
  std::vector<server_st *> servers;

  server_startup_st() :
    count(SERVERS_TO_CREATE),
    udp(0)
  { }

  void shutdown();
  void push_server(server_st *);

  ~server_startup_st();
};

#ifdef	__cplusplus
extern "C" {
#endif


bool server_startup(server_startup_st *construct);
void server_shutdown(server_startup_st *construct);

#ifdef	__cplusplus
}
#endif
