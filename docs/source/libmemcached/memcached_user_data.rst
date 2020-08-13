Storing custom information in the client
========================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: void *memcached_get_user_data (memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: opaque pointer to the user supplied data

.. function:: void *memcached_set_user_data (memcached_st *ptr, void *data)

    :param ptr: pointer to initialized `memcached_st` struct
    :param data: opaque pointer to user supplied data
    :returns: opaque pointer to the previously set `data`

DESCRIPTION
-----------

`libmemcached` allows you to store a pointer to a user specific data inside
the memcached_st structure.

:func:`memcached_set_user_data` is used to set the user specific data in the
:type:`memcached_st` structure.

:func:`memcached_get_user_data` is used to retrieve the user specific data in
the :type:`memcached_st` structure.

RETURN VALUE
------------

:func:`memcached_set_user_data` returns the previous value of the user specific data.

:func:`memcached_get_user_data` returns the current value of the user specific data.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
