Generating hash values directly
===============================

Hash a key value

SYNOPSIS 
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcachedutil -lmemcached

.. function:: uint32_t memcached_generate_hash_value (const char *key, size_t key_length, memcached_hash_t hash_algorithm)

    :param key: the key to generate a hash of
    :param key_length: the length of the `key` without any terminating zero
    :param hash_algorithm: `memcached_hash_t`, the algorithm to use
    :returns: a 32 bit hash value

.. function:: uint32_t memcached_generate_hash (memcached_st *ptr, const char *key, size_t key_length)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param key: the key to generate a hash of
    :param key_length: the length of the `key` without any terminating zero
    :returns: a 32 bit hash value

.. c:type:: enum memcached_hash_t memcached_hash_t

.. enum:: memcached_hash_t

    .. enumerator::  MEMCACHED_HASH_DEFAULT

    .. enumerator::  MEMCACHED_HASH_MD5

    .. enumerator::  MEMCACHED_HASH_CRC

    .. enumerator::  MEMCACHED_HASH_FNV1_64

    .. enumerator::  MEMCACHED_HASH_FNV1A_64

    .. enumerator::  MEMCACHED_HASH_FNV1_32

    .. enumerator::  MEMCACHED_HASH_FNV1A_32

    .. enumerator::  MEMCACHED_HASH_HSIEH

    .. enumerator::  MEMCACHED_HASH_MURMUR

    .. enumerator::  MEMCACHED_HASH_JENKINS

    .. enumerator::  MEMCACHED_HASH_MURMUR3

    .. enumerator::  MEMCACHED_HASH_CUSTOM



DESCRIPTION
-----------

:func:`memcached_generate_hash_value` allows you to hash a key using one of
the hash functions defined in the library. This method is provided for
the convenience of higher-level language bindings and is not necessary
for normal memcache operations.

Support for `MEMCACHED_HASH_HSIEH` is a compile time option that is
disabled by default. To enable tests for this hashing algorithm,
configure and build libmemcached with the Hsieh hash enabled.

:func:`memcached_generate_hash` takes a :type:`memcached_st` structure
and produces the hash value that would have been generated based on the 
defaults of :type:`memcached_st`.

As of version 0.36 all hash methods have been placed into the library
libhashkit(3) which is linked with libmemcached(3). For more information please see its documentation.

RETURN VALUE
------------

A 32-bit integer which is the result of hashing the given key.
For 64-bit hash algorithms, only the least-significant 32 bits are
returned.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
