Working with memcached pools
============================

SYNOPSIS
--------

#include <libmemcachedutil-|libmemcached_version|/pool.h>
  Compile and link with -lmemcachedutil -lmemcached

.. type:: struct memcached_pool_st memcached_pool_st

.. function:: memcached_pool_st* memcached_pool(const char *option_string, size_t option_string_length)

.. function:: memcached_st* memcached_pool_destroy(memcached_pool_st* pool)

.. function:: memcached_st* memcached_pool_fetch(memcached_pool_st*, struct timespec* relative_time, memcached_return_t* rc)

    .. versionadded:: 0.53
       Synonym for memcached_pool_pop

.. function:: memcached_return_t memcached_pool_release(memcached_pool_st* pool, memcached_st* mmc)

    .. versionadded:: 0.53
       Synonym for memcached_pool_push.

.. function:: memcached_return_t memcached_pool_behavior_set(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t data)

.. function:: memcached_return_t memcached_pool_behavior_get(memcached_pool_st *pool, memcached_behavior_t flag, uint64_t *value)

.. function:: memcached_pool_st* memcached_pool_create(memcached_st* mmc, int initial, int max)

    .. deprecated:: 0.46
       Use `memcached_pool`

.. function:: memcached_st* memcached_pool_pop(memcached_pool_st* pool, bool block, memcached_return_t *rc)

    .. deprecated:: 0.53
       Use `memcached_pool_fetch`

.. function:: memcached_return_t memcached_pool_push(memcached_pool_st* pool, memcached_st *mmc)

    .. deprecated:: 0.53
       Use `memcached_pool_release`

DESCRIPTION
-----------

`memcached_pool` is used to create a connection pool of objects you may use to
remove the overhead of using memcached_clone for short lived `memcached_st`
objects. Please see :doc:`../libmemcached/configuration` for details on the
format of the configuration string.

`memcached_pool_destroy` is used to destroy the connection pool created with
`memcached_pool_create` and release all allocated resources. It will return the
pointer to the `memcached_st` structure passed as an argument to
`memcached_pool_create`, and returns the ownership of the pointer to the caller
when created with `memcached_pool_create`, otherwise NULL is returned..

`memcached_pool_fetch` is used to fetch a connection structure from the
connection pool. The relative_time argument specifies if the function should
block and wait for a connection structure to be available if we try to exceed
the maximum size. You need to specify time in relative time.

`memcached_pool_release` is used to return a connection structure back to the
pool.

`memcached_pool_behavior_get` and `memcached_pool_behavior_set` is used to
get/set behavior flags on all connections in the pool.

Both `memcached_pool_release` and `memcached_pool_fetch` are thread safe.

RETURN VALUE
------------

`memcached_pool_destroy` returns the pointer (and ownership) to the
`memcached_st` structure used to create the pool. If connections are in use it
returns NULL.

`memcached_pool_pop` returns a pointer to a `memcached_st` structure from the
pool (or NULL if an allocation cannot be satisfied).

`memcached_pool_release` returns `MEMCACHED_SUCCESS` upon success.

`memcached_pool_behavior_get` and `memcached_pool_behavior_get` returns
`MEMCACHED_SUCCESS` upon success.

If any methods returns MEMCACHED_IN_PROGRESS then a lock on the pool could not
be obtained. If any of the parameters passed to any of these functions is
invalid, MEMCACHED_INVALID_ARGUMENTS will be returned.

memcached_pool_fetch may return MEMCACHED_TIMEOUT if a timeout occurs while
waiting for a free memcached_st. MEMCACHED_NOTFOUND if no memcached_st was
available.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`libmemcached_configuration(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`../libmemcached/configuration`
    * :doc:`../libmemcached/memcached_strerror`
