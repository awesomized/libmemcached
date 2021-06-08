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

#include <libhashkit-1.0/hashkit.h>
#include <string>

class Hashkit {
public:
  Hashkit() { hashkit_create(&self); }

  Hashkit(const Hashkit &source) { hashkit_clone(&self, &source.self); }

  Hashkit &operator=(const Hashkit &source) {
    hashkit_free(&self);
    hashkit_clone(&self, &source.self);

    return *this;
  }

  friend bool operator==(const Hashkit &left, const Hashkit &right) {
    return hashkit_compare(&left.self, &right.self);
  }

  uint32_t digest(std::string &str) { return hashkit_digest(&self, str.c_str(), str.length()); }

  uint32_t digest(const char *key, size_t key_length) {
    return hashkit_digest(&self, key, key_length);
  }

  hashkit_return_t set_function(hashkit_hash_algorithm_t hash_algorithm) {
    return hashkit_set_function(&self, hash_algorithm);
  }

  hashkit_return_t set_distribution_function(hashkit_hash_algorithm_t hash_algorithm) {
    return hashkit_set_distribution_function(&self, hash_algorithm);
  }

  ~Hashkit() { hashkit_free(&self); }

private:
  hashkit_st self;
};
