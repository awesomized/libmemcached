/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#pragma once

#include <stdexcept>
#include <string>

namespace memcache {
class Exception : public std::runtime_error {
public:
  Exception(const std::string &msg, int in_errno)
  : std::runtime_error(msg)
  , _errno(in_errno) {}

  Exception(const char *msg, int in_errno)
  : std::runtime_error(std::string(msg))
  , _errno(in_errno) {}

  virtual ~Exception() throw() {}

  int getErrno() const { return _errno; }

private:
  int _errno;
};

class Warning : public Exception {
public:
  Warning(const std::string &msg, int in_errno)
  : Exception(msg, in_errno) {}
  Warning(const char *msg, int in_errno)
  : Exception(msg, in_errno) {}
};

class Error : public Exception {
public:
  Error(const std::string &msg, int in_errno)
  : Exception(msg, in_errno) {}
  Error(const char *msg, int in_errno)
  : Exception(msg, in_errno) {}
  virtual ~Error() throw() {}
};

} // namespace memcache
