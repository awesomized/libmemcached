Managing the servers used by memcached_st
=========================================

SYNOPSIS 
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: uint32_t memcached_server_count (memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: number of configured servers

.. function:: memcached_return_t memcached_server_add (memcached_st *ptr, const char *hostname, in_port_t port)

    :param ptr: pointer to initialized `memcached_st` struct
    :param hostname: hostname or IP address of the TCP server to add
    :param port: port of the TCP server
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_server_add_udp (memcached_st *ptr, const char *hostname, in_port_t port)

    :param ptr: pointer to initialized `memcached_st` struct
    :param hostname: hostname or IP address of the UDP server to add
    :param port: port of the UDP server
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_server_add_unix_socket (memcached_st *ptr, const char *socket)

    :param ptr: pointer to initialized `memcached_st` struct
    :param socket: path to the UNIX socket of the server to add
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_server_push (memcached_st *ptr, const memcached_server_st *list)

    :param ptr: pointer to initialized `memcached_st` struct
    :param list: pre-configured list of servers to push
    :returns: `memcached_return_t` indicating success

.. function:: const memcached_instance_st * memcached_server_by_key (memcached_st *ptr, const char *key, size_t key_length, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param key: key to hash and lookup a server
    :param key_length: length of `key` without any terminating zero
    :param error: pointer to `memcached_return_t` indicating success
    :returns: the server instance to be used for storing/retrieving `key`

.. function:: const memcached_instance_st * memcached_server_get_last_disconnect (const memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: the instance of the last server for which there was a connection problem

.. function:: memcached_return_t memcached_server_cursor(const memcached_st *ptr, const memcached_server_fn *callback, void *context, uint32_t number_of_callbacks)

    :param ptr: pointer to initialized `memcached_st` struct
    :param callback: list of `memcached_server_fn` to be called for each server instance
    :param context: pointer to user supplied context for the callback
    :param number_of_callbacks: number of callbacks supplied
    :returns: `memcached_return_t` indicating success

.. type:: memcached_return_t (*memcached_server_fn)(const memcached_st *ptr, const memcached_instance_st * server, void *context)

    :param ptr: pointer to the `memcached_st` struct
    :param server: pointer to `memcached_instance_st`
    :param context: pointer to user supplied context
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`libmemcached` performs operations on a list of hosts. The order of
these hosts determine routing to keys. Functions are provided to add keys to
`memcached_st` structures. To manipulate lists of servers see
`memcached_server_st`.

:func:`memcached_server_count` provides you a count of the current number of
servers being used by a :type:`memcached_st` structure.

:func:`memcached_server_add` pushes a single TCP server into the
:type:`memcached_st` structure. This server will be placed at the end.
Duplicate servers are allowed, so duplication is not checked. Executing this
function with the `MEMCACHED_BEHAVIOR_USE_UDP` behavior set will result in
a `MEMCACHED_INVALID_HOST_PROTOCOL`.

:func:`memcached_server_add_udp` pushes a single UDP server into the
:type:`memcached_st` structure. This server will be placed at the end. Duplicate
servers are allowed, so duplication is not checked. Executing this function
without setting the `MEMCACHED_BEHAVIOR_USE_UDP` behavior will result in a
`MEMCACHED_INVALID_HOST_PROTOCOL`.

:func:`memcached_server_add_unix_socket` pushes a single UNIX socket into the
:type:`memcached_st` structure. This UNIX socket will be placed at the end.
Duplicate servers are allowed, so duplication is not checked. The length
of the filename must be one character less than `MEMCACHED_MAX_HOST_LENGTH`.

:func:`memcached_server_push` pushes an array of :type:`memcached_server_st`
into the :type:`memcached_st` structure. These servers will be placed at
the end. Duplicate servers are allowed, so duplication is not checked. A
copy is made of structure so the list provided (and any operations on
the list) are not saved.

:func:`memcached_server_by_key` allows you to provide a key and retrieve the
server which would be used for assignment.

:func:`memcached_server_get_last_disconnect` returns a pointer to the last
server for which there was a connection problem. It does not mean this 
particular server is currently dead but if the library is reporting a server 
is, the returned server is a very good candidate.

:func:`memcached_server_cursor` takes a memcached_st and loops through the
list of hosts currently in the cursor calling the list of callback 
functions provided. You can optionally pass in a value via 
context which will be provided to each callback function. An error
return from any callback will terminate the loop.
:func:`memcached_server_cursor` is passed the original caller
:type:`memcached_st` in its current state.

RETURN VALUE
------------

Varies, see particular functions.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_server_st(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_server_st`
    * :doc:`memcached_strerror`
