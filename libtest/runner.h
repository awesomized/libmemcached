/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once


/**
  Structure which houses the actual callers for the test cases contained in
  the collections.
*/
struct Runner {
  test_callback_runner_fn *pre;
  test_callback_runner_fn *run;
  test_callback_runner_fn *post;
};

