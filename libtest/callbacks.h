/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

typedef void* (test_callback_create_fn)(enum test_return_t*);
typedef enum test_return_t (test_callback_fn)(void *);
typedef enum test_return_t (test_callback_runner_fn)(test_callback_fn*, void *);
typedef enum test_return_t (test_callback_error_fn)(const enum test_return_t, void *);

#ifdef	__cplusplus
}
#endif

