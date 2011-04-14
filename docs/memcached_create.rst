======================================
Creating and destroying a memcached_st
======================================


Create a memcached_st structure

.. index:: object: memcached_st

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. c:function:: memcached_st *memcached_create (memcached_st *ptr);
 
.. c:function:: void memcached_free (memcached_st *ptr);
 
.. c:function:: memcached_st *memcached_clone (memcached_st *destination, memcached_st *source);
 
.. c:function:: void memcached_servers_reset(memcached_st);

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

memcached_create() is used to create a \ ``memcached_st``\  structure that will then
be used by other libmemcached(3) functions to communicate with the server. You
should either pass a statically declared \ ``memcached_st``\  to memcached_create() or
a NULL. If a NULL passed in then a structure is allocated for you.

Please note, when you write new application use memcached_create_with_options() over memcached_create().

memcached_clone() is similar to memcached_create(3) but it copies the
defaults and list of servers from the source \ ``memcached_st``\ . If you pass a null as
the argument for the source to clone, it is the same as a call to memcached_create().
If the destination argument is NULL a \ ``memcached_st``\  will be allocated for you.

memcached_servers_reset() allows you to zero out the list of servers that
the memcached_st has.

To clean up memory associated with a \ ``memcached_st``\  structure you should pass
it to memcached_free() when you are finished using it. memcached_free() is
the only way to make sure all memory is deallocated when you finish using
the structure.

You may wish to avoid using memcached_create(3) or memcached_clone(3) with a
stack based allocation. The most common issues related to ABI safety involve
heap allocated structures.


------
RETURN
------


memcached_create() returns a pointer to the memcached_st that was created
(or initialized). On an allocation failure, it returns NULL.

memcached_clone() returns a pointer to the memcached_st that was created
(or initialized). On an allocation failure, it returns NULL.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
