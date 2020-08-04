Dumping data from a server
==========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. type:: memcached_return_t (*memcached_dump_fn)(memcached_st *ptr, const char *key, size_t key_length, void *context)

    :param ptr: pointer to `memcached_st` object
    :param key: key string being dumped
    :param key_length: length of the key without any terminating zero
    :param context: pointer to the user supplied context
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_dump (memcached_st *ptr, memcached_dump_fn *function, void *context, uint32_t number_of_callbacks)

    :param ptr: pointer to initialized `memcached_st` struct
    :param function: pointer to `number_of_callbacks` `memcached_dump_fn` callbacks
    :param context: pointer to a user managed context
    :param number_of_callbacks: number of callbacks in the `function` array
    :returns: `memcached_return_t` indicating success


DESCRIPTION
-----------

:func:`memcached_dump` is used to get a list of keys found in :manpage:`memcached(1)`
servers. Because memcached does not guarantee to dump all keys you can not
assume you have fetched all keys from the server. The function takes an array
of callbacks that it will use to execute on keys as they are found.

Currently the binary protocol is not tested.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned.
On success that value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable 
string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
