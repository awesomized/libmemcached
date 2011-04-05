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

#include <config.h>

#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <libmemcached/memcached.h>
#include <libmemcached/util.h>

#include <libtest/server.h>

static struct timespec global_sleep_value= { .tv_sec= 0, .tv_nsec= 50000 };

static void global_sleep(void)
{
#ifdef WIN32
  sleep(1);
#else
  nanosleep(&global_sleep_value, NULL);
#endif
}

static bool wait_for_file(const char *filename)
{
  uint32_t timeout= 6;
  uint32_t waited;
  uint32_t this_wait;
  uint32_t retry;

  for (waited= 0, retry= 1; ; retry++, waited+= this_wait)
  {
    if ((! access(filename, R_OK)) || (waited >= timeout))
    {
      return true;
    }

    this_wait= retry * retry / 3 + 1;
    sleep(this_wait);
  }

  return false;
}

static void kill_file(const char *file_buffer)
{
  FILE *fp;

  while ((fp= fopen(file_buffer, "r")))
  {
    char pid_buffer[1024];

    if (fgets(pid_buffer, sizeof(pid_buffer), fp) != NULL)
    {
      pid_t pid= (pid_t)atoi(pid_buffer);
      if (pid != 0)
      {
        if (kill(pid, SIGTERM) == -1)
        {
          remove(file_buffer); // If this happens we may be dealing with a dead server that left its pid file.
        }
        else
        {
          uint32_t counter= 3;
          while ((kill(pid, 0) == 0) && --counter)
          {
            global_sleep();
          }
        }
      }
    }

    global_sleep();

    fclose(fp);
  }
}

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

      uint32_t port_base= 0;
      for (uint32_t x= 0; x < construct->count; x++)
      {
        int status;

        snprintf(construct->pid_file[x], FILENAME_MAX, "/tmp/memcached.pidXXXXXX");
        int fd;
        if ((fd= mkstemp(construct->pid_file[x])) == -1)
        {
          perror("mkstemp");
          return;
        }
        close(fd);

        {
          char *var;
          char variable_buffer[1024];

          snprintf(variable_buffer, sizeof(variable_buffer), "LIBMEMCACHED_PORT_%u", x);

          if ((var= getenv(variable_buffer)))
          {
            construct->port[x]= (in_port_t)atoi(var);
          }
          else
          {
            do {
              construct->port[x]= (in_port_t)(x + TEST_PORT_BASE + port_base);

              if (libmemcached_util_ping("localhost", construct->port[x], NULL))
              {
                port_base++;
                construct->port[x]= 0;
              }
            } while (construct->port[x] == 0);
          }
        }

        char buffer[FILENAME_MAX];
        if (x == 0)
        {
          snprintf(buffer, sizeof(buffer), "%s -d -P %s -t 1 -p %u -U %u -m 128",
                   MEMCACHED_BINARY, construct->pid_file[x], construct->port[x], construct->port[x]);
        }
        else
        {
          snprintf(buffer, sizeof(buffer), "%s -d -P %s -t 1 -p %u -U %u",
                   MEMCACHED_BINARY, construct->pid_file[x], construct->port[x], construct->port[x]);
        }

	if (libmemcached_util_ping("localhost", construct->port[x], NULL))
	{
	  fprintf(stderr, "Server on port %u already exists\n", construct->port[x]);
	}
	else
	{
	  status= system(buffer);
	  fprintf(stderr, "STARTING SERVER: %s  status:%d\n", buffer, status);
	}

        int count;
        size_t remaining_length= sizeof(server_string_buffer) - (size_t)(end_ptr -server_string_buffer);
        count= snprintf(end_ptr, remaining_length,  "localhost:%u,", construct->port[x]);

        if ((size_t)count >= remaining_length || count < 0)
        {
          fprintf(stderr, "server names grew to be larger then buffer allowed\n");
          abort();
        }
        end_ptr+= count;
      }
      *end_ptr= 0;


      for (uint32_t x= 0; x < construct->count; x++)
      {
        if (! wait_for_file(construct->pid_file[x]))
        {
          abort();
        }
      }

      for (uint32_t x= 0; x < construct->count; x++)
      {
        uint32_t counter= 3000; // Absurd, just to catch run away process
        while (construct->pids[x] <= 0  && --counter)
        {
          FILE *file= fopen(construct->pid_file[x], "r");
          if (file)
          {
            char pid_buffer[1024];
            char *found= fgets(pid_buffer, sizeof(pid_buffer), file);

            if (found)
            {
              construct->pids[x]= atoi(pid_buffer);
              fclose(file);

              if (construct->pids[x] > 0)
                break;
            }
            fclose(file);
          }
          switch (errno)
          {
          default:
            fprintf(stderr, "%s -> fopen(%s)\n", construct->pid_file[x], strerror(errno));
            abort();
          case ENOENT:
          case EINTR:
          case EACCES:
            break;
          case ENOTCONN:
            continue;
          }

          // Safety 3rd, check to see if the file has gone away
          if (! wait_for_file(construct->pid_file[x]))
          {
            abort();
          }
        }

        bool was_started= false;
        if (construct->pids[x] > 0)
        {
          counter= 30;
          while (--counter)
          {
            if (kill(construct->pids[x], 0) == 0)
            {
              was_started= true;
              break;
            }
            global_sleep();
          }
        }

        if (was_started == false)
        {
          fprintf(stderr, "Failed to open buffer %s(%d)\n", construct->pid_file[x], construct->pids[x]);
          for (uint32_t y= 0; y < construct->count; y++)
          {
            if (construct->pids[y] > 0)
              kill(construct->pids[y], SIGTERM);
          }
          abort();
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
      kill_file(construct->pid_file[x]);
    }

    free(construct->server_list);
  }
}
