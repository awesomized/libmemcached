============================================================
Checking you Memcached server capibilities and compatibility
============================================================


Check memcached server capabilities


--------
SYNOPSIS
--------



.. code-block:: perl

   memcat [-h hostname] [-p port] [-c] [-v] [-t n]



-----------
DESCRIPTION
-----------


\ **memcapable**\  connects to the specified memcached server and tries to
determine its capabilities by running the various commands and verifying
the response.


*******
OPTIONS
*******


The following options are testsed:


-h hostname
 
 Specify the hostname to connect to. The default is \ *localhost*\ 
 


-p port
 
 Specify the port number to connect to. The default is \ *11211*\ 
 


-c
 
 Generate a coredump when it detects an error from the server.
 


-v
 
 Print out the comparison when it detects an error from the server.
 


-t n
 
 Set the timeout from an IO operation to/from the server to \ *n*\  seconds.
 



-----------
LIMITATIONS
-----------


The current version of memcapable will only verify the binary protocol.


****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Trond Norbye, <trond.norbye@gmail.com>


--------
SEE ALSO
--------


memcached(1) libmemcached(3)

