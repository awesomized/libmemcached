Working with statistical information from a server
==================================================

Get memcached statistics

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. type:: struct memcached_stat_st memcached_stat_st

.. type:: memcached_return_t (*memcached_stat_fn)(const memcached_instance_st * server, const char *key, size_t key_length, const char *value, size_t value_length, void *context)

    :param server: pointer to the `memcached_instance_st` being stat'ed
    :param key: the current key
    :param key_length: the length of the `key` without any terminating zero
    :param value: the value read
    :param value_length: the length of the value without any terminating zero
    :param context: pointer to the user supplied context
    :returns: `memcached_return_t` indicating success

.. function:: memcached_stat_st *memcached_stat (memcached_st *ptr, char *args, memcached_return_t *error)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param args: particular state object to query
    :param error: pointer to `memcached_return_t` indicating success
    :returns: array of `memcached_stat_st` objects for all available servers

.. function:: memcached_return_t memcached_stat_servername (memcached_stat_st *stat, char *args, const char *hostname, in_port_t port)

    :param stat: pointer to a `memcached_stat_st` struct to fill
    :param args: particular state object to query
    :param hostname: the hostname or IP address of the server to stat
    :param port: the port of the server to stat
    :returns: `memcached_return_t` indicating success

.. function:: char * memcached_stat_get_value (memcached_st *ptr, memcached_stat_st *stat, const char *key, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param stat: pointer to initialized `memcached_stat_st` struct
    :param key: the statistic to query
    :param error: pointer to `memcached_return_t` indicating success
    :returns: string value of the statistic

.. function:: char ** memcached_stat_get_keys (memcached_st *ptr, memcached_stat_st *stat, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param stat: pointer to initialized `memcached_stat_st` struct
    :param error: pointer to `memcached_return_t` indicating success
    :returns: array of default keys probably available in the statistics

.. function:: memcached_return_t memcached_stat_execute (memcached_st *ptr, const char *args, memcached_stat_fn func, void *context)

    :param ptr: pointer to initialized `memcached_st` struct
    :param args: particular state object to query
    :param func: `memcached_stat_fn` callback
    :param context: pointer to user supplied context
    :returns: `memcached_return_t` indication success

DESCRIPTION
-----------

`libmemcached` has the ability to query a :manpage:`memcached(1)` server (or
collection of servers) for their current state. Queries to find state return a
:type:`memcached_stat_st` structure. You are responsible for freeing this
structure. While it is possible to access the structure directly it is not
advisable. :func:`memcached_stat_get_value` has been provided to query the structure.

:func:`memcached_stat_execute` uses the servers found in :type:`memcached_stat_st`
and executes a "stat" command on each server. args is an optional argument that
can be passed in to modify the behavior of "stats". You will need to supply a
callback function that will be supplied each pair of values returned by
the memcached server.

:func:`memcached_stat` fetches an array of :type:`memcached_stat_st` structures
containing the state of all available memcached servers. The return value must
be freed by the calling application. If called with the
`MEMCACHED_BEHAVIOR_USE_UDP` behavior set, a NULL value is returned and the
error parameter is set to `MEMCACHED_NOT_SUPPORTED`.

:func:`memcached_stat_servername` can be used standalone without a
:type:`memcached_st` to obtain the state of a particular server.  "args" is used
to define a particular state object (a list of these are not provided for by either
the :func:`memcached_stat_get_keys` call nor are they defined in the memcached
protocol). You must specify the hostname and port of the server you want to
obtain information on.

:func:`memcached_stat_get_value` returns the value of a particular state key.
You specify the key you wish to obtain.  The key must be null terminated.

:func:`memcached_stat_get_keys` returns a list of keys that the server has state
objects on. You are responsible for freeing this list.

A command line tool, `memstat`, is provided so that you do not have to write
an application to do this.

RETURN VALUE
------------

Varies, see particular functions.

Any method returning a :type:`memcached_stat_st` expects you to free the
memory allocated for it.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
