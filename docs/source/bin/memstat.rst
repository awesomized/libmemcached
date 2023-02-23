memstat
=======

SYNOPSIS
--------

memstat [options] [stat args] 

.. program:: memstat

Gather statistics from a server

DESCRIPTION
-----------

:program:`memstat`  dumps the state of :manpage:`memcached(1)` servers.
It prints all data to stdout.

OPTIONS
-------

.. include:: options/common_get.rst

.. option:: -A|--args <stat>

    Stat args.

    **DEPRECATED:** use positional arguments.

.. option:: -a|--analyze [<arg>]

    Analyze and print differences of a server cluster.
    A memory and uptime comparison is performed by default.

    Options:
    
    --analyze[=default]
        Memory and uptime comparison.

    --analyze=latency
        Network latency comparison.

.. option:: -S|-server-version

    Obtain and print server version(s) only.

.. option:: --iterations

    Iteration count of GETs sent by the latency test (default: 1000).


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
