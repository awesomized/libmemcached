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
void memcached_set_configuration_file(memcached_st *self, const char *filename,
                                      size_t filename_length);

LIBMEMCACHED_LOCAL
const char *memcached_parse_filename(memcached_st *memc);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_parse_configuration(memcached_st *ptr, const char *option_string,
                                                 size_t length);

LIBMEMCACHED_LOCAL
size_t memcached_parse_filename_length(memcached_st *memc);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_parse_configure_file(memcached_st &, const char *filename,
                                                  size_t length);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_parse_configure_file(memcached_st &, memcached_array_st &filename);
