memdump
=======

SYNOPSIS
--------

.. program:: memdump

memdump [options]

Dump a list of keys from a server.

DESCRIPTION
-----------

:program:`memdump`  dumps a list of "keys" from all servers that
it is told to fetch from. Because memcached does not guarantee to
provide all keys it is not possible to get a complete "dump".

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/hash.rst
.. include:: options/sasl.rst

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
    * :doc:`/libmemcached/memcached_dump`
