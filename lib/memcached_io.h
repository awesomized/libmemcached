/* Server IO, Not public! */
#include <memcached.h>

ssize_t memcached_io_flush(memcached_st *ptr, unsigned int server_key);
ssize_t memcached_io_write(memcached_st *ptr, unsigned int server_key,
                        char *buffer, size_t length);
void memcached_io_reset(memcached_st *ptr, unsigned int server_key);
ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length);
