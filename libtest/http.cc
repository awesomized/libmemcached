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

#include <config.h>

#include <libtest/common.h>

#if defined(HAVE_CURL_CURL_H) && HAVE_CURL_CURL_H
#include <curl/curl.h>
#else
class CURL;
#endif


static void cleanup_curl(void)
{
#if defined(HAVE_CURL_CURL_H) && HAVE_CURL_CURL_H
  curl_global_cleanup();
#endif
}

static void initialize_curl_startup()
{
#if defined(HAVE_CURL_CURL_H) && HAVE_CURL_CURL_H
  if (curl_global_init(CURL_GLOBAL_ALL))
  {
    fatal_message("curl_global_init(CURL_GLOBAL_ALL) failed");
  }
#endif

  if (atexit(cleanup_curl))
  {
    fatal_message("atexit() failed");
  }
}

static pthread_once_t start_key_once= PTHREAD_ONCE_INIT;
void initialize_curl(void)
{
  int ret;
  if ((ret= pthread_once(&start_key_once, initialize_curl_startup)) != 0)
  {
    fatal_message(strerror(ret));
  }
}

namespace libtest {
namespace http {

#define YATL_USERAGENT "YATL/1.0"

extern "C" size_t
  http_get_result_callback(void *ptr, size_t size, size_t nmemb, void *data)
  {
    vchar_t *_body= (vchar_t*)data;

    _body->resize(size * nmemb);
    memcpy(&_body[0], ptr, _body->size());

    return _body->size();
  }


static void init(CURL *curl, const std::string& url)
{
  if (HAVE_LIBCURL)
  {
#if defined(HAVE_LIBCURL) && HAVE_LIBCURL
    assert(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, YATL_USERAGENT);
#endif
  }
}

HTTP::HTTP(const std::string& url_arg) :
  _url(url_arg),
  _response(0)
{
  initialize_curl();
}

bool GET::execute()
{
  (void)init;

  if (HAVE_LIBCURL)
  {
#if defined(HAVE_LIBCURL) && HAVE_LIBCURL
    CURL *curl= curl_easy_init();

    init(curl, url());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_result_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&_body);

    CURLcode retref= curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, _response);

    curl_easy_cleanup(curl);

    return retref == CURLE_OK;
#endif
  }

  return false;
}

bool POST::execute()
{
  if (HAVE_LIBCURL)
  {
#if defined(HAVE_LIBCURL) && HAVE_LIBCURL
    CURL *curl= curl_easy_init();;

    init(curl, url());

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, _body.size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (void *)&_body[0]);

    CURLcode retref= curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, _response);

    curl_easy_cleanup(curl);
#endif
  }

  return false;
}

bool TRACE::execute()
{
  if (HAVE_LIBCURL)
  {
#if defined(HAVE_LIBCURL) && HAVE_LIBCURL
    CURL *curl= curl_easy_init();;

    init(curl, url());

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "TRACE");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_result_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&_body[0]);

    CURLcode retref= curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, _response);

    curl_easy_cleanup(curl);

    return retref == CURLE_OK;
#endif
  }

  return false;
}

bool HEAD::execute()
{
  if (HAVE_LIBCURL)
  {
#if defined(HAVE_LIBCURL) && HAVE_LIBCURL
    CURL *curl= curl_easy_init();;

    init(curl, url());

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_result_callback);

    CURLcode retref= curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, _response);

    curl_easy_cleanup(curl);

    return retref == CURLE_OK;
#endif
  }

  return false;
}

} // namespace http
} // namespace libtest
