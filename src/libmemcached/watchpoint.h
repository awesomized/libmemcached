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

#define WATCHPOINT
#define WATCHPOINT_ERROR(A)
#define WATCHPOINT_IFERROR(__memcached_return_t) (void) (__memcached_return_t)
#define WATCHPOINT_STRING(A)
#define WATCHPOINT_NUMBER(A)
#define WATCHPOINT_LABELED_NUMBER(A, B)
#define WATCHPOINT_IF_LABELED_NUMBER(A, B, C)
#define WATCHPOINT_ERRNO(A)
#define WATCHPOINT_ASSERT_PRINT(A, B, C)
#define WATCHPOINT_ASSERT(A) (void) (A)
#define WATCHPOINT_ASSERT_INITIALIZED(A)
#define WATCHPOINT_SET(A)
