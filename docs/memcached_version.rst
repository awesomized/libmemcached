=======================================================
Getting version information about the client and server
=======================================================


Get library version


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   const char *
     memcached_lib_version (void) 
 
 
   memcached_return_t
     memcached_version (memcached_st *ptr)



-----------
DESCRIPTION
-----------


memcached_lib_version() is used to return a simple version string representing
the libmemcached version (version of the client library, not server)

memcached_version() is used to set the major, minor, and micro versions of each
memcached server being used by the memcached_st connection structure. It returns the 
memcached server return code.


******
RETURN
******


A string with the version of libmemcached driver is returned from
memcached_lib_version()

A value of type \ ``memcached_return_t``\  is returned from memcached_version()
On success that value will be \ ``MEMCACHED_SUCCESS``\ . If called with the
\ ``MEMCACHED_BEHAVIOR_USE_UDP``\  behavior set, the value \ ``MEMCACHED_NOT_SUPPORTED``\  
will be returned. Use memcached_strerror() to translate this value to 
a printable string.


****
HOME
****


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


******
AUTHOR
******


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------


memcached(1) libmemcached(3) memcached_strerror(3)

