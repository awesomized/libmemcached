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

#include "mem_config.h"

#include "libmemcached/backtrace.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if HAVE_BACKTRACE

#  include BACKTRACE_HEADER

#  ifdef HAVE_ABI____CXA_DEMANGLE
#    include <cxxabi.h>
#  endif

#  ifdef HAVE_DLFCN_H
#    include <dlfcn.h>
#  endif

const int MAX_DEPTH = 50;

void custom_backtrace(void) {
  void *backtrace_buffer[MAX_DEPTH + 1];

  int stack_frames = backtrace(backtrace_buffer, MAX_DEPTH);
  if (stack_frames) {
    char **symbollist = backtrace_symbols(backtrace_buffer, stack_frames);
    if (symbollist) {
      for (int x = 0; x < stack_frames; x++) {
        bool was_demangled = false;

#  ifdef HAVE_ABI____CXA_DEMANGLE
#    ifdef HAVE_DLFCN_H
          Dl_info dlinfo;
          if (dladdr(backtrace_buffer[x], &dlinfo)) {
            char demangled_buffer[1024];
            const char *called_in = "<unresolved>";
            if (dlinfo.dli_sname) {
              size_t demangled_size = sizeof(demangled_buffer);
              int status;
              char *demangled;
              if ((demangled = abi::__cxa_demangle(dlinfo.dli_sname, demangled_buffer,
                                                   &demangled_size, &status))) {
                called_in = demangled;
                fprintf(stderr, "---> demangled: %s -> %s\n", demangled_buffer, demangled);
              } else {
                called_in = dlinfo.dli_sname;
              }

              was_demangled = true;
              fprintf(stderr, "#%d  %p in %s at %s\n", x, backtrace_buffer[x], called_in,
                      dlinfo.dli_fname);
            }
          }
#    endif
#  endif

        if (was_demangled == false) {
          fprintf(stderr, "?%d  %p in %s\n", x, backtrace_buffer[x], symbollist[x]);
        }
      }

      ::free(symbollist);
    }
  }
}

#else  // HAVE_BACKTRACE

void custom_backtrace(void) {
  fprintf(stderr, "Backtrace null function called\n");
}
#endif // HAVE_BACKTRACE
