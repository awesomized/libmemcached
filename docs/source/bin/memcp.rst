=============================
memcp - Copy data to a server
=============================


Copies files to a collection of memcached servers


--------
SYNOPSIS
--------

memcp [options] [file] [server]

.. program:: memcp


-----------
DESCRIPTION
-----------


:program:`memcp` copies one or more files into memcached(1) servers.
It is similar to the standard UNIX cp(1) command.

The key names will be the names of the files,
without any directory path.


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

Mark Atwood, <mark@fallenpegasus.com>


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

