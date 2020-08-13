Setting the verbosity of a server
=================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: memcached_return_t memcached_verbosity (memcached_st *ptr, uint32_t verbosity)

    :param ptr: pointer to initialized `memcached_st` struct
    :param verbosity: level of verbosity
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

:func:`memcached_verbosity` modifies the "verbosity" of the
:manpage:`memcached(1)` servers referenced in the :type:`memcached_st` parameter.

RETURN VALUE
------------

A value of type :type:`memcached_return_t` is returned.

On success that value will be `MEMCACHED_SUCCESS`.

Use :func:`memcached_strerror` to translate this value to a printable string.

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
