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
uint32_t hashkit_digest(const hashkit_st *self, const char *key, size_t key_length);

/**
  This is a utilitly function provided so that you can directly access hashes with a hashkit_st.
*/

HASHKIT_API
uint32_t libhashkit_digest(const char *key, size_t key_length,
                           hashkit_hash_algorithm_t hash_algorithm);

#ifdef __cplusplus
}
#endif
