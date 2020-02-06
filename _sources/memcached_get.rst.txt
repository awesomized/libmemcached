===============================
Retrieving data from the server
===============================

.. index:: object: memcached_st

--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function:: memcached_result_st * memcached_fetch_result (memcached_st *ptr, memcached_result_st *result, memcached_return_t *error)

.. c:function:: char * memcached_get (memcached_st *ptr, const char *key, size_t key_length, size_t *value_length, uint32_t *flags, memcached_return_t *error)

.. c:function::  memcached_return_t memcached_mget (memcached_st *ptr, const char * const *keys, const size_t *key_length, size_t number_of_keys)

.. c:function:: char * memcached_get_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, size_t *value_length, uint32_t *flags, memcached_return_t *error)

.. c:function:: memcached_return_t memcached_mget_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char * const *keys, const size_t *key_length, size_t number_of_keys)

.. c:function::  memcached_return_t memcached_fetch_execute (memcached_st *ptr, memcached_execute_fn *callback, void *context, uint32_t number_of_callbacks)

.. c:function:: memcached_return_t memcached_mget_execute (memcached_st *ptr, const char * const *keys, const size_t *key_length, size_t number_of_keys, memcached_execute_fn *callback, void *context, uint32_t number_of_callbacks)

.. c:function:: memcached_return_t memcached_mget_execute_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char * const *keys, const size_t *key_length, size_t number_of_keys, memcached_execute_fn *callback, void *context, uint32_t number_of_callbacks)

.. c:type:: memcached_return_t (*memcached_execute_fn)(const memcached_st *ptr, memcached_result_st *result, void *context)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_get` is used to fetch an individual value from the server. 
You must pass in a key and its length to fetch the object. You must supply
three pointer variables which will give you the state of the returned
object.  A :c:type:`uint32_t` pointer to contain whatever flags you stored with the value, a :c:type:`size_t` pointer which will be filled with size of of 
the object, and a :c:type:`memcached_return_t` pointer to hold any error. The 
object will be returned upon success and NULL will be returned on failure. Any 
object returned by :c:func:`memcached_get` must be released by the caller 
application.

:c:func:`memcached_mget` is used to select multiple keys at once. For 
multiple key operations it is always faster to use this function. This function always works asynchronously. 

To retrieve data after a successful execution of :c:func:`memcached_mget`, you will need to
call :c:func:`memcached_fetch_result`.  You should continue to call this function until 
it returns a NULL (i.e. no more values). If you need to quit in the middle of a
:c:func:`memcached_mget` call, you can execute a :c:func:`memcached_quit`, those this is not required.

:c:func:`memcached_fetch_result` is used to fetch an individual value from the server. :c:func:`memcached_mget` must always be called before using this method.  
You must pass in a key and its length to fetch the object. You must supply
three pointer variables which will give you the state of the returned
object.  A :c:type:`uint32_t` pointer to contain whatever flags you stored with the value, a :c:type:`size_t` pointer which will be filled with size of of the 
object, and a :c:type:`memcached_return_t` pointer to hold any error. The 
object will be returned upon success and NULL will be returned on failure. :c:type:`MEMCACHED_END` is returned by the \*error value when all objects that have been found are returned. The final value upon :c:type:`MEMCACHED_END` is null. 

:c:func:`memcached_fetch_result` is used to return a :c:type:`memcached_result_st` structure from a memcached server. The result object is forward compatible 
with changes to the server. For more information please refer to the 
:c:type:`memcached_result_st` help. This function will dynamically allocate a 
result structure for you if you do not pass one to the function.

:c:func:`memcached_fetch_execute` is a callback function for result sets. 
Instead of returning the results to you for processing, it passes each of the
result sets to the list of functions you provide. It passes to the function
a :c:type:`memcached_st` that can be cloned for use in the called 
function (it can not be used directly). It also passes a result set which does 
not need to be freed. Finally it passes a "context". This is just a pointer to 
a memory reference you supply the calling function. Currently only one value 
is being passed to each function call. In the future there will be an option 
to allow this to be an array.

:c:func:`memcached_mget_execute` and :c:func:`memcached_mget_execute_by_key`
is similar to :c:func:`memcached_mget`, but it may trigger the supplied 
callbacks with result sets while sending out the queries. If you try to 
perform a really large multiget with :c:func:`memcached_mget` you may 
encounter a deadlock in the OS kernel (it will fail to write data to the 
socket because the input buffer is full). :c:func:`memcached_mget_execute` 
solves this problem by processing some of the results before continuing 
sending out requests. Please note that this function is only available in 
the binary protocol.

:c:func:`memcached_get_by_key` and :c:func:`memcached_mget_by_key` behave 
in a similar nature as :c:func:`memcached_get` and :c:func:`memcached_mget`.
The difference is that they take a master key that is used for determining 
which server an object was stored if key partitioning was used for storage.

All of the above functions are not tested when the 
:c:type:`MEMCACHED_BEHAVIOR_USE_UDP` has been set. Executing any of these 
functions with this behavior on will result in :c:type:`MEMCACHED_NOT_SUPPORTED` being returned, or for those functions which do not return a :c:type:`memcached_return_t`, the error function parameter will be set to :c:type:`MEMCACHED_NOT_SUPPORTED`.


------
RETURN
------


All objects retrieved via :c:func:`memcached_get` or :c:func:`memcached_get_by_key` must be freed with :manpage:`free(3)`.

:c:func:`memcached_get` will return NULL on 
error. You must look at the value of error to determine what the actual error 
was.

:c:func:`memcached_fetch_execute` return :c:type:`MEMCACHED_SUCCESS` if
all keys were successful. :c:type:`MEMCACHED_NOTFOUND` will be return if no
keys at all were found.

:c:func:`memcached_fetch_result` sets error
to :c:type:`MEMCACHED_END` upon successful conclusion.
:c:type:`MEMCACHED_NOTFOUND` will be return if no keys at all were found.

----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
