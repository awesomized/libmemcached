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

memcached_instance_st *memcached_instance_list(const memcached_st *);

uint32_t memcached_instance_list_count(const memcached_st *);

uint32_t memcached_servers_set_count(memcached_server_list_st servers, uint32_t count);

void memcached_instance_list_free(memcached_instance_st *self, uint32_t count);

void memcached_instance_set(memcached_st *, memcached_instance_st *, const uint32_t host_list_size);
