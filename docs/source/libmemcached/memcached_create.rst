Creating and destroying a memcached_st
======================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. type:: struct memcached_st memcached_st

.. function:: memcached_st* memcached_create(memcached_st *ptr)

    :param ptr: pointer to user-allocated `memcached_st` struct or null pointer
    :returns: pointer to initialized `memcached_st` struct

.. function:: void memcached_free(memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct to destroy and possibly free

.. function:: memcached_st* memcached_clone(memcached_st *destination, memcached_st *source)

    :param destination: pointer to user-allocated `memcached_st` struct or null pointer
    :param source: pointer to initialized `memcached_st` struct to copy from
    :returns: pointer to newly initialized `destination`, copied from `source`

.. function:: void memcached_servers_reset(memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct

DESCRIPTION
-----------

`memcached_create` is used to create a `memcached_st` structure that will then
be used by other `libmemcached` functions to communicate with the server. You
should either pass a statically declared `memcached_st` to `memcached_create` or
a NULL. If a NULL passed in then a structure is allocated for you.

Please note, when you write new application use `memcached` over
`memcached_create`.

`memcached_clone` is similar to `memcached_create` but it copies the defaults
and list of servers from the source `memcached_st` pointer. If you pass a null
as the argument for the source to clone, it is the same as a call to
`memcached_create`. If the destination argument is NULL a `memcached_st` will be
allocated for you.

`memcached_servers_reset` allows you to zero out the list of servers that the
`memcached_st` has.

To clean up memory associated with a `memcached_st` structure you should pass it
to `memcached_free` when you are finished using it. `memcached_free` is the only
way to make sure all memory is deallocated when you finish using the structure.

You may wish to avoid using `memcached_create` or `memcached_clone` with a stack
based allocation. The most common issues related to ABI safety involve heap
allocated structures.

RETURN VALUE
------------

`memcached_create` returns a pointer to the `memcached_st` that was created (or
initialized). On an allocation failure, it returns NULL.

`memcached_clone` returns a pointer to the `memcached_st` that was created (or
initialized). On an allocation failure, it returns NULL.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
