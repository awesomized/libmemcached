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

namespace libtest {
namespace http {

#define YATL_USERAGENT "YATL/1.0"

extern "C" size_t
  http_get_result_callback(void *ptr, size_t size, size_t nmemb, void *data)
  {
    size_t body_size= size * nmemb;

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

bool GET::execute()
{
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
