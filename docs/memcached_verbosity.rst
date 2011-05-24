=================================
Setting the verbosity of a server
=================================

.. index:: object: memcached_st

Modifiy verbosity of servers

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:function:: memcached_return_t memcached_verbosity (memcached_st *ptr, unsigned int verbosity);

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


memcached_verbosity() modifies the "verbosity" of the
memcached(1) servers referenced in the \ ``memcached_st``\  parameter.


------
RETURN
------


A value of type \ ``memcached_return_t``\  is returned
On success that value will be \ ``MEMCACHED_SUCCESS``\ .
Use memcached_strerror() to translate this value to a printable string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


memcached(1) libmemcached(3) memcached_strerror(3)

