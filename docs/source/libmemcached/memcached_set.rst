Storing data on the server
==========================

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_set(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_add(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_replace(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_set_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_add_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_replace_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

DESCRIPTION
-----------

:func:`memcached_set`, :func:`memcached_add`, and :func:`memcached_replace` are
all used to store information on the server. All methods take a key, and its
length to store the object. Keys are currently limited to 250 characters when
using either a version of :manpage:`memcached(1)` which is 1.4 or below, or when
using the text protocol. You must supply both a value and a length. Optionally
you store the object. Keys are currently limited to 250 characters by the
memcached(1) server. You must supply both a value and a length. Optionally you
may test an expiration time for the object and a 16 byte value (it is meant to
be used as a bitmap). "flags" is a 4byte space that is stored alongside of the
main value. Many sub libraries make use of this field, so in most cases users
should avoid making use of it.

:func:`memcached_set` will write an object to the server. If an object
already exists it will overwrite what is in the server. If the object does not 
exist it will be written. If you are using the non-blocking mode this function
will always return true unless a network error occurs.

:func:`memcached_replace` replaces an object on the server. If the object is not
found on the server an error occurs.

:func:`memcached_add` adds an object to the server. If the object is found on
the server an error occurs, otherwise the value is stored.

:func:`memcached_set_by_key`, :func:`memcached_add_by_key`, and
:func:`memcached_replace_by_key` methods all behave in a similar method as the non
key methods. The difference is that they use their group_key parameter to map
objects to particular servers.

If you are looking for performance, :func:`memcached_set` with non-blocking IO
is the fastest way to store data on the server.

All of the above functions are tested with the `MEMCACHED_BEHAVIOR_USE_UDP`
behavior enabled. However, when using these operations with this behavior
on, there are limits to the size of the payload being sent to the server.  
The reason for these limits is that the Memcached Server does not allow 
multi-datagram requests and the current server implementation sets a datagram 
size to 1400 bytes. Due to protocol overhead, the actual limit of the user
supplied data is less than 1400 bytes and depends on the protocol in use, as
well as the operation being executed. When running with the binary protocol,
`MEMCACHED_BEHAVIOR_BINARY_PROTOCOL`, the size of the key,value, flags and
expiry combined may not exceed 1368 bytes. When running with the ASCII protocol,
the exact limit fluctuates depending on which function is being executed and
whether the function is a cas operation or not. For non-cas ASCII set operations,
there are at least 1335 bytes available to split among the key, key_prefix, and
value; for cas ASCII operations there are at least 1318 bytes available to split
among the key, key_prefix and value. If the total size of the command, including
overhead, exceeds 1400 bytes, a `MEMCACHED_WRITE_FAILURE` will be returned.

RETURN VALUE
------------

All methods return a value of type `memcached_return_t`.

On success the value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable string.

For :func:`memcached_replace` and :func:`memcached_add`, `MEMCACHED_NOTSTORED`
is a legitimate error in the case of a collision.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_prepend(3)`
    :manpage:`memcached_append(3)`
    :manpage:`memcached_cas(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
    * :doc:`memcached_auto`
    * :doc:`memcached_cas`
