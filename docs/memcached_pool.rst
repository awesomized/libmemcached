============================
Working with memcached pools
============================

--------
SYNOPSIS
--------

#include <libmemcached/memcached_pool.h>

.. c:type:: memcached_pool_st

.. c:function:: memcached_pool_st* memcached_pool(const char *option_string, size_t option_string_length) 

.. c:function:: memcached_pool_st* memcached_pool_create(memcached_st* mmc, int initial, int max)
.. deprecated:: 0.46
   Use :c:func:`memcached_pool()` instead.
 
.. c:function:: memcached_st* memcached_pool_destroy(memcached_pool_st* pool)
 
.. c:function:: memcached_st* memcached_pool_pop (memcached_pool_st* pool, bool block, memcached_return_t *rc)
 
.. c:function:: memcached_return_t memcached_pool_push(memcached_pool_st* pool, memcached_st *mmc)
 
.. c:function:: memcached_return_t memcached_pool_behavior_set(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t data)
 
.. c:function:: memcached_return_t memcached_pool_behavior_get(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t *value)

Compile and link with -lmemcachedutil -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_pool()` is used to create a connection pool of objects you 
may use to remove the overhead of using memcached_clone for short lived
:c:type:`memcached_st` objects. Please see :doc:`libmemcached_configuration` for details on the format of the configuration string.

:c:func:`memcached_pool_destroy()` is used to destroy the connection pool
created with :c:func:`memcached_pool_create()` and release all allocated
resources. It will return the pointer to the :c:type:`memcached_st` structure
passed as an argument to :c:func:`memcached_pool_create()`, and returns the ownership of the pointer to the caller when created with :c:func:`memcached_pool_create()`, otherwise NULL is returned..

:c:func:`memcached_pool_pop()` is used to grab a connection structure from the
connection pool. The block argument specifies if the function should
block and wait for a connection structure to be available if we try
to exceed the maximum size.

:c:func:`memcached_pool_push()` is used to return a connection structure back to the pool.

:c:func:`memcached_pool_behavior_get()` and :c:func:`memcached_pool_behavior_set()` is used to get/set behavior flags on all connections in the pool.


------
RETURN
------

:c:func:`memcached_pool_destroy()` returns the pointer (and ownership) to the :c:type:`memcached_st` structure used to create the pool. If connections are in use it returns NULL.

:c:func:`memcached_pool_pop()` returns a pointer to a :c:type:`memcached_st` structure from the pool (or NULL if an allocation cannot be satisfied).

:c:func:`memcached_pool_push()` returns :c:type:`MEMCACHED_SUCCESS` upon success.

:c:func:`memcached_pool_behavior_get()` and :c:func:`memcached_pool_behavior_get()` returns :c:type:`MEMCACHED_SUCCESS` upon success.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>

Trond Norbye, <trond.norbye@gmail.com>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)` :manpage:`libmemcached_configuration(3)`
