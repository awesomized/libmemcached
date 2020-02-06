=============================
memdump - Dumping your server
=============================


Dump a list of keys from a server.


--------
SYNOPSIS
--------

memdump [options]

.. program:: memdump

-----------
DESCRIPTION
-----------


:program:`memdump`  dumps a list of "keys" from all servers that 
it is told to fetch from. Because memcached does not guarentee to
provide all keys it is not possible to get a complete "dump".


-------
OPTIONS
-------


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


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)`
