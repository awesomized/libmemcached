Set encryption key
==================

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: void memcached_set_encoding_key (memcached_st *ptr, const char *string, const size_t string_length)

Compile and link with -lmemcached

DESCRIPTION
-----------

:func:`memcached_set_encoding_key` sets the key that will be used to encrypt and decrypt data as it is sent and received from the server.

Currently only AES is is supported.

RETURN
------

A value of type :type:`memcached_return_t` is returned On success that value
will be `MEMCACHED_SUCCESS`.  Use :func:`memcached_strerror` to
translate this value to a printable string.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
