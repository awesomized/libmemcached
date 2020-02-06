=================
Setting callbacks
=================


Get and set a callback

.. index:: object: memcached_st


--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:type:: memcached_callback_t
 
.. c:function:: memcached_return_t memcached_callback_set (memcached_st *ptr, memcached_callback_t flag, const void *data)
 
.. c:function:: void * memcached_callback_get (memcached_st *ptr, memcached_callback_t flag, memcached_return_t *error)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


libmemcached(3) can have callbacks set key execution points. These either
provide function calls at points in the code, or return pointers to
structures for particular usages.

:c:func:`memcached_callback_get` takes a callback flag and returns the 
structure or function set by :c:func:`memcached_callback_set`.

:c:func:`memcached_callback_set` changes the function/structure assigned by a
callback flag. No connections are reset.

You can use :c:type:`MEMCACHED_CALLBACK_USER_DATA` to provide custom context 
if required for any of the callbacks.


.. c:type:: MEMCACHED_CALLBACK_CLEANUP_FUNCTION
 
When :c:func:`memcached_delete` is called this function will be excuted. At 
the point of its execution all connections are closed.
 


.. c:type:: MEMCACHED_CALLBACK_CLONE_FUNCTION
 
When :c:func:`memcached_delete` is called this function will be excuted. 
At the point of its execution all connections are closed.

.. c:type:: MEMCACHED_CALLBACK_PREFIX_KEY

   See :c:type:`MEMCACHED_CALLBACK_NAMESPACE`

.. c:type:: MEMCACHED_CALLBACK_NAMESPACE
 
You can set a value which will be used to create a domain for your keys.
The value specified here will be prefixed to each of your keys. The value can
not be greater then :c:macro:`MEMCACHED_PREFIX_KEY_MAX_SIZE` - 1 and will
reduce :c:macro:`MEMCACHED_MAX_KEY` by the value of your key. 

The prefix key is only applied to the primary key, not the master key. 
:c:type:`MEMCACHED_FAILURE` will be returned if no key is set. In the case of 
a key which is too long, :c:type:`MEMCACHED_BAD_KEY_PROVIDED` will be returned.
 
If you set a value with the value being NULL then the prefix key is disabled.

.. c:type:: MEMCACHED_CALLBACK_USER_DATA
 
This allows you to store a pointer to a specifc piece of data. This can be
retrieved from inside of :c:func:`memcached_fetch_execute`. Cloning a 
:c:type:`memcached_st` will copy the pointer to the clone.
 
.. c:type:: MEMCACHED_CALLBACK_MALLOC_FUNCTION
.. deprecated:: <0.32
   Use :c:type:`memcached_set_memory_allocators` instead.

.. c:type:: MEMCACHED_CALLBACK_REALLOC_FUNCTION
.. deprecated:: <0.32
   Use :c:type:`memcached_set_memory_allocators` instead.
 
.. c:type:: MEMCACHED_CALLBACK_FREE_FUNCTION
.. deprecated:: <0.32
   Use :c:type:`memcached_set_memory_allocators` instead.

.. c:type:: MEMCACHED_CALLBACK_GET_FAILURE
 
This function implements the read through cache behavior. On failure of retrieval this callback will be called. 

You are responsible for populating the result object provided. This result object will then be stored in the server and returned to the calling process. 

You must clone the :c:type:`memcached_st` in order to
make use of it. The value will be stored only if you return
:c:type:`MEMCACHED_SUCCESS` or :c:type:`MEMCACHED_BUFFERED`. Returning 
:c:type:`MEMCACHED_BUFFERED` will cause the object to be buffered and not sent 
immediatly (if this is the default behavior based on your connection setup 
this will happen automatically).
 
The prototype for this is:

.. c:function:: memcached_return_t (\*memcached_trigger_key)(memcached_st \*ptr, char \*key, size_t key_length, memcached_result_st \*result);
 


.. c:type:: MEMCACHED_CALLBACK_DELETE_TRIGGER
 
This function implements a trigger upon successful deletion of a key. The memcached_st structure will need to be cloned in order to make use of it.
 
The prototype for this is: 

.. c:function:: typedef memcached_return_t (\*memcached_trigger_delete_key)(memcached_st \*ptr, char \*key, size_t key_length);
 



------
RETURN
------


:c:func:`memcached_callback_get` return the function or structure that was 
provided. Upon error, nothing is set, null is returned, and the 
:c:type:`memcached_return_t` argument is set to :c:type:`MEMCACHED_FAILURE`.

:c:func:`memcached_callback_set` returns :c:type:`MEMCACHED_SUCCESS` upon 
successful setting, otherwise :c:type:`MEMCACHED_FAILURE` on error.


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
