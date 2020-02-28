==============================================
Storing custom user information in the client.
==============================================

.. index:: object: memcached_st

Manage user specific data

LIBRARY
-------

C Client Library for memcached (libmemcached, -lmemcached)

SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: void *memcached_get_user_data (memcached_st *ptr)

.. function:: void *memcached_set_user_data (memcached_st *ptr, void *data)

Compile and link with -lmemcached

DESCRIPTION
-----------

libmemcached(3) allows you to store a pointer to a user specific data inside
the memcached_st structure.

:func:`memcached_set_user_data` is used to set the user specific data in the
:type:`memcached_st` structure.

:func:`memcached_get_user_data` is used to retrieve the user specific data in the :type:`memcached_st` structure.

RETURN VALUE
------------

:func:`memcached_set_user_data` returns the previous value of the user specific data.

:func:`memcached_get_user_data` returns the current value uf the user specific data.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)`

