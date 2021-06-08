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

#define memcached_literal_param(str)          (str),strlen(str)
#define memcached_literal_param_size(str)     strlen(str)
#define memcached_string_make_from_cstr(str)  (str),((str)?strlen(str):0)

/**
  Strings are always under our control so we make some assumptions
  about them.

  1) is_initialized is always valid.
  2) A string once intialized will always be, until free where we
     unset this flag.
  3) A string always has a root.
*/

memcached_string_st *memcached_string_create(memcached_st *ptr, memcached_string_st *string,
                                             size_t initial_size);

memcached_return_t memcached_string_check(memcached_string_st *string, size_t need);

char *memcached_string_c_copy(memcached_string_st *string);

memcached_return_t memcached_string_append_character(memcached_string_st *string, char character);

memcached_return_t memcached_string_append(memcached_string_st *string, const char *value,
                                           size_t length);

void memcached_string_reset(memcached_string_st *string);

void memcached_string_free(memcached_string_st *string);
void memcached_string_free(memcached_string_st &);

size_t memcached_string_length(const memcached_string_st *self);
size_t memcached_string_length(const memcached_string_st &);

size_t memcached_string_size(const memcached_string_st *self);

const char *memcached_string_value(const memcached_string_st *self);
const char *memcached_string_value(const memcached_string_st &);

char *memcached_string_take_value(memcached_string_st *self);

char *memcached_string_value_mutable(const memcached_string_st *self);

bool memcached_string_set(memcached_string_st &, const char *, size_t);

void memcached_string_set_length(memcached_string_st *self, size_t length);
void memcached_string_set_length(memcached_string_st &, const size_t length);

bool memcached_string_resize(memcached_string_st &, const size_t);
char *memcached_string_c_str(memcached_string_st &);
