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

#include "libmemcached/common.h"

memcached_server_list_st memcached_servers_parse(const char *server_strings) {
  char *string;
  const char *begin_ptr;
  const char *end_ptr;
  memcached_server_st *servers = NULL;
  memcached_return_t rc;

  WATCHPOINT_ASSERT(server_strings);

  end_ptr = server_strings + strlen(server_strings);

  for (begin_ptr = server_strings, string = (char *) strchr(server_strings, ',');
       begin_ptr != end_ptr; string = (char *) strchr(begin_ptr, ','))
  {
    char buffer[HUGE_STRING_LEN];
    char *ptr, *ptr2 = NULL;
    uint32_t weight = 0;

    if (string) {
      memcpy(buffer, begin_ptr, (size_t)(string - begin_ptr));
      buffer[(unsigned int) (string - begin_ptr)] = 0;
      begin_ptr = string + 1;
    } else {
      size_t length = strlen(begin_ptr);
      memcpy(buffer, begin_ptr, length);
      buffer[length] = 0;
      begin_ptr = end_ptr;
    }

    ptr = strchr(buffer, '[');
    if (ptr) {
      ptr2 = strchr(ptr+1, ']');
    }
    if (ptr && ptr2) {
      // [IPv6]:port
      ptr = strchr(ptr2+1, ':');
    } else {
      // IPv4:port or name:port
      ptr = strchr(buffer, ':');
    }
    in_port_t port = 0;
    if (ptr) {
      ptr[0] = 0;

      ptr++;

      errno = 0;
      port = (in_port_t) strtoul(ptr, (char **) NULL, 10);
      if (errno) {
        memcached_server_free(servers);
        return NULL;
      }

      ptr2 = strchr(ptr, ' ');
      if (!ptr2)
        ptr2 = strchr(ptr, ':');

      if (ptr2) {
        ptr2++;
        errno = 0;
        weight = uint32_t(strtoul(ptr2, (char **) NULL, 10));
        if (errno) {
          memcached_server_free(servers);
          return NULL;
        }
      }
    }

    servers = memcached_server_list_append_with_weight(servers, buffer, port, weight, &rc);

    if (isspace(*begin_ptr)) {
      begin_ptr++;
    }
  }

  return servers;
}
