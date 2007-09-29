#include <memcached.h>

void parse_opt_servers (memcached_st *ptr,
                        char *opt_servers);
char *strdup_cleanup(const char *str);
void cleanup(void);
long int timedif(struct timeval a, struct timeval b);
