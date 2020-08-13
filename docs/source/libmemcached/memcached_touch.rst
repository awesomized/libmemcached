Update expiration on a key
==========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: memcached_return_t memcached_touch (memcached_st *ptr, const char *key, size_t key_length, time_t expiration)

    :param ptr: pointer to initialized `memcached_st` struct
    :param key: the key to touch
    :param key_length: the length of `key` without any terminating zero
    :param expiration: new expiration as a unix timestamp or as relative expiration time in seconds
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_touch_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, time_t expiration)

    :param ptr: pointer to initialized `memcached_st` struct
    :param group_key: the `key` namespace
    :param group_key_length: the length of `group_key` without any terminating zero
    :param key: the key to touch
    :param key_length: the length of `key` without any terminating zero
    :param expiration: new expiration as a unix timestamp or as relative expiration time in seconds
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

:func:`memcached_touch` is used to update the expiration time on an existing key.
:func:`memcached_touch_by_key` works the same, but it takes a master key 
to find the given value.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned.
On success that value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable 
string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
