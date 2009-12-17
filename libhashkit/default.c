/* HashKit
 * Copyright (C) 2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include "common.h"

uint32_t hashkit_default(const char *key, size_t key_length)
{
  const char *ptr= key;
  uint32_t value= 0;

  while (key_length--)
  {
    uint32_t val= (uint32_t) *ptr++;
    value += val;
    value += (value << 10);
    value ^= (value >> 6);
  }
  value += (value << 3);
  value ^= (value >> 11);
  value += (value << 15);

  return value;
}
