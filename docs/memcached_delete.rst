.. highlight:: perl


memcached_delete
****************


Delete a key


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


********
SYNOPSIS
********



.. code-block:: perl

   #include <memcached.h>
 
   memcached_return_t
     memcached_delete (memcached_st *ptr,
                       const char *key, size_t key_length,
                       time_t expiration);
 
   memcached_return_t
   memcached_delete_by_key (memcached_st *ptr,
                            const char *master_key, size_t master_key_length,
                            const char *key, size_t key_length,
                            time_t expiration);



***********
DESCRIPTION
***********


memcached_delete() is used to delete a particular key. 
memcached_delete_by_key() works the same, but it takes a master key to
find the given value.

Expiration works by placing the item into a delete queue, which means that
it won't possible to retrieve it by the "get" command, but "add" and 
"replace" command with this key will also fail (the "set" command will 
succeed, however). After the time passes, the item is finally deleted from server memory.

Please note the the Danga memcached server removed support for expiration in
the 1.4 version.


******
RETURN
******


A value of type \ ``memcached_return_t``\  is returned
On success that value will be \ ``MEMCACHED_SUCCESS``\ .
Use memcached_strerror() to translate this value to a printable string.

If you are using the non-blocking mode of the library, success only
means that the message was queued for delivery.


****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Brian Aker, <brian@tangent.org>


********
SEE ALSO
********


memcached(1) libmemcached(3) memcached_strerror(3)

