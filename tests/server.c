/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */

/*
  Startup, and shutdown the memcached servers.
*/

#define TEST_PORT_BASE MEMCACHED_DEFAULT_PORT+10

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <libmemcached/memcached.h>
#include <libmemcached/util.h>
#include <unistd.h>
#include <time.h>

#include "server.h"

void server_startup(server_startup_st *construct)
{
  if ((construct->server_list= getenv("MEMCACHED_SERVERS")))
  {
    printf("servers %s\n", construct->server_list);
    construct->servers= memcached_servers_parse(construct->server_list);
    construct->server_list= NULL;
    construct->count= 0;
  }
  else
  {
    {
      char server_string_buffer[8096];
      char *end_ptr;
      end_ptr= server_string_buffer;

      for (uint32_t x= 0; x < construct->count; x++)
      {
        char buffer[1024]; /* Nothing special for number */
        int count;
        int status;

        sprintf(buffer, "/tmp/%umemc.pid", x);
        if (access(buffer, F_OK) == 0)
        {
          FILE *fp= fopen(buffer, "r");
          remove(buffer);

          if (fp != NULL)
          {
            if (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
              pid_t pid= (pid_t)atoi(buffer);
              if (pid != 0)
                kill(pid, SIGTERM);
            }

            fclose(fp);
          }
        }

        if (x == 0)
        {
          sprintf(buffer, "%s -d -u root -P /tmp/%umemc.pid -t 1 -p %u -U %u -m 128",
		  MEMCACHED_BINARY, x, x + TEST_PORT_BASE, x + TEST_PORT_BASE);
        }
        else
        {
          sprintf(buffer, "%s -d -u root -P /tmp/%umemc.pid -t 1 -p %u -U %u",
		  MEMCACHED_BINARY, x, x + TEST_PORT_BASE, x + TEST_PORT_BASE);
        }
	if (libmemcached_util_ping("localhost", (in_port_t)(x + TEST_PORT_BASE), NULL))
	{
	  fprintf(stderr, "Server on port %u already exists\n", x + TEST_PORT_BASE);
	}
	else
	{
	  status= system(buffer);
	  fprintf(stderr, "STARTING SERVER: %s  status:%d\n", buffer, status);
	}
        count= sprintf(end_ptr, "localhost:%u,", x + TEST_PORT_BASE);
        end_ptr+= count;
      }
      *end_ptr= 0;

      for (uint32_t x= 0; x < construct->count; x++)
      {
        uint32_t counter= 3;
        char buffer[1024]; /* Nothing special for number */

        snprintf(buffer, sizeof(buffer), "/tmp/%umemc.pid", x);

        while (--counter)
        {
          int memcached_pid;

          FILE *file;
          file= fopen(buffer, "r");
          if (file == NULL)
          {
            struct timespec req= { .tv_sec= 0, .tv_nsec= 5000 };
            nanosleep(&req, NULL);

            continue;
          }
          char *found= fgets(buffer, sizeof(buffer), file);
          if (!found)
          {
            abort();
          }
          // Yes, we currently pitch this and don't make use of it.
          memcached_pid= atoi(buffer);
          fclose(file);
        }


      }

      construct->server_list= strdup(server_string_buffer);
    }
    printf("servers %s\n", construct->server_list);
    construct->servers= memcached_servers_parse(construct->server_list);
  }

  assert(construct->servers);

  srandom((unsigned int)time(NULL));

  for (uint32_t x= 0; x < memcached_server_list_count(construct->servers); x++)
  {
    printf("\t%s : %d\n", memcached_server_name(&construct->servers[x]), memcached_server_port(&construct->servers[x]));
    assert(construct->servers[x].fd == -1);
    assert(construct->servers[x].cursor_active == 0);
  }

  printf("\n");
}

void server_shutdown(server_startup_st *construct)
{
  if (construct->server_list)
  {
    for (uint32_t x= 0; x < construct->count; x++)
    {
      char buffer[1024]; /* Nothing special for number */
      sprintf(buffer, "cat /tmp/%umemc.pid | xargs kill", x);
      /* We have to check the return value of this or the compiler will yell */
      int sys_ret= system(buffer);
      assert(sys_ret != -1);
      sprintf(buffer, "/tmp/%umemc.pid", x);
      unlink(buffer);
    }

    free(construct->server_list);
  }
}
