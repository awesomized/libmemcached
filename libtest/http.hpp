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

namespace libtest {
namespace http {

class HTTP {
public:

  HTTP(const std::string& url_arg);

  virtual bool execute()= 0;

  virtual ~HTTP()
  { }

  const std::string& url() const
  {
    return _url;
  }

  long response()
  {
    return _response;
  }

private:
  std::string _url;

protected:
  long _response;
};

class GET: public HTTP {
public:

  GET(const std::string& url_arg) :
    HTTP(url_arg)
  {
  }

  bool execute();

private:
  libtest::vchar_t _body;
};

class POST: public HTTP {
public:

  POST(const std::string& url_arg,
       const vchar_t& post_arg) :
    HTTP(url_arg),
    _post(post_arg)
  {
  }

  bool execute();

private:
  libtest::vchar_t _post;
  libtest::vchar_t _body;
};

class TRACE: public HTTP {
public:

  TRACE(const std::string& url_arg,
        const vchar_t& body_arg) :
    HTTP(url_arg),
    _body(body_arg)
  {
  }

  bool execute();

private:
  libtest::vchar_t _body;
};

class HEAD: public HTTP {
public:

  HEAD(const std::string& url_arg) :
    HTTP(url_arg)
  {
  }

  bool execute();

private:
};

} // namespace http
} // namespace libtest
