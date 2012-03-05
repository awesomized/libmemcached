/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011-2012 Data Differential, http://datadifferential.com/
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

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <syslog.h>

#define UTIL_MAX_ERROR_SIZE 2048

namespace datadifferential {
namespace util {

/** Verbosity levels.
 */
enum verbose_t
{
  // Logging this will cause shutdown
  VERBOSE_FATAL= LOG_EMERG, // syslog:LOG_EMERG

  VERBOSE_ALERT= LOG_ALERT, // syslog:LOG_ALERT
  VERBOSE_CRITICAL= LOG_CRIT, //  syslog:LOG_CRIT

  VERBOSE_ERROR= LOG_ERR, // syslog:LOG_ERR

  VERBOSE_WARN= LOG_WARNING, // syslog:LOG_WARNING

  VERBOSE_NOTICE= LOG_NOTICE, // syslog:LOG_NOTICE

  VERBOSE_INFO= LOG_INFO, // syslog:LOG_INFO

  VERBOSE_DEBUG= LOG_DEBUG // syslog:LOG_DEBUG
};


struct log_info_st
{
  std::string name;
  std::string filename;
  int fd;
  bool opt_syslog;
  bool opt_file;
  bool init_success;

  log_info_st(const std::string& name_arg, const std::string &filename_arg, bool syslog_arg) :
    name(name_arg),
    filename(filename_arg),
    fd(-1),
    opt_syslog(syslog_arg),
    opt_file(false),
    init_success(false)
  {
    if (opt_syslog)
    {
      openlog(name.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
    }

    init();
  }

  void init()
  {
    if (filename.size())
    {
      if (filename.compare("stderr") == 0)
      {
        fd= STDERR_FILENO;
      }
      else
      {
        fd= open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (fd == -1)
        {
          if (opt_syslog)
          {
            char buffer[1024];
            char *getcwd_ret= getcwd(buffer, sizeof(buffer));
            syslog(LOG_ERR, "Could not open log file \"%.*s\", from \"%s\", open failed with (%s)", 
                   int(filename.size()), filename.c_str(), 
                   getcwd_ret,
                   strerror(errno));
          }
          std::cerr << "Could not open log file for writing, switching to stderr." << std::endl;

          fd= STDERR_FILENO;
        }
      }

      opt_file= true;
    }

    init_success= true;
  }

  bool initialized() const
  {
    return init_success;
  }

  int file() const
  {
    return fd;
  }

  void write(verbose_t verbose, const char *format, ...)
  {
    if (opt_file or opt_syslog)
    {
      va_list args;
      va_start(args, format);
      char mesg[BUFSIZ];
      int mesg_length= vsnprintf(mesg, sizeof(mesg), format, args);
      va_end(args);

      if (opt_file)
      {
        char buffer[UTIL_MAX_ERROR_SIZE];
        int buffer_length= snprintf(buffer, sizeof(buffer), "%7s %.*s\n", verbose_name(verbose), mesg_length, mesg);
        if (::write(file(), buffer, buffer_length) == -1)
        {
          std::cerr << "Could not write to log file." << std::endl;
          syslog(LOG_EMERG, "gearmand could not open log file %s, got error %s", filename.c_str(), strerror(errno));
        }

      }

      if (opt_syslog)
      {
        syslog(int(verbose), "%7s %.*s", verbose_name(verbose), mesg_length, mesg);
      }
    }
  }

  ~log_info_st()
  {
    if (fd != -1 and fd != STDERR_FILENO)
    {
      close(fd);
    }

    if (opt_syslog)
    {
      closelog();
    }
  }

private:
  const char *verbose_name(verbose_t verbose)
  {
    switch (verbose)
    {
    case VERBOSE_FATAL:
      return "FATAL";

    case VERBOSE_ALERT:
      return "ALERT";

    case VERBOSE_CRITICAL:
      return "CRITICAL";

    case VERBOSE_ERROR:
      return "ERROR";

    case VERBOSE_WARN:
      return "WARNING";

    case VERBOSE_NOTICE:
      return "NOTICE";

    case VERBOSE_INFO:
      return "INFO";

    case VERBOSE_DEBUG:
      return "DEBUG";

    default:
      break;
    }

    return "UNKNOWN";
  }
};

} // namespace util
} // namespace datadifferential
