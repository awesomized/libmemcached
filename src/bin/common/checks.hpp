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

#include "options.hpp"
#include "libmemcached/common.h"

#include <cerrno>
#include <cstring>
#include <fstream>

bool check_buffering(const client_options &opt, memcached_st &memc) {
  if (opt.isset("buffer")) {
    if (MEMCACHED_SUCCESS != memcached_flush_buffers(&memc)) {
      if (!opt.isset("quiet")) {
        std::cerr << memcached_last_error_message(&memc) << "\n";
      }
      return false;
    }
  }
  return true;
}

bool check_argp(const client_options &opt, char **argp, const char *error_msg) {
  if (argp && *argp) {
    return true;
  }
  if (!opt.isset("quiet")) {
    std::cerr << error_msg << "\n";
  }
  return false;
}

bool check_memcached(const client_options &opt, memcached_st &memc) {
  if (!memcached_create(&memc)) {
    if (!opt.isset("quiet")) {
      std::cerr << "Failed to initialize memcached client.\n";
    }
    return false;
  }
  return true;
}

bool check_return(const client_options &opt, memcached_st &memc, memcached_return_t rc) {
  if (!memcached_success(rc)) {
    if (!opt.isset("quiet")) {
      if (!memcached_fatal(rc)) {
        if (opt.isset("verbose")) {
          std::cerr << "Failed: " << memcached_strerror(&memc, rc) << "\n";;
        }
      } else {
        std::cerr << "Fatal error: " << memcached_last_error_message(&memc) << "\n";
      }
    }
    return false;
  }
  return true;
}

bool check_return(const client_options &opt, memcached_st &memc, const char *key,
                  memcached_return_t rc) {
  if (!memcached_success(rc)) {
    if (!opt.isset("quiet")) {
      if (MEMCACHED_NOTFOUND == rc) {
        if (opt.isset("verbose")) {
          std::cerr << "Could not find key '" << key
                    << "': " << memcached_strerror(&memc, rc) << "\n";;
        }
      } else {
        std::cerr << "Fatal error for key '" << key
                  << "': " << memcached_last_error_message(&memc) << "\n";
      }
    }
    return false;
  }
  return true;
}

std::ostream *check_ostream(const client_options &opt, const char *file, std::ofstream &stream) {
  if (file && *file) {
    if (opt.isset("debug")) {
      std::cerr << "Opening '" << file << "' for writing.\n";
    }
    errno = 0;
    stream.open(file, std::ios::binary | std::ios::out);
    if (stream.is_open()) {
      return &stream;
    } else if (!opt.isset("quiet")) {
      std::cerr << "Failed to open '" << file << "' for writing: " << strerror(errno) << ".\n";
    }
  }
  return &std::cout;
}

std::istream *check_istream(const client_options &opt, const char *file, std::ifstream &stream) {
  if (file && *file) {
    if (file[0] != '-' || file[1] != 0) {
      if (opt.isset("debug")) {
        std::cerr << "Opening '" << file << "' for reading.\n";
      }
      errno = 0;
      stream.open(file, std::ios::binary | std::ios::in);
      if (stream.is_open()) {
        return &stream;
      } else if (!opt.isset("quiet")) {
        std::cerr << "Failed to open '" << file << "' for reading: " << strerror(errno) << ".\n";
      }
      return nullptr;
    }
  }
  return &std::cin;
}
