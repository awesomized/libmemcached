===============================
Generating hash values directly
===============================

.. index:: object: memcached_st

Hash a key value


-------- 
SYNOPSIS 
--------


#include <libmemcached/memcached.h>

.. c:type:: memcached_hash_t
 
.. c:function:: uint32_t memcached_generate_hash_value (const char *key, size_t key_length, memcached_hash_t hash_algorithm)

.. c:function:: uint32_t memcached_generate_hash (memcached_st *ptr, const char *key, size_t key_length)

.. c:type:: MEMCACHED_HASH_DEFAULT

.. c:type:: MEMCACHED_HASH_MD5

.. c:type:: MEMCACHED_HASH_CRC

.. c:type:: MEMCACHED_HASH_FNV1_64

.. c:type:: MEMCACHED_HASH_FNV1A_64

.. c:type:: MEMCACHED_HASH_FNV1_32

.. c:type:: MEMCACHED_HASH_FNV1A_32

.. c:type:: MEMCACHED_HASH_JENKINS

.. c:type:: MEMCACHED_HASH_MURMUR

.. c:type:: MEMCACHED_HASH_HSIEH

.. c:type:: MEMCACHED_HASH_MURMUR3


Compile and link with -lmemcachedutil -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_generate_hash_value` allows you to hash a key using one of
the hash functions defined in the library. This method is provided for
the convenience of higher-level language bindings and is not necessary
for normal memcache operations.

The allowed hash algorithm constants are listed in the manpage for
:c:func:`memcached_behavior_set`.

:c:func:`memcached_generate_hash` takes a :c:type:`memcached_st` struture 
and produces the hash value that would have been generated based on the 
defaults of :c:type:`memcached_st`.

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
