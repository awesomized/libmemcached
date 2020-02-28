Creating and destroying a memcached_st
======================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. type:: memcached_st

.. function:: memcached_st* memcached_create(memcached_st *ptr)

.. function:: void memcached_free(memcached_st *ptr)

.. function:: memcached_st* memcached_clone(memcached_st *destination, memcached_st *source)

.. function:: void memcached_servers_reset(memcached_st)

Compile and link with -lmemcached

DESCRIPTION
-----------

:func:`memcached_create` is used to create a :type:`memcached_st`
structure that will then be used by other libmemcached(3) functions to 
communicate with the server. You should either pass a statically declared 
:type:`memcached_st` to :func:`memcached_create` or
a NULL. If a NULL passed in then a structure is allocated for you.

Please note, when you write new application use
:func:`memcached` over :func:`memcached_create`.

:func:`memcached_clone` is similar to :func:`memcached_create` but
it copies the defaults and list of servers from the source 
:type:`memcached_st`. If you pass a null as the argument for the source 
to clone, it is the same as a call to :func:`memcached_create`.
If the destination argument is NULL a :type:`memcached_st` will be allocated 
for you.

:func:`memcached_servers_reset` allows you to zero out the list of
servers that the :type:`memcached_st` has.

To clean up memory associated with a :type:`memcached_st` structure you
should pass it to :func:`memcached_free` when you are finished using it. 
:func:`memcached_free` is the only way to make sure all memory is 
deallocated when you finish using the structure.

You may wish to avoid using memcached_create(3) or memcached_clone(3) with a
stack based allocation. The most common issues related to ABI safety involve
heap allocated structures.

RETURN
------

:func:`memcached_create` returns a pointer to the :type:`memcached_st`
that was created (or initialized). On an allocation failure, it returns NULL.

:func:`memcached_clone` returns a pointer to the :type:`memcached_st`
that was created (or initialized). On an allocation failure, it returns NULL.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
