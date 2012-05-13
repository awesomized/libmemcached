/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <config.h>
#include <libtest/common.h>
#include <cstdarg>

namespace libtest {

fatal::fatal(const char *file_arg, int line_arg, const char *func_arg, const char *format, ...) :
  std::runtime_error(func_arg),
  _line(line_arg),
  _file(file_arg),
  _func(func_arg)
  {
    va_list args;
    va_start(args, format);
    char last_error[BUFSIZ];
    int last_error_length= vsnprintf(last_error, sizeof(last_error), format, args);
    va_end(args);

    strncpy(_mesg, last_error, sizeof(_mesg));

    snprintf(_error_message, sizeof(_error_message), "%s:%d FATAL:%s (%s)", _file, int(_line), last_error, _func);
  }

static bool _disabled= false;
static uint32_t _counter= 0;

bool fatal::is_disabled()
{
  return _disabled;
}

void fatal::disable()
{
  _disabled= true;
}

void fatal::enable()
{
  _disabled= false;
}

uint32_t fatal::disabled_counter()
{
  return _counter;
}

void fatal::increment_disabled_counter()
{
  _counter++;
}

disconnected::disconnected(const char *file, int line, const char *func, const char *instance, const in_port_t port, const char *format, ...) :
  _port(port),
  std::runtime_error(func)
{
  strncpy(_instance, instance, sizeof(_instance));
  va_list args;
  va_start(args, format);
  char last_error[BUFSIZ];
  (void)vsnprintf(last_error, sizeof(last_error), format, args);
  va_end(args);

  snprintf(_error_message, sizeof(_error_message), "%s:%d FATAL:%s (%s)", file, int(line), last_error, func);
}

} // namespace libtest

