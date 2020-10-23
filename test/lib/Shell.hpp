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

#include "test/lib/common.hpp"

class Shell {
public:
  explicit Shell(bool redirect_stderr = true);
  explicit Shell(string prefix, bool redirect_stderr = true);
  bool run(const string &command, string &output);
  bool run(const string &command);

private:
  string prefix;
  bool redirect;

  string prepareCommand(const string &command_);
};
