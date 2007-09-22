#include <memcached.h>

void parse_opt_servers (memcached_st *ptr,
                        char *opt_servers);
char *strdup_cleanup(const char *str);
void cleanup(void);
