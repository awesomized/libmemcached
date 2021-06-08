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

#ifdef __cplusplus
extern "C" {
#endif

HASHKIT_API
uint32_t libhashkit_one_at_a_time(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_fnv1_64(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_fnv1a_64(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_fnv1_32(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_fnv1a_32(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_crc32(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_hsieh(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_murmur(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_murmur3(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_jenkins(const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_md5(const char *key, size_t key_length);

HASHKIT_API
void libhashkit_md5_signature(const unsigned char *key, size_t length, unsigned char *result);

#ifdef __cplusplus
}
#endif
