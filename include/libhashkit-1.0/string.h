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
struct hashkit_string_st;
#endif

#ifdef __cplusplus
extern "C" {
#endif

HASHKIT_API
void hashkit_string_free(hashkit_string_st *ptr);

HASHKIT_API
size_t hashkit_string_length(const hashkit_string_st *self);

HASHKIT_API
const char *hashkit_string_c_str(const hashkit_string_st *self);

#ifdef __cplusplus
} // extern "C"
#endif
