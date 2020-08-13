Managing lists of servers
=========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. c:type:: struct memcached_instance_st memcached_instance_st

.. c:type:: struct memcached_server_st memcached_server_st

.. c:type:: struct memcached_server_st *memcached_server_list_st

.. function:: void memcached_server_list_free (memcached_server_list_st list)

    :param list: instance of initialized `memcached_server_list_st` object

.. function:: memcached_server_list_st memcached_server_list_append (memcached_server_list_st list, const char *hostname, in_port_t port, memcached_return_t *error)

    :param list: instance of an existing `memcached_server_list_st` or nullptr to create one
    :param hostname: the hostname or path to the socket, defaults to localhost if null
    :param port: the port to use, defaults to 11211 if 0
    :param error: pointer to store any `memcached_return_t` error indicating success
    :returns: new instance of `memcached_server_list_st` on success or nullptr on failure

.. function:: uint32_t memcached_server_list_count (memcached_server_list_st list)

    :param list: instance of `memcached_server_list_st`
    :returns: count of servers in the list

.. function:: const char *memcached_server_error (const memcached_instance_st * instance)

    :param instance: pointer to an initialized `memcached_instance_st` object
    :returns: last error message sent from the server to the client

.. function:: void memcached_server_error_reset (const memcached_instance_st * list)

    .. deprecated:: 0.39
   
.. function:: void memcached_servers_parse ()

    .. deprecated:: 0.39

    See `memcached`.

DESCRIPTION
-----------

`libmemcached` operates on a list of hosts which are stored in
:type:`memcached_server_st` structures. You should not modify these structures
directly. Functions are provided to modify these structures.

:func:`memcached_server_list` is used to provide an array of all defined hosts.
This was incorrectly documented as "requiring free" up till version 0.39.

:func:`memcached_server_list_free` deallocates all memory associated with the
array of :type:`memcached_server_st` that you passed to it.

:func:`memcached_server_list_append` adds a server to the end of a
:type:`memcached_server_st` array. On error null will be returned and the
:type:`memcached_return_t` pointer you passed into the function will be set with the appropriate error. If the value of port is zero, it is set to the default
port of a memcached server.

DEPRECATED :func:`memcached_servers_parse`, please see :func:`memcached`

:func:`memcached_server_error` can be used to look at the text of the last
error message sent by the server to to the client.

Before version 0.39 theses functions used a memcached_server_st \*. In 0.39
memcached_server_st \* was aliased to :type:`memcached_server_list_st`.
This was done for a style reason to help clean up some concepts in the code.

RETURN VALUE
------------

Varies, see particular functions.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_servers(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_servers`
    * :doc:`memcached_strerror`
