============================
Introducing libmemcachedutil
============================


Utility library for libmemcached


--------
SYNOPSIS
--------

#include <libmemcached/memcached_util.h>
  Compile and link with -lmemcachedutil


-----------
DESCRIPTION
-----------


`libmemcachedutil` is a small and thread-safe client library that
provides extra functionality built on top of `libmemcached`.


-------
THREADS
-------

Do not try to access an instance of :c:type:`memcached_st` from multiple threads
at the same time. If you want to access memcached from multiple threads
you should either clone the :c:type:`memcached_st`, or use the memcached pool
implementation. See :c:func:`memcached_pool_create`.

--------
SEE ALSO
--------

.. only:: man

    :manpage:`libmemcached(3)`
    :manpage:`memcached_pool(3)`
    :manpage:`memcached_pool_destroy(3)`
    :manpage:`memcached_pool_pop(3)`
    :manpage:`memcached_pool_push(3)`

.. only:: html

    * :doc:`libmemcached`
    * :doc:`memcached_pool`
