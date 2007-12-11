/* Server IO, Not public! */
#include <memcached.h>

ssize_t memcached_io_write(memcached_st *ptr, unsigned int server_key,
                        char *buffer, size_t length, char with_flush);
void memcached_io_reset(memcached_st *ptr, unsigned int server_key);
ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length);
memcached_return memcached_io_close(memcached_st *ptr, unsigned int server_key);
void memcached_io_preread(memcached_st *ptr);
