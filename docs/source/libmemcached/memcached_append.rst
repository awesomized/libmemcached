Appending or Prepending Data
============================

SYNOPSIS
--------

#include <libmemcached-|libmemcached_version|/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_prepend(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_append(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_prepend_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. function:: memcached_return_t memcached_append_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param group_key: key namespace
    :param group_key_length: length of the key namespace without any terminating zero
    :param key: the key
    :param key_length: length of the key without any terminating zero
    :param value: the value to append/prepend
    :param value_length: the length of the value without any terminating zero
    :param expiration: expiration as a unix timestamp or as relative expiration time in seconds
    :param flags: 16 bit flags
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`memcached_prepend` and memcached_append are used to modify information on a
server. All methods take a ``key``, and ``key_length`` to store the object. Keys
are currently limited to 250 characters when using either a version of memcached
which is 1.4 or below, or when using the text protocol. You must supply both a
value and a length. Optionally you may set an expiration time for the object
and a 16 bit value (it is meant to be used as a bitmap). ``flags`` is a 4 byte
space that is stored along the main value. Many sub libraries make use of
this field, so in most cases users should avoid making use of it.

`memcached_prepend` places a segment of data before the last piece of data
stored. Currently expiration and key are not used in the server.

`memcached_append` places a segment of data at the end of the last piece of data
stored. Currently expiration and key are not used in the server.

`memcached_prepend_by_key` and `memcached_append_by_key` methods both behave in
a similar manner as the non key methods. The difference is that they use their
group_key parameter to map objects to particular servers.

If you are looking for performance, `memcached_set` with non-blocking IO is the
fastest way to store data on the server.

All of the above functions are tested with the `MEMCACHED_BEHAVIOR_USE_UDP`
behavior enabled. However, when using these operations with this behavior on,
there are limits to the size of the payload being sent to the server.  The
reason for these limits is that the Memcached Server does not allow
multi-datagram requests and the current server implementation sets a datagram
size to 1400 bytes. Due to protocol overhead, the actual limit of the user
supplied data is less than 1400 bytes and depends on the protocol in use as,
well as the operation being executed. When running with the binary protocol,
`MEMCACHED_BEHAVIOR_BINARY_PROTOCOL`, the size of the key,value, flags and
expiry combined may not exceed 1368 bytes. When running with the ASCII protocol,
the exact limit fluctuates depending on which function is being executed and
whether the function is a cas operation or not. For non-cas ASCII set
operations, there are at least 1335 bytes available to split among the key,
key_prefix, and value; for cas ASCII operations there are at least 1318 bytes
available to split among the key, key_prefix and value. If the total size of the
command, including overhead, exceeds 1400 bytes, a `MEMCACHED_WRITE_FAILURE`
will be returned.

RETURN VALUE
------------

All methods return a value of type `memcached_return_t`. On success the value
will be `MEMCACHED_SUCCESS`.

Use `memcached_strerror` to translate this value to a printable string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_set(3)`
    :manpage:`memcached_add(3)`
    :manpage:`memcached_cas(3)`
    :manpage:`memcached_replace(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_set`
    * :doc:`memcached_cas`
    * :doc:`memcached_strerror`
