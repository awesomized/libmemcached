#include <memcached.h>

memcached_server_st *parse_opt_servers(char *server_strings);
char *strdup_cleanup(const char *str);
void cleanup(void);
long int timedif(struct timeval a, struct timeval b);
