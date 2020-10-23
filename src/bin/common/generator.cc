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

#include "mem_config.h"

#include <stdint.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include "generator.h"

#define KEY_BYTES 20

/* Use this for string generation */
static const char ALPHANUMERICS[] = "0123456789ABCDEFGHIJKLMNOPQRSTWXYZabcdefghijklmnopqrstuvwxyz";

#define ALPHANUMERICS_SIZE (sizeof(ALPHANUMERICS) - 1)

static size_t get_alpha_num(void) {
  return (size_t) random() % ALPHANUMERICS_SIZE;
}

void get_random_string(char *buffer, size_t size) {
  char *buffer_ptr = buffer;

  while (--size) {
    *buffer_ptr++ = ALPHANUMERICS[get_alpha_num()];
  }
  *buffer_ptr++ = ALPHANUMERICS[get_alpha_num()];
}

void pairs_free(pairs_st *pairs) {
  if (pairs == NULL) {
    return;
  }

  /* We free until we hit the null pair we stores during creation */
  for (uint32_t x = 0; pairs[x].key; x++) {
    free(pairs[x].key);
    if (pairs[x].value) {
      free(pairs[x].value);
    }
  }

  free(pairs);
}

pairs_st *pairs_generate(uint64_t number_of, size_t value_length) {
  pairs_st *pairs = (pairs_st *) calloc((size_t) number_of + 1, sizeof(pairs_st));

  if (pairs == NULL) {
    goto error;
  }

  for (uint64_t x = 0; x < number_of; x++) {
    pairs[x].key = (char *) calloc(KEY_BYTES, sizeof(char));

    if (pairs[x].key == NULL)
      goto error;

    get_random_string(pairs[x].key, KEY_BYTES);
    pairs[x].key_length = KEY_BYTES;

    if (value_length) {
      pairs[x].value = (char *) calloc(value_length, sizeof(char));

      if (pairs[x].value == NULL)
        goto error;

      get_random_string(pairs[x].value, value_length);
      pairs[x].value_length = value_length;
    } else {
      pairs[x].value = NULL;
      pairs[x].value_length = 0;
    }
  }

  return pairs;
error:
  std::cerr << "Memory Allocation failure in pairs_generate." << std::endl;
  exit(EXIT_SUCCESS);
}
