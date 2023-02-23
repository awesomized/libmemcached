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

.. include:: options/common_get.rst
.. include:: options/hash.rst
.. include:: options/udp.rst

.. option:: -R|--noreply

    Enable the NOREPLY behavior for storage commands.

.. option:: -F|--flush

    Flush all servers prior test.

.. option:: -t|--test <arg>

    Test to perform (options: get, mget, set; default: get).

.. option:: -c|--concurrency <num>

    Concurrency (number of threads to start; default: 1).

.. option:: -e|--execute-number <num>

    Number of times to execute the tests (default: 10000).

.. option:: -l|--initial-load <num>

    Number of keys to load before executing tests (default: 10000).


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
