=======================
Flushing client buffers
=======================


.. index:: object: memcached_st

Flush and send buffered commands

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. function:: memcached_return_t memcached_flush_buffers (memcached_st *ptr)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:func:`memcached_flush_buffers` is used in conjunction with 
`MEMCACHED_BEHAVIOR_BUFFER_REQUESTS` (see memcached_behavior(3)) to flush all buffers by sending the buffered commands to the server for processing.


------
RETURN
------


A value of type :type:`memcached_return_t` is returned
On success that value will be `MEMCACHED_SUCCESS`.
Use :func:`memcached_strerror` to translate this value to a printable 
string.




--------
SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
