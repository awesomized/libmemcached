=================
Anaylzing servers
=================


Analyze server information


-------
LIBRARY
-------


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   memcached_analysis_st *
     memcached_analyze (memcached_st *ptr,
                        memcached_stat_st *stat,
                        memcached_return_t *error);



-----------
DESCRIPTION
-----------


libmemcached(3) has the ability to query a memcached server (or collection
of servers) for their current state. Queries to find state return a
\ ``memcached_analysis_st``\  structure. You are responsible for freeing this structure.

memcached_analyze() analyzes useful information based on the provided servers
and sets the result to the \ ``memcached_analysis_st``\  structure. The return value
must be freed by the calling application.

A command line tool, memstat(1) with the option --analyze, is provided so that
you do not have to write an application to use this method.


------
RETURN
------


A pointer to the allocated \ ``memcached_analysis_st``\  structure on success and
a NULL pointer on failure. You may inspect the error detail by checking the
\ ``memcached_return_t``\  value.

Any method returning a \ ``memcached_analysis_st``\  expects you to free the
memory allocated for it.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`

