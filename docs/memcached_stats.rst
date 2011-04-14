==================================================
Working with statistical information from a server
==================================================


Get memcached statistics


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   memcached_stat_st *memcached_stat (memcached_st *ptr,
                                      char *args,
                                      memcached_return_t *error);
 
   memcached_return_t memcached_stat_servername (memcached_stat_st *stat,
                                                 char *args, 
                                                 const char *hostname,
                                                 unsigned int port);
 
   char *
     memcached_stat_get_value (memcached_st *ptr,
                               memcached_stat_st *stat, 
                               const char *key,
                               memcached_return_t *error);
 
   char ** 
     memcached_stat_get_keys (memcached_st *ptr,
                              memcached_stat_st *stat, 
                              memcached_return_t *error);
 
   memcached_return_t
     memcached_stat_execute (memcached_st *memc,
                             const char *args,
                             memcached_stat_fn func,
                             void *context);



-----------
DESCRIPTION
-----------


libmemcached(3) has the ability to query a memcached server (or collection
of servers) for their current state. Queries to find state return a
\ ``memcached_stat_st``\  structure. You are responsible for freeing this structure.
While it is possible to access the structure directly it is not advisable.
<memcached_stat_get_value() has been provided to query the structure.

memcached_stat_execute() uses the servers found in \ ``memcached_stat_st``\  and 
executes a "stat" command on each server. args is an optional argument that 
can be passed in to modify the behavior of "stats". You will need to supply
a callback function that will be supplied each pair of values returned by
the memcached server.

memcached_stat() fetches an array of \ ``memcached_stat_st``\  structures containing
the state of all available memcached servers. The return value must be freed
by the calling application. If called with the \ ``MEMCACHED_BEHAVIOR_USE_UDP``\ 
behavior set, a NULL value is returned and the error parameter is set to 
\ ``MEMCACHED_NOT_SUPPORTED``\ .

memcached_stat_servername() can be used standalone without a \ ``memcached_st``\  to
obtain the state of a particular server.  "args" is used to define a
particular state object (a list of these are not provided for by either
the memcached_stat_get_keys() call nor are they defined in the memcached
protocol). You must specify the hostname and port of the server you want to
obtain information on.

memcached_stat_get_value() returns the value of a particular state key. You
specify the key you wish to obtain.  The key must be null terminated.

memcached_stat_get_keys() returns a list of keys that the server has state
objects on. You are responsible for freeing this list.

A command line tool, memstat(1), is provided so that you do not have to write
an application to do this.


******
RETURN
******


Varies, see particular functions.

Any method returning a \ ``memcached_stat_st``\  expects you to free the
memory allocated for it.


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

