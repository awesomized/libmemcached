/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once


namespace libtest {

/**
  Structure which houses the actual callers for the test cases contained in
  the collections.
*/
class Runner {
public:
  virtual test_return_t run(test_callback_fn* func, void *object);
  virtual test_return_t pre(test_callback_fn* func, void *object);
  virtual test_return_t post(test_callback_fn* func, void *object);

  Runner();

  virtual ~Runner() { }
};

} // namespace Runner
