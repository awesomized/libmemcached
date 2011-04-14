===============================
Generating hash values directly
===============================


Hash a key value


-------- 
SYNOPSIS 
--------


#include <libmemcached/memcached.h>
 
.. c:function:: uint32_t memcached_generate_hash_value (const char *key, size_t key_length, memcached_hash_t hash_algorithm);

.. c:function:: uint32_t memcached_generate_hash (memcached_st *ptr, const char *key, size_t key_length);

Compile and link with -lmemcachedutil -lmemcached


-----------
DESCRIPTION
-----------


memcached_generate_hash_value() allows you to hash a key using one of
the hash functions defined in the library. This method is provided for
the convenience of higher-level language bindings and is not necessary
for normal memcache operations.

The allowed hash algorithm constants are listed in the manpage for
memcached_behavior_set().

memcached_generate_hash() takes a memcached_st struture and produces
the hash value that would have been generated based on the defaults
of the memcached_st structure.

As of version 0.36 all hash methods have been placed into the library
libhashkit(3) which is linked with libmemcached(3). For more information please see its documentation.


------
RETURN
------


A 32-bit integer which is the result of hashing the given key.
For 64-bit hash algorithms, only the least-significant 32 bits are
returned.


----
HOME
----


To find out more information please check: 
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
