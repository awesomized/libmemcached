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

#include "mem_config.h"

#include <cstdio>
#include <cstring>
#include <iostream>

#include "libmemcached-1.0/memcached.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "No arguments provided." << std::endl;
    return EXIT_FAILURE;
  }

  for (int x = 1; x < argc; x++) {
    char buffer[BUFSIZ];
    memcached_return_t rc;
    rc = libmemcached_check_configuration(argv[x], strlen(argv[x]), buffer, sizeof(buffer));

    if (rc != MEMCACHED_SUCCESS) {
      std::cerr << "Failed to parse argument #" << x << " " << argv[x] << std::endl;
      std::cerr << buffer << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
