#include <ctype.h>
#include <strings.h>
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

void help_command(char *command_name, char *description,
                  const struct option *long_options,
                  memcached_programs_help_st *options)
{
  unsigned int x;

  printf("%s v%u.%u\n\n", command_name, 1, 0);
  printf("\t%s\n\n", description);
  printf("Current options. A '=' means the option takes a value.\n\n");

  for (x= 0; long_options[x].name; x++) 
    printf("\t --%s%c\n", long_options[x].name, 
           long_options[x].has_arg ? '=' : ' ');  

  printf("\n");
  exit(0);
}
