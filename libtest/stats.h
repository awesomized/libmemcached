/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once
struct Stats {
  int32_t collection_success;
  int32_t collection_skipped;
  int32_t collection_failed;
  int32_t collection_total;

  uint32_t success;
  uint32_t skipped;
  uint32_t failed;
  uint32_t total;

  Stats() :
    collection_success(0),
    collection_skipped(0),
    collection_failed(0),
    collection_total(0),
    success(0),
    skipped(0),
    failed(0),
    total(0)
  { }
};

