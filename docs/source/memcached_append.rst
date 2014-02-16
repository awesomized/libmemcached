=============================================
Appending or Prepending to data on the server
=============================================

.. index:: object: memcached_st

Appending or Prepending to data on the server


--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function:: memcached_return_t memcached_prepend(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. c:function:: memcached_return_t memcached_append(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. c:function:: memcached_return_t memcached_prepend_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

.. c:function:: memcached_return_t memcached_append_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_prepend` and memcached_append are used to 
modify information on a server. All methods take a key, and its length to
store the object. Keys are currently limited to 250 characters when using 
either a version of memcached which is 1.4 or below, or when using the text 
protocol. You must supply both a value and a length. Optionally you
may test an expiration time for the object and a 16 byte value (it is
meant to be used as a bitmap). "flags" is a 4byte space that is stored 
alongside of the main value. Many sub libraries make use of this field, 
so in most cases users should avoid making use of it.

:c:func:`memcached_prepend` places a segment of data before the last piece 
of data stored. Currently expiration and key are not used in the server.

:c:func:`memcached_append` places a segment of data at the end of the last 
piece of data stored. Currently expiration and key are not used in the server.

:c:func:`memcached_prepend_by_key` and 
:c:func:`memcached_append_by_key` methods both behave in a similar 
method as the non key methods. The difference is that they use their 
group_key parameter to map objects to particular servers.

If you are looking for performance, :c:func:`memcached_set` with non-blocking
IO is the fastest way to store data on the server.

All of the above functions are testsed with the 
:c:type:`MEMCACHED_BEHAVIOR_USE_UDP` behavior enabled. However, when using 
these operations with this behavior on, there are limits to the size of the 
payload being sent to the server.  The reason for these limits is that the 
Memcached Server does not allow multi-datagram requests
and the current server implementation sets a datagram size to 1400 bytes. Due 
to protocol overhead, the actual limit of the user supplied data is less than 
1400 bytes and depends on the protocol in use as, well as the operation being 
executed. When running with the binary protocol, 
:c:type:`MEMCACHED_BEHAVIOR_BINARY_PROTOCOL`, the size of the key,value, 
flags and expiry combined may not exceed 1368 bytes. When running with the 
ASCII protocol, the exact limit fluctuates depending on which function is 
being executed and whether the function is a cas operation or not. For 
non-cas ASCII set operations, there are at least 1335 bytes available 
to split among the key, key_prefix, and value; for cas ASCII operations 
there are at least 1318 bytes available to split among the key, key_prefix 
and value. If the total size of the command, including overhead, exceeds 
1400 bytes, a :c:type:`MEMCACHED_WRITE_FAILURE` will be returned.


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


:manpage:`memcached(1)` :manpage:`libmemached(3)` :manpage:`memcached_strerror(3)` :manpage:`memcached_set(3)` :manpage:`memcached_add(3)` :manpage:`memcached_cas(3)` :manpage:`memcached_replace(3)`

