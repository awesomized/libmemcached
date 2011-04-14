=============================
memdump - Dumping your server
=============================


Dump a list of keys from a server.


--------
SYNOPSIS
--------

:program: `memdump`

.. program:: memdump

.. option:: --help

-----------
DESCRIPTION
-----------


\ **memdump**\  currently dumps a list of "keys" from all servers that 
it is told to fetch from. Because memcached does not guarentee to
provide all keys it is not possible to get a complete "dump".

For a full list of operations run the tool with the \ **--help**\  option.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)`
