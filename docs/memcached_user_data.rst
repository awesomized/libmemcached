.. highlight:: perl


memcached_set_user_data, memcached_get_user_data
************************************************


Manage user specific data


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


********
SYNOPSIS
********



.. code-block:: perl

   #include <memcached.h>
 
   void *memcached_get_user_data (memcached_st *ptr);
 
   void *memcached_set_user_data (memcached_st *ptr, void *data);



***********
DESCRIPTION
***********


libmemcached(3) allows you to store a pointer to a user specific data inside
the memcached_st structure.

memcached_set_user_data() is used to set the user specific data in the
memcached_st structure.

memcached_get_user_data() is used to retrieve the user specific data in
the memcached_st structure.


******
RETURN
******


memcached_set_user_data() returns the previous value of the user specific 
data.

memcached_get_user_data() returns the current value uf the user specific
data.


****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Trond Norbye, <trond.norbye@gmail.com>


********
SEE ALSO
********


memcached(1) libmemcached(3)

