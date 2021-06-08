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

#define MEMCACHED_MAX_BUFFER    8196
#define MEMCACHED_MAX_KEY       251 /* We add one to have it null terminated */
#define MEMCACHED_MAX_NAMESPACE 128

#define MEMCACHED_MAX_HOST_SORT_LENGTH       86 /* Used for Ketama */
#define MEMCACHED_MAX_INTEGER_DISPLAY_LENGTH 20
#define MEMCACHED_VERSION_STRING_LENGTH      24

/* legacy */
#define MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH MEMCACHED_MAX_INTEGER_DISPLAY_LENGTH
#define MEMCACHED_PREFIX_KEY_MAX_SIZE            MEMCACHED_MAX_NAMESPACE
