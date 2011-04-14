========================================================
Manipulate the server information stored in memcached_st
========================================================


Manage server list


*******
LIBRARY
*******


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   uint32_t memcached_server_count (memcached_st *ptr);
 
   memcached_return_t memcached_server_add (memcached_st *ptr, const char *hostname, in_port_t port);
 
   memcached_return_t memcached_server_add_udp (memcached_st *ptr, const char *hostname, in_port_t port);
 
   memcached_return_t memcached_server_add_unix_socket (memcached_st *ptr, const char *socket);
 
   memcached_return_t memcached_server_push (memcached_st *ptr, const memcached_server_st *list); 

   memcached_server_instance_st memcached_server_by_key (const memcached_st *ptr, const char *key, size_t key_length, memcached_return_t *error);
 
   memcached_server_instance_st memcached_server_get_last_disconnect (const memcached_st *ptr)
 
   memcached_return_t memcached_server_cursor(const memcached_st *ptr, const memcached_server_fn *callback, void *context, uint32_t number_of_callbacks);



-----------
DESCRIPTION
-----------


libmemcached(3) performs operations on a list of hosts. The order of these
hosts determine routing to keys. Functions are provided to add keys to
memcached_st structures. To manipulate lists of servers see
memcached_server_st(3).

memcached_server_count() provides you a count of the current number of
servers being used by a \ ``memcached_st``\  structure.

memcached_server_add() pushes a single TCP server into the \ ``memcached_st``\ 
structure. This server will be placed at the end. Duplicate servers are
allowed, so duplication is not checked. Executing this function with the
\ ``MEMCACHED_BEHAVIOR_USE_UDP``\  behavior set will result in a
\ ``MEMCACHED_INVALID_HOST_PROTOCOL``\ .

memcached_server_add_udp() pushes a single UDP server into the \ ``memcached_st``\ 
structure. This server will be placed at the end. Duplicate servers are
allowed, so duplication is not checked. Executing this function with out
setting the \ ``MEMCACHED_BEHAVIOR_USE_UDP``\  behavior will result in a
\ ``MEMCACHED_INVALID_HOST_PROTOCOL``\ .

memcached_server_add_unix_socket() pushes a single UNIX socket into the
\ ``memcached_st``\  structure. This UNIX socket will be placed at the end.
Duplicate servers are allowed, so duplication is not checked. The length
of the filename must be one character less then MEMCACHED_MAX_HOST_LENGTH.

memcached_server_push() pushes an array of \ ``memcached_server_st``\  into
the \ ``memcached_st``\  structure. These servers will be placed at the
end. Duplicate servers are allowed, so duplication is not checked. A
copy is made of structure so the list provided (and any operations on
the list) are not saved.

memcached_server_by_key() allows you to provide a key and retrieve the
server which would be used for assignment. This structure is cloned
from its original structure and must be freed. If NULL is returned you
should consult \*error. The returning structure should be freed with
memcached_server_free().

memcached_server_get_last_disconnect() returns a pointer to the last server
for which there was a connection problem. It does not mean this particular
server is currently dead but if the library is reporting a server is,
the returned server is a very good candidate.

memcached_server_cursor() takes a memcached_st and loops through the 
list of hosts currently in the cursor calling the list of callback 
functions provided. You can optionally pass in a value via 
context which will be provided to each callback function. An error
return from any callback will terminate the loop. memcached_server_cursor()
is passed the original caller memcached_st in its current state.


******
RETURN
******


Varies, see particular functions.


****
HOME
****


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
