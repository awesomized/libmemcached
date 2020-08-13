Determine if a keys exists
==========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function::  memcached_return_t memcached_exist(memcached_st *ptr, char *key, size_t *key_length)

.. function::  memcached_return_t memcached_exist_by_key(memcached_st *ptr, char *group_key, size_t *group_key_length, char *key, size_t *key_length)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param group_key: the key namespace
    :param group_key_length: length of the `group_key` without any terminating zero
    :param key: the key to check
    :param key_length: length of the `key` without any terminating zero
    :returns: `memcached_return_t` indicating success

.. versionadded:: 0.53

DESCRIPTION
-----------

:func:`memcached_exist()` can be used to check if a key exists.

RETURN VALUE
------------

`MEMCACHED_SUCCESS`
  The key exists.

`MEMCACHED_NOTFOUND`
  The key was not found.

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
