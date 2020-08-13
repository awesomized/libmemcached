Flush and send buffered commands
================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: memcached_return_t memcached_flush_buffers (memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

:func:`memcached_flush_buffers` is used in conjunction with
`MEMCACHED_BEHAVIOR_BUFFER_REQUESTS` to flush all buffers by
sending the buffered commands to the server for processing.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned.
On success that value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_behavior(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
    * :doc:`memcached_behavior`
