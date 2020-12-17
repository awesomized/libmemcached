libmemcachedutil - C/C++ utilities extending libmemcached
=========================================================

Utility library for libmemcached

SYNOPSIS
--------

#include <libmemcachedutil-|libmemcachedutil_version|/util.h>
  Compile and link with -lmemcachedutil -lmemcached

DESCRIPTION
-----------

`libmemcachedutil` is a small and thread-safe client library that provides extra
functionality built on top of `libmemcached`.

THREADS AND PROCESSES
---------------------

Do not try to access an instance of `memcached_st` from multiple threads at the
same time. If you want to access memcached from multiple threads you should
either clone the `memcached_st`, or use the memcached pool implementation. See
`memcached_pool`.

.. toctree::
    :titlesonly:
    :caption: Additional Utilities

    memcached_pool

SEE ALSO
--------

.. only:: man

    :manpage:`libmemcached(3)`
    :manpage:`memcached_pool(3)`
    :manpage:`memcached_pool_destroy(3)`
    :manpage:`memcached_pool_pop(3)`
    :manpage:`memcached_pool_push(3)`

.. only:: html

    * :doc:`../libmemcached`

