========================
Working with result sets
========================

--------
SYNOPSIS
--------

#include <libmemcached/memcached_pool.h>

.. c:type:: memcached_result_st

.. c:function:: memcached_result_st * memcached_result_create (memcached_st *ptr, memcached_result_st *result)

.. c:function:: void memcached_result_free (memcached_result_st *result)

.. c:function:: const char * memcached_result_key_value (memcached_result_st *result)

.. c:function:: size_t memcached_result_key_length (const memcached_result_st *result)

.. c:function:: const char *memcached_result_value (memcached_result_st *ptr)

.. c:function:: char *memcached_result_take_value (memcached_result_st *ptr)

.. c:function:: size_t memcached_result_length (const memcached_result_st *ptr)

.. c:function:: uint32_t memcached_result_flags (const memcached_result_st *result)

.. c:function:: uint64_t memcached_result_cas (const memcached_result_st *result)

.. c:function:: memcached_return_t memcached_result_set_value (memcached_result_st *ptr, const char *value, size_t length)

.. c:function:: void memcached_result_set_flags (memcached_result_st *ptr, uint32_t flags)

.. c:function:: void memcached_result_set_expiration (memcached_result_st *ptr, time_t)

Compile and link with -lmemcachedutil -lmemcached



-----------
DESCRIPTION
-----------


libmemcached(3) can optionally return a :c:type:`memcached_result_st` which 
acts as a result object. The result objects have added benefits over the 
character pointer returns, in that they are forward compatible with new 
return items that future memcached servers may implement (the best current 
example of this is the CAS return item). The structures can also be reused, 
which will save on calls to malloc(3). It is suggested that you use result 
objects over char \* return functions.

The structure of :c:type:`memcached_result_st` has been encapsulated, you should
not write code to directly access members of the structure.

:c:func:`memcached_result_create` will either allocate memory for a
:c:type:`memcached_result_st` or will initialize a structure passed to it.

:c:func:`memcached_result_free` will deallocate any memory attached to the
structure. If the structure was also allocated, it will deallocate it.

:c:func:`memcached_result_key_value` returns the key value associated with the
current result object.

:c:func:`memcached_result_key_length` returns the key length associated with 
the current result object.

:c:func:`memcached_result_value` returns the result value associated with the
current result object.

:c:func:`memcached_result_take_value` returns and hands over the result value
associated with the current result object. You must call free() to release this
value, unless you have made use of a custom allocator. Use of a custom
allocator requires that you create your own custom free() to release it.

:c:func:`memcached_result_length` returns the result length associated with 
the current result object.

:c:func:`memcached_result_flags` returns the flags associated with the
current result object.

:c:func:`memcached_result_cas` returns the cas associated with the
current result object. This value will only be available if the server
tests it.

:c:func:`memcached_result_set_value` takes a byte array and a size and sets
the result to this value. This function is used for trigger responses.

:c:func:`memcached_result_set_flags` takes a result structure and stores a new
value for the flags field.

:c:func:`memcached_result_set_expiration` takes a result structure and stores
a new value for the expiration field (this is only used by read through
triggers).

You may wish to avoid using memcached_result_create(3) with a
stack based allocation. The most common issues related to ABI safety involve
heap allocated structures.


------
RETURN
------


Varies, see particular functions. All structures must have
:c:func:`memcached_result_free` called on them for cleanup purposes. Failure 
to do this will result in leaked memory.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
