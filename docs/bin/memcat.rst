=================================
memcat - "cat" data from a server
=================================



--------
SYNOPSIS
--------

memcat [options] key

Copy a set of keys to stdout


.. program:: memcat


-----------
DESCRIPTION
-----------


:program:`memcat` outputs to stdout the value a single or multiple set of keys
stored in a memcached(1) server. If any key is not found an error is returned.

It is similar to the standard UNIX cat(1) utility.


-------
OPTIONS
-------


You can specify servers via the option:

.. cmdoption:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with the option:

.. cmdoption:: --help


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

