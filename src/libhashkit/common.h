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

#pragma once

#include "libhashkit/hashkitcon.h"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ciso646>

#ifndef __WORDSIZE
#  ifdef __MINGW32__
#    define __WORDSIZE 32
#  endif
#endif

#include "libhashkit-1.0/hashkit.h"
#include "libhashkit/algorithm.h"
#include "libhashkit/is.h"
#include "libhashkit/string.h"
#include "libhashkit/aes.h"

#ifdef __cplusplus
extern "C" {
#endif

void md5_signature(const unsigned char *key, unsigned int length, unsigned char *result);

int update_continuum(hashkit_st *hashkit);

#ifdef __cplusplus
}
#endif
