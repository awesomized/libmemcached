Working with data on the server in an atomic fashion
====================================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_cas(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags, uint64_t cas)

.. function:: memcached_return_t memcached_cas_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags, uint64_t cas)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param group_key: key namespace
    :param group_key_length: length of the `group_key` without any trailing zero
    :param key: the key to check
    :param key_length: the length of the `key` without any trailing zero
    :param value: the value to set
    :param value_length: the length of the `value` without any trailing zero
    :param expiration: expiration time as unix timestamp or relative time in seconds
    :param flags: 16 bit flags
    :param cas: the ``cas`` value to compare
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`memcached_cas` overwrites data in the server as long as the ``cas`` value is
still the same in the server. You can get the ``cas`` value of a result by
calling `memcached_result_cas` on a `memcached_result_st` structure.

`memcached_cas_by_key` method behaves in a similar way as the non key methods.
The difference is that it uses the ``group_key`` parameter to map objects to
particular servers.

`memcached_cas` is tested with the `MEMCACHED_BEHAVIOR_USE_UDP` behavior
enabled. However, when using these operations with this behavior on, there are
limits to the size of the payload being sent to the server.  The reason for
these limits is that the Memcached Server does not allow multi-datagram requests
and the current server implementation sets a datagram size to 1400 bytes. Due to
protocol overhead, the actual limit of the user supplied data is less than 1400
bytes and depends on the protocol in use as, well as the operation being
executed. When running with the binary protocol,
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

All methods return a value of type `memcached_return_t`.
On success the value will be `MEMCACHED_SUCCESS`.
Use `memcached_strerror` to translate this value to a printable 
string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_set(3)`
    :manpage:`memcached_append(3)`
    :manpage:`memcached_add(3)`
    :manpage:`memcached_prepend(3)`
    :manpage:`memcached_replace(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_set`
    * :doc:`memcached_append`
    * :doc:`memcached_strerror`
