Disconnecting a client from a server
====================================

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: void memcached_quit (memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct

DESCRIPTION
-----------

:func:`memcached_quit` will disconnect you from all currently connected
servers. It will also reset the state of the connection (i.e., any
:func:`memcached_fetch` you are in the middle of will be terminated). This
function is called automatically when you call :func:`memcached_free` on the
:type:`memcached_st` structure.

You do not need to call this on your own. All operations to change server
hashes and parameters will handle connections to the server for you. This
function is provided mainly so that you can timeout your connections or
reset connections during the middle of a :func:`memcached_fetch`.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned On success that value
will be `MEMCACHED_SUCCESS`.  Use :func:`memcached_strerror` to
translate this value to a printable string.

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
