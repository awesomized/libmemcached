Getting version information
===========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: const char * memcached_lib_version (void)

    :returns: version string of `libmemcached`

.. function:: memcached_return_t memcached_version (memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

:func:`memcached_lib_version` is used to return a simple version string
representing the `libmemcached` version (client library, not server).

:func:`memcached_version` is used to set the major, minor, and micro versions of
each memcached server being used by the `memcached_st` connection structure.
It returns the memcached server return code.

RETURN VALUE
------------

:func:`memcached_lib_version` returns a string with the version of the libmemcached driver.

A value of :type:`memcached_return_t` is returned from :func:'memcached_version'

On success that value will be `MEMCACHED_SUCCESS`.

If called with the `MEMCACHED_BEHAVIOR_USE_UDP` behavior set, the value
`MEMCACHED_NOT_SUPPORTED` will be returned.

Use :func:`memcached_strerror` to translate this value to
a printable string.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

