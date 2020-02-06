==========================
Dumping data from a server
==========================


Get a list of keys found on memcached servers

.. index:: object: memcached_st


--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>

.. c:function:: memcached_return_t memcached_dump (memcached_st *ptr, memcached_dump_fn *function, void *context, uint32_t number_of_callbacks)
 
.. c:type:: memcached_return_t (*memcached_dump_fn)(memcached_st *ptr,  const char *key, size_t key_length, void *context)

Compile and link with -lmemcached



-----------
DESCRIPTION
-----------


:c:func:`memcached_dump` is used to get a list of keys found in memcached(1) 
servers. Because memcached(1) does not guarentee to dump all keys you can not 
assume you have fetched all keys from the server. The function takes an array
of callbacks that it will use to execute on keys as they are found.

Currently the binary protocol is not testsed.


------
RETURN
------


A value of type :c:type:`memcached_return_t` is returned
On success that value will be :c:type:`MEMCACHED_SUCCESS`.
Use :c:func:`memcached_strerror` to translate this value to a printable 
string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
