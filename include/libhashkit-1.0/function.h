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

/**
  This sets/gets the default function we will be using.
*/
HASHKIT_API
hashkit_return_t hashkit_set_function(hashkit_st *hash, hashkit_hash_algorithm_t hash_algorithm);

HASHKIT_API
hashkit_return_t hashkit_set_custom_function(hashkit_st *hash, hashkit_hash_fn function,
                                             void *context);

HASHKIT_API
hashkit_hash_algorithm_t hashkit_get_function(const hashkit_st *hash);

/**
  This sets/gets the function we use for distribution.
*/
HASHKIT_API
hashkit_return_t hashkit_set_distribution_function(hashkit_st *hash,
                                                   hashkit_hash_algorithm_t hash_algorithm);

HASHKIT_API
hashkit_return_t hashkit_set_custom_distribution_function(hashkit_st *self,
                                                          hashkit_hash_fn function, void *context);

HASHKIT_API
hashkit_hash_algorithm_t hashkit_get_distribution_function(const hashkit_st *self);

#ifdef __cplusplus
}
#endif
