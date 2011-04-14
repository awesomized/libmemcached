====================================
Disconnecting a client from a server
====================================


--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. c:function:: void memcached_quit (memcached_st *ptr);

Compile and link with -lmemcached

-----------
DESCRIPTION
-----------


memcached_quit() will disconnect you from all currently connected servers.
It will also reset the state of the connection (ie, any memcached_fetch() you
are in the middle of will be terminated). This function is called
automatically when you call memcached_free() on the \ ``memcached_st``\  structure.

You do not need to call this on your own. All operations to change server
hashes and parameters will handle connections to the server for you. This
function is provided mainly so that you can timeout your connections or
reset connections during the middle of a memcached_fetch().


------
RETURN
------


A value of type \ ``memcached_return``\  is returned
On success that value will be \ ``MEMCACHED_SUCCESS``\ .
Use memcached_strerror() to translate this value to a printable string.


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

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
