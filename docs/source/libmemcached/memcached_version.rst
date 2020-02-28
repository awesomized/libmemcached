=======================================================
Getting version information about the client and server
=======================================================

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: const char * memcached_lib_version (void)

.. function:: memcached_return_t memcached_version (memcached_st *ptr)

Compile and link with -lmemcached

-----------
DESCRIPTION
-----------

:func:`memcached_lib_version` is used to return a simple version string representing the libmemcached version (client library version, not server version)

:func:`memcached_version` is used to set the major, minor, and micro versions of each memcached server being used by the memcached_st connection structure. It returns the memcached server return code.

------
RETURN
------

:func:`memcached_lib_version` returns a string with the version of the libmemcached driver.

A value of :type:`memcached_return_t` is returned from :func:'memcached_version'

On success that value will be `MEMCACHED_SUCCESS`.

If called with the `MEMCACHED_BEHAVIOR_USE_UDP` behavior set, the value
`MEMCACHED_NOT_SUPPORTED` will be returned.

Use :func:`memcached_strerror` to translate this value to
a printable string.

--------
SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

