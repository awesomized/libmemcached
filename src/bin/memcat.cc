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

#define PROGRAM_NAME        "memcat"
#define PROGRAM_DESCRIPTION "Cat a set of key values to stdout."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"

#include <iostream>
#include <fstream>

memcached_return_t memcat(const client_options &opt, memcached_st *memc, const char *key, std::ostream &ref) {
  memcached_return_t rc;
  uint32_t flags;
  size_t len;
  auto val = memcached_get(memc, key, strlen(key), &len, &flags, &rc);

  if (MEMCACHED_SUCCESS == rc) {
    if (opt.isset("verbose")) {
      ref << "key: " << key << "\n";
    }
    if (opt.isset("flags")) {
      ref << "flags: " << flags << "\n";
    }
    if (opt.isset("verbose")) {
      ref << "value: ";
    }
    ref.write(val, len);
    ref << std::endl;
  }

  if (val) {
    free(val);
  }
  return rc;
}

int main(int argc, char *argv[]) {
  char **argp = nullptr;
  memcached_st memc;
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, "key [ key ... ]"};

  for (const auto &def : opt.defaults) {
    opt.add(def);
  }
  opt.add("flags", 'F', no_argument, "Display key flags, too.");
  opt.add("file", 'f', required_argument, "Output to file instead of standard output.");

  if (!opt.parse(argc, argv, &argp)) {
    exit(EXIT_FAILURE);
  }

  if (opt.isset("quiet") && !opt.isset("file")) {
    std::cerr << "--quiet operation was requested, but --file was not set.\n";
    exit(EXIT_FAILURE);
  }

  if (!memcached_create(&memc)) {
    if (!opt.isset("quiet")) {
      std::cerr << "Failed to initialize memcached client.\n";
    }
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  if (!*argp) {
    if (!opt.isset("quiet")) {
      std::cerr << "No key(s) provided.\n";
    }
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto arg = argp; *arg; ++arg) {
    auto key = *arg;
    if (*key) {
      memcached_return_t rc;
      auto file = opt.argof("file");
      if (file && *file) {
        std::ofstream stream{file, std::ios::binary};
        rc = memcat(opt, &memc, key, stream);
      } else {
        rc = memcat(opt, &memc, key, std::cout);
      }
      if (MEMCACHED_SUCCESS != rc) {
        exit_code = EXIT_FAILURE;

        if (MEMCACHED_NOTFOUND == rc) {
          if (opt.isset("verbose")) {
            std::cerr << "not found: " << key << "\n";
          }
          // continue;
        } else {
          if (!opt.isset("quiet")) {
            std::cerr << memcached_last_error_message(&memc) << "\n";
          }
          break;
        }
      }
    }
  }

  memcached_free(&memc);
  exit(exit_code);
}
