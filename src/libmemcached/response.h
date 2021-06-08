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

/* Read a single response from the server */
memcached_return_t memcached_read_one_response(memcached_instance_st *ptr,
                                               memcached_result_st *result);

memcached_return_t memcached_response(memcached_instance_st *ptr, memcached_result_st *result);

memcached_return_t memcached_response(memcached_instance_st *ptr, char *buffer,
                                      size_t buffer_length, memcached_result_st *result);
