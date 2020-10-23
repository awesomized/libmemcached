/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include "common.hpp"

class ForkAndExec {
public:
  enum mode { READ, WRITE };

  ForkAndExec(const char *binary, char **argv);
  ~ForkAndExec();

  ForkAndExec(const ForkAndExec &) = delete;
  ForkAndExec &operator=(const ForkAndExec &) = delete;
  ForkAndExec(ForkAndExec &&) = default;
  ForkAndExec &operator=(ForkAndExec &&) = default;

  [[nodiscard]] int createPipe();
  pid_t operator()();

private:
  int ready[2], pipes[2];
  const char *binary;
  char **argv;

  bool prepareExecReadyPipe();
  void prepareOutputPipe();
  void closePipe(int &fd);
  void pollExecReadyPipe();
};
