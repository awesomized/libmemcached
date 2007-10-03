#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"

/* Use this for string generation */
static const char ALPHANUMERICS[]=
  "0123456789ABCDEFGHIJKLMNOPQRSTWXYZabcdefghijklmnopqrstuvwxyz";

#define ALPHANUMERICS_SIZE (sizeof(ALPHANUMERICS)-1)

static void get_random_string(char *buffer, size_t size)
{
  char *buffer_ptr= buffer;

  while (--size)
    *buffer_ptr++= ALPHANUMERICS[random() % ALPHANUMERICS_SIZE];
  *buffer_ptr++= ALPHANUMERICS[random() % ALPHANUMERICS_SIZE];
}

void pairs_free(pairs_st *pairs)
{
  unsigned int x;

  if (!pairs)
    return;

  /* We free until we hit the null pair we stores during creation */
  for (x= 0; pairs[x].key; x++)
  {
    free(pairs[x].key);
    free(pairs[x].value);
  }

  free(pairs);
}

pairs_st *pairs_generate(unsigned long long number_of)
{
  unsigned int x;
  pairs_st *pairs;

  pairs= (pairs_st*)malloc(sizeof(pairs_st) * (number_of+1));

  if (!pairs)
    goto error;

  memset(pairs, 0, sizeof(pairs_st) * (number_of+1));

  for (x= 0; x < number_of; x++)
  {
    pairs[x].key= (char *)malloc(sizeof(char) * 100);
    if (!pairs[x].key)
      goto error;
    get_random_string(pairs[x].key, 100);
    pairs[x].key_length= 100;

    pairs[x].value= (char *)malloc(sizeof(char) * 400);
    if (!pairs[x].value)
      goto error;
    get_random_string(pairs[x].value, 400);
    pairs[x].value_length= 400;
  }

  return pairs;
error:
    fprintf(stderr, "Memory Allocation failure in pairs_generate.\n");
    exit(0);
}
