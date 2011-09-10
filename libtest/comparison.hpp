/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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

#include <typeinfo>

#if defined(HAVE_LIBMEMCACHED) && HAVE_LIBMEMCACHED
#include <libmemcached/memcached.h>
#endif

#if defined(HAVE_LIBGEARMAN) && HAVE_LIBGEARMAN
#include <libgearman/gearman.h>
#endif

namespace libtest {

template <class T_comparable, class T_hint>
bool _compare_truth_hint(const char *file, int line, const char *func, T_comparable __expected, const char *assertation_label,  T_hint __hint)
{
  if (__expected == false)
  {
    libtest::stream::make_cerr(file, line, func) << "Assertation  \"" << assertation_label << "\" failed, hint: " << __hint;
    return false;
  }

  return true;
}

template <class T1_comparable, class T2_comparable>
bool _compare(const char *file, int line, const char *func, const T1_comparable __expected, const T2_comparable __actual)
{
  if (__expected != __actual)
  {
    if (typeid(__expected) == typeid(test_return_t))
    {
      const char *expected_str= test_strerror(test_return_t(__expected));
      const char *got_str= test_strerror(test_return_t(__actual));

      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << expected_str
        << "\" got \"" 
        << got_str
        << "\"";
    }
#if defined(HAVE_LIBMEMCACHED) && HAVE_LIBMEMCACHED
    else if (typeid(__expected) == typeid(memcached_return_t))
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << memcached_strerror(NULL, memcached_return_t(__expected)) 
        << "\" got \"" 
        << memcached_strerror(NULL, memcached_return_t(__actual)) << "\"";
    }
#endif
#if defined(HAVE_LIBGEARMAN) && HAVE_LIBGEARMAN
    else if (typeid(__expected) == typeid(gearman_return_t))
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << gearman_strerror(gearman_return_t(__expected)) 
        << "\" got \"" 
        << gearman_strerror(gearman_return_t(__actual)) << "\"";
    }
#endif
    else
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" << __expected << "\" got \"" << __actual << "\"";
    }
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

template <class T_comparable>
bool _truth(const char *file, int line, const char *func, T_comparable __truth)
{
  if (bool(__truth))
  {
    libtest::stream::make_cerr(file, line, func) << "Assertion failed for " << func << "() with \"" << __truth << "\"";
    return false;
  }

  return true;
}

template <class T1_comparable, class T2_comparable, class T_hint>
bool _compare_hint(const char *file, int line, const char *func, T1_comparable __expected, T2_comparable __actual, T_hint __hint)
{
  if (__expected != __actual)
  {
    if (typeid(__expected) == typeid(test_return_t))
    {
      const char *expected_str= test_strerror(test_return_t(__expected));
      const char *got_str= test_strerror(test_return_t(__actual));

      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << expected_str
        << "\" got \"" 
        << got_str
        << "\""
        << " Additionally: \"" << __hint << "\"";
    }
#if defined(HAVE_LIBMEMCACHED) && HAVE_LIBMEMCACHED
    else if (typeid(__expected) == typeid(memcached_return_t))
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << memcached_strerror(NULL, memcached_return_t(__expected)) 
        << "\" got \"" 
        << memcached_strerror(NULL, memcached_return_t(__actual)) << "\""
        << " Additionally: \"" << __hint << "\"";
    }
#endif
#if defined(HAVE_LIBGEARMAN) && HAVE_LIBGEARMAN
    else if (typeid(__expected) == typeid(gearman_return_t))
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" 
        << gearman_strerror(gearman_return_t(__expected)) 
        << "\" got \"" 
        << gearman_strerror(gearman_return_t(__actual)) << "\""
        << " Additionally: \"" << __hint << "\"";
    }
#endif
    else
    {
      libtest::stream::make_cerr(file, line, func) << "Expected \"" << __expected << "\" got \"" << __actual << "\""
        << " Additionally: \"" << __hint << "\"";
    }
    return false;
  }

  return true;
}

} // namespace libtest
