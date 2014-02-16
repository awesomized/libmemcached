================================================
Coverting Errors, memcached_return_t, to strings
================================================


.. index:: object: memcached_st

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:function:: const char * memcached_strerror (memcached_st *ptr, memcached_return_t rc)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


:c:func:`memcached_strerror` takes a :c:type:`memcached_return_t` value and returns a string describing the error.

This string must not be modified by the application.

:c:type:`memcached_return_t` values are returned from nearly all libmemcached(3) functions.

:c:type:`memcached_return_t` values are of an enum type so that you can set up responses with switch/case and know that you are capturing all possible return values.


------
RETURN
------


:c:func:`memcached_strerror` returns a string describing a :c:type:`memcached_return_t` value.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_



--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

