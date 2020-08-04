memcached_fetch
=================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function::  char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, size_t *value_length, uint32_t *flags, memcached_return_t *error)

   .. deprecated:: 0.50
      Use :func:`memcached_fetch_result` instead.


DESCRIPTION
-----------

:func:`memcached_fetch` is used to fetch an individual value from the server.
:func:`memcached_mget` must always be called before using this method.
You must pass in a key and its length to fetch the object.

You must supply three pointer variables which will give you the state of the returned
object:

    * A :type:`uint32_t` pointer to contain whatever flags you stored with the value,
    * a :type:`size_t` pointer which will be filled with size of of the object, and
    * a :type:`memcached_return_t` pointer to hold any error.

The object will be returned upon success and NULL will be returned on failure.

`MEMCACHED_END` is returned by the error value when all objects that have been found are returned.
The final value upon `MEMCACHED_END` is a NULL pointer.

Values returned by :func:`memcached_fetch` must be freed by the caller.

All of the above functions are not tested when the
`MEMCACHED_BEHAVIOR_USE_UDP` has been set. Executing any of these 
functions with this behavior on will result in `MEMCACHED_NOT_SUPPORTED`
being returned, or for those functions which do not return a
:type:`memcached_return_t`, the error function parameter will
be set to `MEMCACHED_NOT_SUPPORTED`.

RETURN VALUE
------------

:func:`memcached_fetch` sets error to
to `MEMCACHED_END` upon successful conclusion.

`MEMCACHED_NOTFOUND` will be return if no keys at all were found.

`MEMCACHED_KEY_TOO_BIG` is set to error whenever :func:`memcached_fetch` was used
and the key was set larger then `MEMCACHED_MAX_KEY`, which was the largest
key allowed for the original memcached ascii server.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_fetch_result(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
    * :doc:`memcached_get`
