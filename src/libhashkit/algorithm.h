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

extern "C" {

uint32_t hashkit_one_at_a_time(const char *key, size_t key_length, void *context);

uint32_t hashkit_fnv1_64(const char *key, size_t key_length, void *context);

uint32_t hashkit_fnv1a_64(const char *key, size_t key_length, void *context);

uint32_t hashkit_fnv1_32(const char *key, size_t key_length, void *context);

uint32_t hashkit_fnv1a_32(const char *key, size_t key_length, void *context);

uint32_t hashkit_crc32(const char *key, size_t key_length, void *context);

uint32_t hashkit_hsieh(const char *key, size_t key_length, void *context);

uint32_t hashkit_murmur(const char *key, size_t key_length, void *context);

uint32_t hashkit_murmur3(const char *key, size_t key_length, void *context);

uint32_t hashkit_jenkins(const char *key, size_t key_length, void *context);

uint32_t hashkit_md5(const char *key, size_t key_length, void *context);

}
