============================
Working with memcached pools
============================

.. index:: object: memcached_pool_st

--------
SYNOPSIS
--------

#include <libmemcached/memcached_pool.h>

.. c:function:: memcached_pool_st *memcached_pool(const char *option_string, size_t option_string_length); 

.. c:function:: memcached_pool_st * memcached_pool_create(memcached_st* mmc, int initial, int max); DEPRECATED
 
.. c:function:: memcached_st * memcached_pool_destroy(memcached_pool_st* pool);
 
.. c:function:: memcached_st * memcached_pool_pop (memcached_pool_st* pool, bool block, memcached_return_t *rc);
 
.. c:function:: memcached_return_t memcached_pool_push(memcached_pool_st* pool, memcached_st *mmc);
 
.. c:function:: memcached_st *memcached_create (memcached_st *ptr);
 
.. c:function:: memcached_return_t memcached_pool_behavior_set(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t data)
 
.. c:function:: memcached_return_t memcached_pool_behavior_get(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t *value)

Compile and link with -lmemcachedutil -lmemcached


-----------
DESCRIPTION
-----------


memcached_pool() is used to create a connection pool of objects you may use
to remove the overhead of using memcached_clone for short lived
\ ``memcached_st``\ objects. Please see :manpage:`libmemcached_configuration` for details on the format of the configuration string.

DEPRECATED memcached_pool_create() is used to create a connection pool of
objects you may use to remove the overhead of using memcached_clone for
short lived \ ``memcached_st``\ objects. The mmc argument should be an
initialised \ ``memcached_st``\ structure, and a successfull invocation of
memcached_pool_create takes full ownership of the variable (until it is
released by memcached_pool_destroy).  The \ ``initial``\  argument specifies
the initial size of the connection pool, and the \ ``max``\ argument
specifies the maximum size the connection pool should grow to. Please note
that the library will allocate a fixed size buffer scaled to the max size of
the connection pool, so you should not pass MAXINT or some other large
number here.

memcached_pool_destroy() is used to destroy the connection pool
created with memcached_pool_create() and release all allocated
resources. It will return the pointer to the \ ``memcached_st``\  structure
passed as an argument to memcached_pool_create(), and returns the ownership
of the pointer to the caller when created with memcached_pool_create,
otherwise NULL is returned..

memcached_pool_pop() is used to grab a connection structure from the
connection pool. The block argument specifies if the function should
block and wait for a connection structure to be available if we try
to exceed the maximum size.

memcached_pool_push() is used to return a connection structure back to the pool.

memcached_pool_behavior_set() and memcached_pool_behagior_get() is
used to get/set behavior flags on all connections in the pool.


------
RETURN
------


memcached_pool_create() returns a pointer to the newly created
memcached_pool_st structure. On an allocation failure, it returns
NULL.

memcached_pool_destroy() returns the pointer (and ownership) to the
memcached_st structure used to create the pool. If connections are in
use it returns NULL.

memcached_pool_pop() returns a pointer to a memcached_st structure
from the pool (or NULL if an allocation cannot be satisfied).

memcached_pool_push() returns MEMCACHED_SUCCESS upon success.

memcached_pool_behavior_get() and memcached_pool_behavior_get()
returns MEMCACHED_SUCCESS upon success.


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

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
