=======================
Flushing client buffers
=======================


.. index:: object: memcached_st

Flush and senf buffered commands

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. c:function:: memcached_return_t memcached_flush_buffers (memcached_st *ptr)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_flush_buffers` is used in conjunction with 
:c:type:`MEMCACHED_BEHAVIOR_BUFFER_REQUESTS` (see memcached_behavior(3)) to flush all buffers by sending the buffered commands to the server for processing.


------
RETURN
------


A value of type :c:type:`memcached_return_t` is returned
On success that value will be :c:type:`MEMCACHED_SUCCESS`.
Use :c:func:`memcached_strerror` to translate this value to a printable 
string.


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
