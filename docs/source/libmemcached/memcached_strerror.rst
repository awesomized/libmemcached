Converting Error Codes to Messages
==================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: const char *memcached_strerror(memcached_st *ptr, memcached_return_t rc)

    :param ptr: pointer to initialized `memcached_st` struct
    :param rc: `memcached_return_t` value to query the string representation for
    :returns: the string representation of `rc`

DESCRIPTION
-----------

`memcached_strerror` takes a `memcached_return_t` value and returns a string
describing the error.

This string must not be modified by the application.

`memcached_return_t` values are returned from nearly all libmemcached(3)
functions.

`memcached_return_t` values are of an enum type so that you can set up responses
with switch/case and know that you are capturing all possible return values.

RETURN VALUE
------------

`memcached_strerror` returns a string describing a `memcached_return_t` value.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_return_t`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_return_t`
