Set encryption key
==================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: memcached_return_t memcached_set_encoding_key (memcached_st *ptr, const char *str, const size_t length)

    :param ptr: pointer to initialized `memcached_st` struct
    :param str: the key to use
    :param length: the length of `key` without any terminating zero
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

:func:`memcached_set_encoding_key` sets the key that will be used to encrypt and
decrypt data as it is sent and received from the server.

Currently only AES is is supported.

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

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
