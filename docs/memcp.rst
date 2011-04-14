=============================
memcp - Copy data to a server
=============================


Copies files to a collection of memcached servers


--------
SYNOPSIS
--------

:program: `memcp`

.. program:: memcp

.. option:: --help


-----------
DESCRIPTION
-----------


\ **memcp**\  copies one or more files into memcached(1) servers.
It is similar to the standard UNIX cp(1) command.

The key names will be the names of the files,
without any directory path part.

You can specify servers via the \ **--servers**\  option or via the
environment variable \ ``MEMCACHED_SERVERS``\ . If you specify neither of
these, the final value in the command line list is the name of a
server(s).

For a full list of operations run the tool with the \ **--help**\  option.


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


memcached(1) libmemcached(3)

