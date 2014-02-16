=========================
memtouch - Touches a key.
=========================


--------
SYNOPSIS
--------

memtouch [options] key

.. program:: memtouch


-----------
DESCRIPTION
-----------


:program:`memtouch` does a "touch" on the specified key.

-------
OPTIONS
-------


You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

If you do not specify either these, the final value in the command line list is the name of a server(s).

For a full list of operations run the tool with the option:

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

--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

