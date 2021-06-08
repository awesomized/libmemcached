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

enum memcached_callback_t {
  MEMCACHED_CALLBACK_PREFIX_KEY = 0,
  MEMCACHED_CALLBACK_USER_DATA = 1,
  MEMCACHED_CALLBACK_CLEANUP_FUNCTION = 2,
  MEMCACHED_CALLBACK_CLONE_FUNCTION = 3,
  MEMCACHED_CALLBACK_GET_FAILURE = 7,
  MEMCACHED_CALLBACK_DELETE_TRIGGER = 8,
  MEMCACHED_CALLBACK_MAX,
  MEMCACHED_CALLBACK_NAMESPACE = MEMCACHED_CALLBACK_PREFIX_KEY
};

#ifndef __cplusplus
typedef enum memcached_callback_t memcached_callback_t;
#endif
