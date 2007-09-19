#include <stdio.h>
#include <memcached.h>

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  unsigned int x;
  size_t string_length;
  uint16_t  flags;
  memcached_return rc;

  if (argc == 1)
    return 0;

  memc= memcached_init(NULL);

  for (x= 1; x < argc; x++)
  {
    string= memcached_get(memc, argv[x], strlen(argv[x]),
                          &string_length, &flags, &rc);
    if (string)
    {
      printf("%.*s\n", string_length, string);
      free(string);
    }
  }

  memcached_deinit(memc);

  return 0;
};
