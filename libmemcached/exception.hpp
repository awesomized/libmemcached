/*
 * Summary: Exceptions for the C++ interface
 *
 * Copy: See Copyright for the status of this software.
 *
 */

/**
 * @file
 * @brief Exception declarations
 */

#ifndef LIBMEMACHED_EXCEPTION_HPP
#define LIBMEMACHED_EXCEPTION_HPP

#include <stdexcept>


namespace memcache 
{
  class Exception : public std::runtime_error
  {
  public:
    Exception(const std::string& msg, bool in_errno)
      : 
        std::runtime_error(msg), 
        errno(in_errno) 
    {}

    Exception(const char *msg, bool in_errno)
      : 
        std::runtime_error(msg), 
        errno(in_errno) {}

    virtual ~Exception() throw() {}

    int getErrno() const 
    { 
      return errno; 
    }

  private:
    int errno;
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
