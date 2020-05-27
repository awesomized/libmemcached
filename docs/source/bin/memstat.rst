memstat
=======

SYNOPSIS
--------

memstat [options]

.. program:: memstat

Gather statistics from a server

DESCRIPTION
-----------

:program:`memstat`  dumps the state of :manpage:`memcached(1)` servers.
It prints all data to stdout.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/sasl.rst

.. option:: --analyze

    Analyze and print differences of a server cluster. A memory and uptime comparison is performed by default.

    Available additional modes:

    --analyze=latency
        Network latency comparison



.. option:: --server-version

    Obtain and print server version(s) only.

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
* :doc:`/libmemcached/memcached_stats`
