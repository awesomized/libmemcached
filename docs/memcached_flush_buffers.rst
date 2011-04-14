=======================
Flushing client buffers
=======================


Flush buffers and send buffered commands


-------
LIBRARY
-------


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. c:function:: memcached_return_t memcached_flush_buffers (memcached_st *ptr);



-----------
DESCRIPTION
-----------


memcached_flush_buffers() is used in conjunction with 
MEMCACHED_BEHAVIOR_BUFFER_REQUESTS (see memcached_behavior(3)) to flush
all buffers by sending the buffered commands to the server for processing.


------
RETURN
------


A value of type \ ``memcached_return_t``\  is returned
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

Trond Norbye, <trond.norbye@gmail.com>


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
