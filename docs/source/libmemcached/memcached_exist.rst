===========================
Determine if a keys exists.
===========================

.. index:: object: memcached_st

--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function::  memcached_return_t memcached_exist(memcached_st *ptr, char *key, size_t *key_length)

.. c:function::  memcached_return_t memcached_exist_by_key(memcached_st *ptr, char *group_key, size_t *group_key_length, char *key, size_t *key_length)

Compile and link with -lmemcached

.. versionadded:: 0.53


-----------
DESCRIPTION
-----------

:c:func:`memcached_exist()` can be used to check if a key exists.


------
RETURN
------

:c:type:`MEMCACHED_SUCCESS`
  The key exists.

:c:type:`MEMCACHED_NOTFOUND`
  The key was not found.

--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`


