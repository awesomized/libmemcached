=======================================
memflush - flush all data from a server
=======================================


Reset a server or list of servers


--------
SYNOPSIS
--------

memflush [options]

.. program:: memflush

-----------
DESCRIPTION
-----------


:program:`memflush`  resets the contents of memcached(1) servers.
This means that all data in the specified servers will be deleted.


-------
OPTIONS
-------


You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with option:

.. option:: --help


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>

Mark Atwood <mark@fallenpegasus.com>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)`
