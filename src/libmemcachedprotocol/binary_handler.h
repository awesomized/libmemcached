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

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_request(
    const protocol_binary_request_header *request);

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_response(
    const protocol_binary_request_header *request, const protocol_binary_response_header *response);

LIBMEMCACHED_LOCAL
memcached_protocol_event_t
memcached_binary_protocol_process_data(memcached_protocol_client_st *client, ssize_t *length,
                                       void **endptr);
