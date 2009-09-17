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

#endif /* LIBMEMCACHED_BYTEORDER_H */
