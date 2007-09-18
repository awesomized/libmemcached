#include <stdio.h>
#include <memcached.h>

int main(int argc, char *argv[])
{
  memcached_st memc;

  (void)memcached_init(&memc);
  memcached_deinit(&memc);

  return 0;
};
