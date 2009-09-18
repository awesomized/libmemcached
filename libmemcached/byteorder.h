#ifndef LIBMEMCACHED_BYTEORDER_H
#define LIBMEMCACHED_BYTEORDER_H

#include "config.h"

/* Define this here, which will turn on the visibilty controls while we're
 * building libmemcached.
 */
#define BUILDING_LIBMEMCACHED 1

#include "libmemcached/memcached.h"

#ifndef HAVE_HTONLL
LIBMEMCACHED_LOCAL
extern uint64_t ntohll(uint64_t);
LIBMEMCACHED_LOCAL
extern uint64_t htonll(uint64_t);
#endif

#ifdef linux
/* /usr/include/netinet/in.h defines macros from ntohs() to _bswap_nn to
 * optimize the conversion functions, but the prototypes generate warnings
 * from gcc. The conversion methods isn't the bottleneck for my app, so
 * just remove the warnings by undef'ing the optimization ..
 */
#undef ntohs
#undef ntohl
#undef htons
#undef htonl
#endif

#endif /* LIBMEMCACHED_BYTEORDER_H */
