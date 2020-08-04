Wiping clean the contents of a server
=====================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: memcached_return_t memcached_flush (memcached_st *ptr, time_t expiration)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param expiration: expiration as a unix timestamp or as relative expiration time in seconds
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`memcached_flush` is used to wipe clean the contents of :manpage:`memcached(1)` servers.
It will either do this immediately or expire the content based on the
expiration time passed to the method (a value of zero causes an immediate
flush). The operation is not atomic to multiple servers, just atomic to a
single server. That is, it will flush the servers in the order that they were
added.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned.
On success that value will be `MEMCACHED_SUCCESS`.
Use `memcached_strerror` to translate this value to a printable string.

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
