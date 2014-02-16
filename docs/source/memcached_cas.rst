====================================================
Working with data on the server in an atomic fashion
====================================================

.. index:: object: memcached_st


--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>

.. c:function:: memcached_return_t memcached_cas(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags, uint64_t cas)

.. c:function:: memcached_return_t memcached_cas_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags, uint64_t cas)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

:c:func:`memcached_cas` overwrites data in the server as long as the "cas" 
value is still the same in the server. You can get the cas value of a result 
by calling :c:func:`memcached_result_cas` on a memcached_result_st(3) 
structure. At the point that this note was written cas is still buggy in memached. Turning on tests for it in libmemcached(3) is optional. Please see 
:c:func:`memcached_set` for information on how to do this.

:c:func:`memcached_cas_by_key` method behaves in a similar method as the non 
key methods. The difference is that it uses the group_key parameter 
to map objects to particular servers.

:c:func:`memcached_cas` is testsed with the :c:type:`MEMCACHED_BEHAVIOR_USE_UDP` behavior enabled. However, when using these operations with this behavior 
on, there are limits to the size of the payload being sent to the server.  The 
reason for these limits is that the Memcached Server does not allow 
multi-datagram requests and the current server implementation sets a datagram 
size to 1400 bytes. Due to protocol overhead, the actual limit of the user 
supplied data is less than 1400 bytes and depends on the protocol in use as, 
well as the operation being executed. When running with the binary protocol, 
:c:type:`MEMCACHED_BEHAVIOR_BINARY_PROTOCOL`, the size of the key,value, 
flags and expiry combined may not exceed 1368 bytes. When running with the 
ASCII protocol, the exact limit fluctuates depending on which function is 
being executed and whether the function is a cas operation or not. For 
non-cas ASCII set operations, there are at least 1335 bytes available to 
split among the key, key_prefix, and value; for cas ASCII operations there 
are at least 1318 bytes available to split among the key, key_prefix and value. If the total size of the command, including overhead, exceeds 1400 bytes, a :c:type:`MEMCACHED_WRITE_FAILURE` will be returned.


------
RETURN
------


All methods return a value of type :c:type:`memcached_return_t`.
On success the value will be :c:type:`MEMCACHED_SUCCESS`.
Use :c:func:`memcached_strerror` to translate this value to a printable 
string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemached(3)` :manpage:`memcached_strerror(3)` :manpage:`memcached_set(3)` :manpage:`memcached_append(3)` :manpage:`memcached_add(3)` :manpage:`memcached_prepend(3)` :manpage:`memcached_replace(3)`

