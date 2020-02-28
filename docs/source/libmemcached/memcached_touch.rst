===========================================
memcached_touch, memcached_touch_by_key
===========================================

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: memcached_return_t memcached_touch (memcached_st *ptr, const char *key, size_t key_length, time_t expiration)

.. function:: memcached_return_t memcached_touch_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, time_t expiration)

Compile and link with -lmemcached

DESCRIPTION
-----------

:func:`memcached_touch` is used to update the expiration time on an existing key.
:func:`memcached_touch_by_key` works the same, but it takes a master key 
to find the given value.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned
On success that value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable 
string.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

