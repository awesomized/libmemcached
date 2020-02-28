=================================
memrm - Remove data from a server
=================================

memrm - Remove a key(s) from a collection of memcached servers

SYNOPSIS
--------

memrm [options] [key]

.. program:: memrm

DESCRIPTION
-----------

:program:`memrm`  removes items, specified by key, from memcached(1) servers.

OPTIONS
-------

You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with the

.. option:: --help

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)`

