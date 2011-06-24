=======================================================================
memcapable - Checking a Memcached server capibilities and compatibility
=======================================================================

--------
SYNOPSIS
--------

memcapable [options]

.. program:: memcapable

-----------
DESCRIPTION
-----------

:program:`memcapable` connects to the specified memcached server and tries to
determine its capabilities by running the various commands and verifying
the response.


-----------
LIMITATIONS
-----------


The current version of memcapable will only verify the binary protocol.


-------
OPTIONS
-------


.. option:: -h hostname
 
Specify the hostname to connect to. The default is \ *localhost*\ 

.. option:: -p port
 
Specify the port number to connect to. The default is \ *11211*\ 
 
.. option:: -c
 
Generate a coredump when it detects an error from the server.
 
.. option:: -v
 
Print out the comparison when it detects an error from the server.
 
.. option:: -t n
 
Set the timeout from an IO operation to/from the server to \ *n*\  seconds.
 

----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Trond Norbye, <trond.norbye@gmail.com>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)`

