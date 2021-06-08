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

/**
  @note The following definitions are just here for backwards compatibility.
*/
typedef memcached_return_t memcached_return;
typedef memcached_server_distribution_t memcached_server_distribution;
typedef memcached_behavior_t memcached_behavior;
typedef memcached_callback_t memcached_callback;
typedef memcached_hash_t memcached_hash;
typedef memcached_connection_t memcached_connection;
typedef memcached_clone_fn memcached_clone_func;
typedef memcached_cleanup_fn memcached_cleanup_func;
typedef memcached_execute_fn memcached_execute_function;
typedef memcached_server_fn memcached_server_function;
typedef memcached_trigger_key_fn memcached_trigger_key;
typedef memcached_trigger_delete_key_fn memcached_trigger_delete_key;
typedef memcached_dump_fn memcached_dump_func;
typedef memcached_instance_st *memcached_server_instance_st;
