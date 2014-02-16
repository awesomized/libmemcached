=================
memcached_fetch
=================

.. index:: object: memcached_st

--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function::  char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, size_t *value_length, uint32_t *flags, memcached_return_t *error)

   .. deprecated:: 0.50
      Use :c:func:`memcached_fetch_result` instead.

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

:c:func:`memcached_fetch` is used to fetch an individual value from the server. :c:func:`memcached_mget` must always be called before using this method.  
You must pass in a key and its length to fetch the object. You must supply
three pointer variables which will give you the state of the returned
object.  A :c:type:`uint32_t` pointer to contain whatever flags you stored with the value, a :c:type:`size_t` pointer which will be filled with size of of the 
object, and a :c:type:`memcached_return_t` pointer to hold any error. The 
object will be returned upon success and NULL will be returned on failure. :c:type:`MEMCACHED_END` is returned by the \*error value when all objects that have been found are returned. The final value upon :c:type:`MEMCACHED_END` is null. 

Values returned by :c:func:`memcached_fetch` must be freed by the caller. 

All of the above functions are not tested when the 
:c:type:`MEMCACHED_BEHAVIOR_USE_UDP` has been set. Executing any of these 
functions with this behavior on will result in :c:type:`MEMCACHED_NOT_SUPPORTED` being returned, or for those functions which do not return a :c:type:`memcached_return_t`, the error function parameter will be set to :c:type:`MEMCACHED_NOT_SUPPORTED`.


------
RETURN
------

:c:func:`memcached_fetch` sets error to 
to :c:type:`MEMCACHED_END` upon successful conclusion.
:c:type:`MEMCACHED_NOTFOUND` will be return if no keys at all were found.

:c:type:`MEMCACHED_KEY_TOO_BIG` is set to error whenever :c:func:`memcached_fetch` was used
and the key was set larger then :c:type:`MEMCACHED_MAX_KEY`, which was the largest
key allowed for the original memcached ascii server.


----
HOME
----

To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)` :manpage:`memcached_fetch_result(3)`

