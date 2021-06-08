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

/* Public defines */
#define MEMCACHED_DEFAULT_PORT             11211
#define MEMCACHED_DEFAULT_PORT_STRING      "11211"
#define MEMCACHED_POINTS_PER_SERVER        100
#define MEMCACHED_POINTS_PER_SERVER_KETAMA 160
#define MEMCACHED_CONTINUUM_SIZE \
  MEMCACHED_POINTS_PER_SERVER * 100 /* This would then set max hosts to 100 */
#define MEMCACHED_STRIDE                  4
#define MEMCACHED_DEFAULT_TIMEOUT         5000
#define MEMCACHED_DEFAULT_CONNECT_TIMEOUT 4000
#define MEMCACHED_CONTINUUM_ADDITION \
  10 /* How many extra slots we should build for in the continuum */
#define MEMCACHED_EXPIRATION_NOT_ADD           0xffffffffU
#define MEMCACHED_SERVER_FAILURE_LIMIT         5
#define MEMCACHED_SERVER_FAILURE_RETRY_TIMEOUT 2
#define MEMCACHED_SERVER_FAILURE_DEAD_TIMEOUT  0
#define MEMCACHED_SERVER_TIMEOUT_LIMIT         0
