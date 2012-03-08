===========================================
memcached_touch, memcached_touch_by_key
===========================================

.. index:: object: memcached_st

--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function:: memcached_return_t memcached_touch (memcached_st *ptr, const char *key, size_t key_length, time_t expiration)

.. c:function:: memcached_return_t memcached_touch_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, time_t expiration)

Compile and link with -lmemcached

-----------
DESCRIPTION
-----------


:c:func:`memcached_touch` is used to update the expiration time on an existing key.
:c:func:`memcached_touch_by_key` works the same, but it takes a master key 
to find the given value.


------
RETURN
------


A value of type :c:type:`memcached_return_t` is returned
On success that value will be :c:type:`MEMCACHED_SUCCESS`.
Use :c:func:`memcached_strerror` to translate this value to a printable 
string.

----
HOME
----

To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

