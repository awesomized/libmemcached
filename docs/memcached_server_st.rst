=========================
Managing lists of servers
=========================



--------
SYNOPSIS
--------



#include <libmemcached/memcached.h>
 
.. c:function:: const memcached_server_instance_st memcached_server_list (memcached_st *ptr);

.. c:function:: void memcached_server_list_free (memcached_server_list_st list);
 
.. c:function:: memcached_server_list_st memcached_server_list_append (memcached_server_list_st list, const char *hostname, unsigned int port, memcached_return_t *error);
 
.. c:function:: uint32_t memcached_server_list_count (memcached_server_list_st list);
 
.. c:function:: const char *memcached_server_error (memcached_server_instance_st instance);
 
.. c:function:: void memcached_server_error_reset (memcached_server_instance_st list); DEPRECATED 
   

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


libmemcached(3) operates on a list of hosts which are stored in
memcached_server_st structures. You should not modify these structures
directly. Functions are provided to modify these structures (and more can be
added, just ask!).

memcached_server_list() is used to provide an array of all defined hosts.
This was incorrectly documented as "requiring free()" up till version 0.39.

memcached_server_list_free() deallocates all memory associated with the array
of memcached_server_st that you passed to it.

memcached_server_list_append() adds a server to the end of a
memcached_server_st array. On error null will be returned and the
memcached_return_t pointer you passed into the function will be set with the
appropriate error. If the value of port is zero, it is set to the default
port of a memcached server.

DEPRECATED memcached_servers_parse(), please see :manpage:`memcached(3)`

memcached_server_error() can be used to look at the text of the last error 
message sent by the server to to the client.

Before version 0.39 theses functions used a memcache_server_st \*. In 0.39
memcached_server_st \* was aliased to memcached_server_list_st. This was
done for a style reason/to help clean up some concepts in the code.


------
RETURN
------


Varies, see particular functions.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_



--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
