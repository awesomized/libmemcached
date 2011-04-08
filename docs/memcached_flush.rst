.. highlight:: perl


memcached_flush
--------------*


Wipe contents of memcached servers


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   memcached_return_t
     memcached_flush (memcached_st *ptr,
                      time_t expiration);



-----------
DESCRIPTION
-----------


memcached_flush() is used to wipe clean the contents of memcached(1) servers.
It will either do this immediately or expire the content based on the
expiration time passed to the method (a value of zero causes an immediate
flush). The operation is not atomic to multiple servers, just atomic to a
single server. That is, it will flush the servers in the order that they were
added.


******
RETURN
******


A value of type \ ``memcached_return_t``\  is returned
On success that value will be \ ``MEMCACHED_SUCCESS``\ .
Use memcached_strerror() to translate this value to a printable string.


****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
