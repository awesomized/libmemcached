memslap
=======

SYNOPSIS
--------

memslap [options]

.. program::  memslap

Load testing and benchmarking a server

DESCRIPTION
-----------

:program:`memslap` is a load generation and benchmark tool for :manpage:`memcached(1)`
servers. It generates configurable workload such as threads, concurrencies, connections,
run time, overwrite, miss rate, key size, value size, get/set proportion, expected
throughput, and so on.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst

ENVIRONMENT
-----------

.. envvar:: MEMCACHED_SERVERS

    Specify the list of servers.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)`
  :manpage:`libmemcached(3)`

.. only:: html

* :doc:`/libmemcached`
