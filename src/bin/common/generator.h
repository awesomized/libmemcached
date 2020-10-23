/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

typedef struct pairs_st pairs_st;

struct pairs_st {
  char *key;
  size_t key_length;
  char *value;
  size_t value_length;
};

#ifdef __cplusplus
extern "C" {
#endif

pairs_st *pairs_generate(uint64_t number_of, size_t value_length);
void pairs_free(pairs_st *pairs);

#ifdef __cplusplus
} // extern "C"
#endif
