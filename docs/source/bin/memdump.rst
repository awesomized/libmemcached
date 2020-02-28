=============================
memdump - Dumping your server
=============================

Dump a list of keys from a server.

SYNOPSIS
--------

memdump [options]

.. program:: memdump

DESCRIPTION
-----------

:program:`memdump`  dumps a list of "keys" from all servers that
it is told to fetch from. Because memcached does not guarantee to
provide all keys it is not possible to get a complete "dump".

OPTIONS
-------

For a full list of operations run the tool with option:

.. option:: --help

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)`
