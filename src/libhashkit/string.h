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

hashkit_string_st *hashkit_string_create(size_t initial_size);

bool hashkit_string_append_character(hashkit_string_st *string, char character);

bool hashkit_string_append(hashkit_string_st *string, const char *value, size_t length);

char *hashkit_string_c_copy(hashkit_string_st *string);

void hashkit_string_reset(hashkit_string_st *string);

bool hashkit_string_resize(hashkit_string_st &string, const size_t need);

size_t hashkit_string_max_size(const hashkit_string_st *self);

char *hashkit_string_take(hashkit_string_st *self);

char *hashkit_string_c_str_mutable(hashkit_string_st *self);

void hashkit_string_set_length(hashkit_string_st *self, size_t length);
