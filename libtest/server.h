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

#define SERVERS_TO_CREATE 5

struct server_st {
private:
  bool _used;
  pid_t _pid;
  in_port_t _port;
  char pid_file[FILENAME_MAX]; // Did we start it, or was it just sitting there?
public:

  char hostname[NI_MAXHOST];

  server_st() :
    _used(false),
    _pid(-1),
    _port(0)
  {
    pid_file[0]= 0;
    strncpy(hostname, "localhost", sizeof(hostname));
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
    return not _port == 0;
  }

  void set_used()
  {
    _used= true;
  }

  void set_pid(pid_t arg)
  {
    _pid= arg;
  }

  pid_t pid() const
  {
    return _pid;
  }

  bool is_used() const
  {
    return _used;
  }

  ~server_st();

  bool has_pid()
  {
    return _pid > 0;
  }

  bool is_socket() const
  {
    return hostname[0] == '/';
  }

  void set_hostname(const char *arg)
  {
    strncpy(hostname, arg, sizeof(hostname));
  }

  bool kill();

private:
  void reset_pid();
};

std::ostream& operator<<(std::ostream& output, const server_st &arg);

struct server_startup_st
{
  uint8_t count;
  uint8_t udp;
  std::string server_list;
  server_st server[SERVERS_TO_CREATE];

  server_startup_st() :
    count(SERVERS_TO_CREATE),
    udp(0)
  { }

  ~server_startup_st()
  { }
};

#ifdef	__cplusplus
extern "C" {
#endif


bool server_startup(server_startup_st *construct);
void server_shutdown(server_startup_st *construct);

#ifdef	__cplusplus
}
#endif
