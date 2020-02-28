===============================
Generating hash values directly
===============================

.. index:: object: memcached_st

Hash a key value


-------- 
SYNOPSIS 
--------


#include <libmemcached/memcached.h>

.. type:: memcached_hash_t
 
.. function:: uint32_t memcached_generate_hash_value (const char *key, size_t key_length, memcached_hash_t hash_algorithm)

.. function:: uint32_t memcached_generate_hash (memcached_st *ptr, const char *key, size_t key_length)

.. type:: MEMCACHED_HASH_DEFAULT

.. type:: MEMCACHED_HASH_MD5

.. type:: MEMCACHED_HASH_CRC

.. type:: MEMCACHED_HASH_FNV1_64

.. type:: MEMCACHED_HASH_FNV1A_64

.. type:: MEMCACHED_HASH_FNV1_32

.. type:: MEMCACHED_HASH_FNV1A_32

.. type:: MEMCACHED_HASH_JENKINS

.. type:: MEMCACHED_HASH_MURMUR

.. type:: MEMCACHED_HASH_HSIEH

.. type:: MEMCACHED_HASH_MURMUR3


Compile and link with -lmemcachedutil -lmemcached


-----------
DESCRIPTION
-----------


:func:`memcached_generate_hash_value` allows you to hash a key using one of
the hash functions defined in the library. This method is provided for
the convenience of higher-level language bindings and is not necessary
for normal memcache operations.

The allowed hash algorithm constants are listed in the manpage for
:func:`memcached_behavior_set`.

:func:`memcached_generate_hash` takes a :type:`memcached_st` structure
and produces the hash value that would have been generated based on the 
defaults of :type:`memcached_st`.

As of version 0.36 all hash methods have been placed into the library
libhashkit(3) which is linked with libmemcached(3). For more information please see its documentation.


------
RETURN
------


A 32-bit integer which is the result of hashing the given key.
For 64-bit hash algorithms, only the least-significant 32 bits are
returned.



--------
SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
