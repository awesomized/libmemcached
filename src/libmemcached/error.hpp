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

#include "libmemcached/common.h"

#pragma once

#ifdef __cplusplus

#  define STRINGIFY(x) #  x
#  define TOSTRING(x)  STRINGIFY(x)
#  define MEMCACHED_AT __FILE__ ":" TOSTRING(__LINE__)

memcached_return_t memcached_set_parser_error(Memcached &memc, const char *at, const char *format,
                                              ...);

memcached_return_t memcached_set_error(Memcached &, memcached_return_t rc, const char *at);

memcached_return_t memcached_set_error(memcached_instance_st &, memcached_return_t rc,
                                       const char *at);

memcached_return_t memcached_set_error(Memcached &, memcached_return_t rc, const char *at,
                                       const char *str, size_t length);

memcached_return_t memcached_set_error(memcached_instance_st &, memcached_return_t rc,
                                       const char *at, const char *str, size_t length);

memcached_return_t memcached_set_error(Memcached &memc, memcached_return_t rc, const char *at,
                                       memcached_string_t &str);

memcached_return_t memcached_set_error(memcached_instance_st &, memcached_return_t rc,
                                       const char *at, memcached_string_t &str);

memcached_return_t memcached_set_errno(Memcached &memc, int local_errno, const char *at,
                                       memcached_string_t &str);

memcached_return_t memcached_set_errno(memcached_instance_st &, int local_errno, const char *at,
                                       memcached_string_t &str);

memcached_return_t memcached_set_errno(Memcached &memc, int local_errno, const char *at,
                                       const char *str, size_t length);

memcached_return_t memcached_set_errno(memcached_instance_st &, int local_errno, const char *at,
                                       const char *str, size_t length);

memcached_return_t memcached_set_errno(Memcached &memc, int local_errno, const char *at);

memcached_return_t memcached_set_errno(memcached_instance_st &, int local_errno, const char *at);

bool memcached_has_current_error(Memcached &);

bool memcached_has_current_error(memcached_instance_st &);

void memcached_error_free(Memcached &);

void memcached_error_free(memcached_server_st &);

void memcached_error_free(memcached_instance_st &self);

memcached_error_t *memcached_error_copy(const memcached_instance_st &);

memcached_return_t memcached_instance_error_return(memcached_instance_st *);

#endif
