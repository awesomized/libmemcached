================================================
Coverting Errors, memcached_return_t, to strings
================================================


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
     memcached_strerror (memcached_st *ptr,
                         memcached_return_t rc);



-----------
DESCRIPTION
-----------


memcached_strerror() takes a \ ``memcached_return_t``\  value and returns a string
describing the error.

This string must not be modified by the application.

\ ``memcached_return_t``\  values are returned from nearly all libmemcached(3) functions.

\ ``memcached_return_t``\  values are of an enum type so that you can set up responses
with switch/case and know that you are capturing all possible return values.


******
RETURN
******


memcached_strerror() returns a string describing a \ ``memcached_return_t``\  value.


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


memcached(1) libmemcached(3)

