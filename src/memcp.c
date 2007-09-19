#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <memcached.h>

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  unsigned int x;
  size_t string_length;
  uint16_t  flags;
  memcached_return rc;

  if (argc < 3)
    return 0;

  memc= memcached_init(NULL);

  /* Get the server name */
  {
    char *ptr;
    char *hostname;
    size_t hostname_length;
    unsigned int port;

    ptr= index(argv[argc-1], ':');

    if (ptr)
    {
      hostname_length= ptr - argv[argc-1];
      hostname= (char *)malloc(hostname_length+1);
      memset(hostname, 0, hostname_length+1);
      memcpy(hostname, argv[argc-1], hostname_length);

      ptr++;

      port= strtol(ptr, (char **)NULL, 10);

      memcached_server_add(memc, hostname, port);
      free(hostname);
    }
    else
    {
      memcached_server_add(memc, argv[argc -1], 0);
    }
  }

  for (x= 1; x < argc-1; x++)
  {
    char *mptr;
    struct stat sbuf;
    int fd;
    char *ptr;

    fd= open(argv[x], O_RDONLY);

    if (fd == -1)
    {
      fprintf(stderr, "Failed opening %s\n", argv[x]);
      exit(1);
    }

    (void)fstat(fd, &sbuf);
    mptr= mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    ptr= rindex(argv[x], '/');
    if (ptr)
    {
      ptr++;
    }
    else
    {
      ptr= argv[x];
    }
    
    rc= memcached_set(memc, ptr, strlen(ptr),
                      mptr, sbuf.st_size,
                      (time_t)0, (uint16_t)0);

    munmap(mptr, sbuf.st_size);
    close(fd);
  }

  memcached_deinit(memc);

  return 0;
};
