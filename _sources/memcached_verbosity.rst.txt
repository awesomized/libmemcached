=================================
Setting the verbosity of a server
=================================

.. index:: object: memcached_st

Modifiy verbosity of servers

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:function:: memcached_return_t memcached_verbosity (memcached_st *ptr, uint32_t verbosity)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_verbosity` modifies the "verbosity" of the
memcached(1) servers referenced in the :c:type:`memcached_st`  parameter.


------
RETURN
------


A value of type :c:type:`memcached_return_t` is returned.

On success that value will be :c:type:`MEMCACHED_SUCCESS`.

Use :c:func:`memcached_strerror` to translate this value to a printable string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

