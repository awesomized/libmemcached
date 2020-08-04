Analyzing servers
=================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. type:: struct memcached_analysis_st memcached_analysis_st

.. function:: memcached_analysis_st *memcached_analyze(memcached_st *ptr, memcached_stat_st *stat, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param stat: pointer to a `memcached_stat_st` struct to fill
    :param error: pointer to `memcached_return_t` indicating success
    :returns: pointer to an allocated and filled out `memcached_analysis_t` struct

DESCRIPTION
-----------

`libmemcached` has the ability to query a memcached server (or collection of
servers) for their current state. Queries to find state return a
`memcached_analysis_st` structure. You are responsible for freeing this
structure.

`memcached_analyze` analyzes useful information based on the provided servers
and sets the result to the `memcached_analysis_st` structure. The return value
must be freed by the calling application.

.. seealso:: :option:`memstat --analyze`
    A command line tool to analyze a memcached server.


RETURN VALUE
------------

A pointer to the allocated `memcached_analysis_st` structure on success and a
NULL pointer on failure. You may inspect the error detail by checking the
`memcached_return_t` value.

Any method returning a `memcached_analysis_st` expects you to free the memory
allocated for it.

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
