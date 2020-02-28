Deleting data from a server
===========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_delete (memcached_st *ptr, const char *key, size_t key_length, time_t expiration)

.. function:: memcached_return_t memcached_delete_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, time_t expiration)


DESCRIPTION
-----------

`memcached_delete` is used to delete a particular key. `memcached_delete_by_key`
works the same, but it takes a master key to find the given value.

Expiration works by placing the item into a delete queue, which means that it
won't be possible to retrieve it by the "get" command. The "add" and "replace"
commands with this key will also fail (the "set" command will succeed, however).
After the time passes, the item is finally deleted from server memory.

Please note the the memcached server removed tests for expiration in the 1.4
version.

RETURN VALUE
------------

A value of type `memcached_return_t` is returned
On success that value will be `MEMCACHED_SUCCESS`.
Use `memcached_strerror` to translate this value to a printable string.

If you are using the non-blocking mode of the library, success only means that
the message was queued for delivery.

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
