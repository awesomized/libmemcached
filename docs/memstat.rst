=========================================
memstat - Gather statistics from a server
=========================================


memstat - Display the operating status of a single or group of memcached servers


--------
SYNOPSIS
--------

:program: `memstat`

.. program:: memstat

.. option:: --help

-----------
DESCRIPTION
-----------


\ **memstat**\  dumps the state of memcached(1) servers.
It displays all data to stdout.

You can specify servers via the \ **--servers**\  option or via the
environment variable \ ``MEMCACHED_SERVERS``\ . \ **--args**\  can be used
to specify the "argument" sent to the stats command (ie slab, size, items,
etc..).

For a full list of operations run the tool with the \ **--help**\  option.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


memcached(1) libmemcached(3)

