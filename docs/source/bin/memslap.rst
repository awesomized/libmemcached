=================================================
memslap - Load testing  and benchmarking a server
=================================================

SYNOPSIS
--------

memslap [options]

.. program::  memslap

DESCRIPTION
-----------

:program:`memslap` is a load generation and benchmark tool for memcached(1)
servers. It generates configurable workload such as threads, concurrencies, connections, run time, overwrite, miss rate, key size, value size, get/set proportion, expected throughput, and so on. 

OPTIONS
-------

You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with:

.. option:: --help

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)`
