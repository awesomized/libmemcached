/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

typedef void* (test_callback_create_fn)(libtest::server_startup_st&, test_return_t&);
typedef bool test_callback_destroy_fn(void *);
typedef enum test_return_t (test_callback_fn)(void *);
typedef enum test_return_t (test_callback_runner_fn)(test_callback_fn*, void *);
typedef enum test_return_t (test_callback_error_fn)(const test_return_t, void *);

#ifdef	__cplusplus
}
#endif

