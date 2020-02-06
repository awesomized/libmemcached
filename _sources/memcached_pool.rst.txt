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
   Use :c:func:`memcached_pool`
 
.. c:function:: memcached_st* memcached_pool_destroy(memcached_pool_st* pool)
 
.. c:function:: memcached_st* memcached_pool_pop(memcached_pool_st* pool, bool block, memcached_return_t *rc)
.. deprecated:: 0.53
   Use :c:func:`memcached_pool_fetch`

.. c:function:: memcached_st* memcached_pool_fetch(memcached_pool_st*, struct timespec* relative_time, memcached_return_t* rc)

.. versionadded:: 0.53
   Synonym for memcached_pool_pop

.. c:function:: memcached_return_t memcached_pool_push(memcached_pool_st* pool, memcached_st *mmc)
.. deprecated:: 0.53
   Use :c:func:`memcached_pool_push`

.. c:function:: memcached_return_t memcached_pool_release(memcached_pool_st* pool, memcached_st* mmc)
.. versionadded:: 0.53
   Synonym for memcached_pool_push.
 
.. c:function:: memcached_return_t memcached_pool_behavior_set(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t data)
 
.. c:function:: memcached_return_t memcached_pool_behavior_get(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t *value)

Compile and link with -lmemcachedutil -lmemcached

-----------
DESCRIPTION
-----------


:c:func:`memcached_pool` is used to create a connection pool of objects you 
may use to remove the overhead of using memcached_clone for short lived
:c:type:`memcached_st` objects. Please see :doc:`libmemcached_configuration` for details on the format of the configuration string.

:c:func:`memcached_pool_destroy` is used to destroy the connection pool
created with :c:func:`memcached_pool_create` and release all allocated
resources. It will return the pointer to the :c:type:`memcached_st` structure
passed as an argument to :c:func:`memcached_pool_create`, and returns the ownership of the pointer to the caller when created with :c:func:`memcached_pool_create`, otherwise NULL is returned..

:c:func:`memcached_pool_fetch` is used to fetch a connection structure from the
connection pool. The relative_time argument specifies if the function should
block and wait for a connection structure to be available if we try
to exceed the maximum size. You need to specify time in relative time.

:c:func:`memcached_pool_release` is used to return a connection structure back to the pool.

:c:func:`memcached_pool_behavior_get` and :c:func:`memcached_pool_behavior_set` is used to get/set behavior flags on all connections in the pool.

Both :c:func:`memcached_pool_release` and :c:func:`memcached_pool_fetch` are thread safe.

------
RETURN
------

:c:func:`memcached_pool_destroy` returns the pointer (and ownership) to the :c:type:`memcached_st` structure used to create the pool. If connections are in use it returns NULL.

:c:func:`memcached_pool_pop` returns a pointer to a :c:type:`memcached_st` structure from the pool (or NULL if an allocation cannot be satisfied).

:c:func:`memcached_pool_release` returns :c:type:`MEMCACHED_SUCCESS` upon success.

:c:func:`memcached_pool_behavior_get` and :c:func:`memcached_pool_behavior_get` returns :c:type:`MEMCACHED_SUCCESS` upon success.

If any methods returns MEMCACHED_IN_PROGRESS then a lock on the pool could not be obtained. If any of the parameters passed to any of these functions is invalid, MEMCACHED_INVALID_ARGUMENTS will be returned.

memcached_pool_fetch may return MEMCACHED_TIMEOUT if a timeout occurs while waiting for a free memcached_st. MEMCACHED_NOTFOUND if no memcached_st was available.


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
