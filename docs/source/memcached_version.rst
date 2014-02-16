=======================================================
Getting version information about the client and server
=======================================================

 
--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:function:: const char * memcached_lib_version (void) 

.. c:function:: memcached_return_t memcached_version (memcached_st *ptr)


Compile and link with -lmemcached



-----------
DESCRIPTION
-----------


:c:func:`memcached_lib_version` is used to return a simple version string representing the libmemcached version (client library version, not server version)

:c:func:`memcached_version` is used to set the major, minor, and micro versions of each memcached server being used by the memcached_st connection structure. It returns the memcached server return code.


------
RETURN
------


:c:func:`memcached_lib_version` returns a string with the version of the libmemcached driver.

A value of :c:type:`memcached_return_t` is returned from :c:func:'memcached_version'

On success that value will be :c:type:`MEMCACHED_SUCCESS`. 

If called with the :c:func:`MEMCACHED_BEHAVIOR_USE_UDP` behavior set, the value :c:type:`MEMCACHED_NOT_SUPPORTED` will be returned. 

Use :c:func:`memcached_strerror` to translate this value to 
a printable string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

