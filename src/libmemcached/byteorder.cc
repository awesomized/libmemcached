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

#include "mem_config.h"

#if HAVE_HTONLL && HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#endif

#include "libmemcached/byteorder.h"

/* Byte swap a 64-bit number. */
#ifndef swap64
#  ifndef HAVE_HTONLL
static inline uint64_t swap64(uint64_t in) {
#    if !WORDS_BIGENDIAN
  /* Little endian, flip the bytes around until someone makes a faster/better
   * way to do this. */
  uint64_t rv = 0;
  for (uint8_t x = 0; x < 8; ++x) {
    rv = (rv << 8) | (in & 0xff);
    in >>= 8;
  }
  return rv;
#    else
  /* big-endian machines don't need byte swapping */
  return in;
#    endif // WORDS_BIGENDIAN
}
#  endif
#endif

#include <sys/types.h>

uint64_t memcached_ntohll(uint64_t value) {
#ifdef HAVE_HTONLL
  return ntohll(value);
#else
  return swap64(value);
#endif
}

uint64_t memcached_htonll(uint64_t value) {
#ifdef HAVE_HTONLL
  return htonll(value);
#else
  return swap64(value);
#endif
}
