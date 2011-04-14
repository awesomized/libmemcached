==========================
Dumping data from a server
==========================


Get a list of keys found on memcached servers


-------
LIBRARY
-------


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   memcached_return_t
     memcached_dump (memcached_st *ptr, 
                     memcached_dump_fn *function, 
                     void *context, 
                     uint32_t number_of_callbacks);
 
   typedef memcached_return_t (*memcached_dump_fn)(memcached_st *ptr,  
                                                   const char *key, 
                                                   size_t key_length, 
                                                   void *context);



-----------
DESCRIPTION
-----------


memcached_dump() is used to get a list of keys found  memcached(1) servers.
Because memcached(1) does not guarentee to dump all keys you can not assume
you have fetched all keys from the server. The function takes an array
of callbacks that it will use to execute on keys as they are found.

Currently the binar protocol is not testsed.


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


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
