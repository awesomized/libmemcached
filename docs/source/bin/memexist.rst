memexist
========

SYNOPSIS
--------

.. program:: memexist

memexist [options] <key>

Check for the existence of a key.

DESCRIPTION
-----------

:program:`memexist` checks for the existence of a key within a cluster.

OPTIONS
-------

.. include:: options/common_get.rst
.. include:: options/hash.rst

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
    * :doc:`/libmemcached/memcached_exist`
