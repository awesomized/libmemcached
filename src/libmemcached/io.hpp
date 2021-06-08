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

void initialize_binary_request(memcached_instance_st *server, protocol_binary_request_header &);

bool memcached_io_write(memcached_instance_st *ptr);

ssize_t memcached_io_write(memcached_instance_st *ptr, const void *buffer, size_t length,
                           bool with_flush);

bool memcached_io_writev(memcached_instance_st *ptr, libmemcached_io_vector_st vector[],
                         const size_t number_of, const bool with_flush);

memcached_return_t memcached_io_wait_for_write(memcached_instance_st *);
memcached_return_t memcached_io_wait_for_read(memcached_instance_st *);

void memcached_io_reset(memcached_instance_st *ptr);

memcached_return_t memcached_io_read(memcached_instance_st *ptr, void *buffer, size_t length,
                                     ssize_t &nread);

/* Read a line (terminated by '\n') into the buffer */
memcached_return_t memcached_io_readline(memcached_instance_st *ptr, char *buffer_ptr, size_t size,
                                         size_t &total);

/* Read n bytes of data from the server and store them in dta */
memcached_return_t memcached_safe_read(memcached_instance_st *ptr, void *dta, const size_t size);

memcached_instance_st *memcached_io_get_readable_server(memcached_st *memc, memcached_return_t &);

memcached_return_t memcached_io_slurp(memcached_instance_st *ptr);

#define IO_POLL_CONNECT 0
memcached_return_t memcached_io_poll(memcached_instance_st *inst, int16_t events = IO_POLL_CONNECT, int prev_errno = 0);
