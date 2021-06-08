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

struct memcached_st;
struct memcached_stat_st;
struct memcached_analysis_st;
struct memcached_result_st;
struct memcached_array_st;
struct memcached_error_t;

// All of the flavors of memcache_server_st
struct memcached_server_st;
struct memcached_instance_st;
typedef struct memcached_instance_st memcached_instance_st;
typedef struct memcached_server_st *memcached_server_list_st;

struct memcached_callback_st;

// The following two structures are internal, and never exposed to users.
struct memcached_string_st;
struct memcached_string_t;
struct memcached_continuum_item_st;

#else

typedef struct memcached_st memcached_st;
typedef struct memcached_stat_st memcached_stat_st;
typedef struct memcached_analysis_st memcached_analysis_st;
typedef struct memcached_result_st memcached_result_st;
typedef struct memcached_array_st memcached_array_st;
typedef struct memcached_error_t memcached_error_t;

// All of the flavors of memcache_server_st
typedef struct memcached_server_st memcached_server_st;
typedef struct memcached_instance_st memcached_instance_st;
typedef struct memcached_server_st *memcached_server_list_st;

typedef struct memcached_callback_st memcached_callback_st;

// The following two structures are internal, and never exposed to users.
typedef struct memcached_string_st memcached_string_st;
typedef struct memcached_string_t memcached_string_t;

#endif
