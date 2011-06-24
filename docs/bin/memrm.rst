=================================
memrm - Remove data from a server
=================================


memrm - Remove a key(s) from a collection of memcached servers


--------
SYNOPSIS
--------

memrm [options] [key]

.. program:: memrm


-----------
DESCRIPTION
-----------


:program:`memrm`  removes items, specified by key, from memcached(1) servers.


-------
OPTIONS
-------


You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with the

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

