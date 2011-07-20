/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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

#pragma once

namespace libtest {

template <class T_comparable, class T_hint>
bool _compare_true_hint(const char *file, int line, const char *func, T_comparable __expected, const char *assertation_label,  T_hint __hint)
{
  if (__expected == false)
  {
    libtest::stream::make_cerr(file, line, func) << "Assertation  \"" << assertation_label << "\" failed, hint: " << __hint;
    return false;
  }

  return true;
}

template <class T_comparable>
bool _compare(const char *file, int line, const char *func, T_comparable __expected, T_comparable __actual)
{
  if (__expected != __actual)
  {
    libtest::stream::make_cerr(file, line, func) << "Expected \"" << __expected << "\" got \"" << __actual << "\"";
    return false;
  }

  return true;
}

template <class T_comparable>
bool _compare_zero(const char *file, int line, const char *func, T_comparable __actual)
{
  if (T_comparable(0) != __actual)
  {
    libtest::stream::make_cerr(file, line, func) << "Expected 0 got \"" << __actual << "\"";
    return false;
  }

  return true;
}

template <class T_comparable, class T_hint>
bool _compare_hint(const char *file, int line, const char *func, T_comparable __expected, T_comparable __actual, T_hint __hint)
{
  if (__expected != __actual)
  {
    libtest::stream::make_cerr(file, line, func) << "Expected \"" << __expected << "\" got \"" << __actual << "\" Additionally: \"" << __hint << "\"";

    return false;
  }

  return true;
}

} // namespace libtest
