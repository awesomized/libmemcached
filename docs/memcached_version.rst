=======================================================
Getting version information about the client and server
=======================================================

 
--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:function:: const char * memcached_lib_version (void) 

.. c:function:: memcached_return_t memcached_version (memcached_st *ptr)


Compile and link with -lmemcached



-----------
DESCRIPTION
-----------


memcached_lib_version() is used to return a simple version string representing
the libmemcached version (client library version, not server version)

memcached_version() is used to set the major, minor, and micro versions of each
memcached server being used by the memcached_st connection structure. It returns the 
memcached server return code.


------
RETURN
------


A string with the version of libmemcached driver is returned from
memcached_lib_version()

A value of type \ ``memcached_return_t``\  is returned from memcached_version()
On success that value will be \ ``MEMCACHED_SUCCESS``\ . If called with the
\ ``MEMCACHED_BEHAVIOR_USE_UDP``\  behavior set, the value \ ``MEMCACHED_NOT_SUPPORTED``\  
will be returned. Use memcached_strerror() to translate this value to 
a printable string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


memcached(1) libmemcached(3) memcached_strerror(3)

