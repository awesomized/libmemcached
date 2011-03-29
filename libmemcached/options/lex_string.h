#pragma once

#include <stdlib.h>
#include <iostream>

struct string_t
{
  const char *c_str;
  size_t length;
};

inline std::ostream& operator<<(std::ostream& output, const string_t& arg)
{
  output << arg.c_str;
  return output;
}
