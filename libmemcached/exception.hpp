/*
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2009 Patrick Galbraith (patg@patg.net)
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in this directory for full text.
 */

/**
 * @file
 * @brief Exception declarations
 */

#ifndef LIBMEMACHED_EXCEPTION_HPP
#define LIBMEMACHED_EXCEPTION_HPP

#include <stdexcept>


namespace memcache {

  class Exception : public std::runtime_error
  {
  public:
    Exception(const std::string& msg, bool errno)
      : std::runtime_error(msg), _errno(errno) {}
    Exception(const char *msg, bool errno)
      : std::runtime_error(msg), _errno(errno) {}
    virtual ~Exception() throw() {}

    int getErrno() const { return _errno; }
  private:
    int _errno;

  };

  class Warning : public Exception
  {
  public:
    Warning(const std::string& msg, bool errno) : Exception(msg, errno) {}
    Warning(const char *msg, bool errno) : Exception(msg, errno) {}
  };

  class Error : public Exception
  {
  public:
    Error(const std::string& msg, bool errno) : Exception(msg, errno) {}
    Error(const char *msg, bool errno) : Exception(msg, errno) {}
    virtual ~Error() throw() {}
  };

} /* namespace libmemcached */

#endif /* LIBMEMACHED_EXCEPTION_HPP */
