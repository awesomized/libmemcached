Setting the verbosity of a server
=================================

.. index:: object: memcached_st

Modify verbosity of servers

SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: memcached_return_t memcached_verbosity (memcached_st *ptr, uint32_t verbosity)

Compile and link with -lmemcached

DESCRIPTION
-----------

:func:`memcached_verbosity` modifies the "verbosity" of the
memcached(1) servers referenced in the :type:`memcached_st`  parameter.

RETURN
------

A value of type :type:`memcached_return_t` is returned.

On success that value will be `MEMCACHED_SUCCESS`.

Use :func:`memcached_strerror` to translate this value to a printable string.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

