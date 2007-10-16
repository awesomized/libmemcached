#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "utilities.h"


long int timedif(struct timeval a, struct timeval b)
{
  register int us, s;

  us = a.tv_usec - b.tv_usec;
  us /= 1000;
  s = a.tv_sec - b.tv_sec;
  s *= 1000;
  return s + us;
}

void version_command(char *command_name)
{
  printf("%s v%u.%u\n", command_name, 1, 0);
  exit(0);
}

char *lookup_help(memcached_options option)
{
  switch (option)
  {
  case OPT_SERVERS: return("List which servers you wish to connect to.");
  case OPT_VERSION: return("Display the version of the application and then exit.");
  case OPT_HELP: return("Diplay this message and then exit.");
  case OPT_VERBOSE: return("Give more details on the progression of the application.");
  case OPT_DEBUG: return("Provide output only useful for debugging.");
  case OPT_FLAG: return("Provide flag information for storage operation.");
  case OPT_EXPIRE: return("Set the expire option for the object.");
  case OPT_SET: return("Use set command with memcached when storing.");
  case OPT_REPLACE: return("Use replace command with memcached when storing.");
  case OPT_ADD: return("Use add command with memcached when storing.");
  case OPT_SLAP_EXECUTE_NUMBER: return("Number of times to execute the given test.");
  case OPT_SLAP_INITIAL_LOAD: return("Number of key pairs to load before executing tests.");
  case OPT_SLAP_TEST: return("Test to run.");
  case OPT_SLAP_CONCURRENCY: return("Number of users to simulate with load.");
  case OPT_SLAP_NON_BLOCK: return("Set TCP up to use non-blocking IO.");
  case OPT_SLAP_TCP_NODELAY: return("Set TCP socket up to use nodelay.");
  case OPT_FLUSH: return("Flush servers before running tests.");
  };

  return "forgot to document this one :)";
}

void help_command(char *command_name, char *description,
                  const struct option *long_options,
                  memcached_programs_help_st *options)
{
  unsigned int x;

  printf("%s v%u.%u\n\n", command_name, 1, 0);
  printf("\t%s\n\n", description);
  printf("Current options. A '=' means the option takes a value.\n\n");

  for (x= 0; long_options[x].name; x++) 
  {
    char *help_message;

    printf("\t --%s%c\n", long_options[x].name, 
           long_options[x].has_arg ? '=' : ' ');  
    if ((help_message= lookup_help(long_options[x].val)))
      printf("\t\t%s\n", help_message);
  }

  printf("\n");
  exit(0);
}
