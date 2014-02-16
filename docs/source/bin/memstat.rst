=========================================
memstat - Gather statistics from a server
=========================================


memstat - Display the operating status of a single or group of memcached servers


--------
SYNOPSIS
--------

memstat [options]

.. program:: memstat

-----------
DESCRIPTION
-----------


:program:`memstat`  dumps the state of memcached(1) servers.
It displays all data to stdout.


-------
OPTIONS
-------


You can specify servers via the option:

.. option:: --servers  

or via the environment variable:

.. envvar:: 'MEMCACHED_SERVERS, --args'

which can be used to specify the "argument" sent to the stats command (ie slab, size, items, etc..).

For a full list of operations run the tool with:

.. option:: --help

.. option:: --analyze  

----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

