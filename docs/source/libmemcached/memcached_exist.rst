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

   .. versionadded:: 0.53

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

:c:func:`memcached_exist()` can be used to check to see if a key exists. No value is returned if the key exists, or does not exist, on the server. 


------
RETURN
------

:c:func:`memcached_exist()` sets error to 
to :c:type:`MEMCACHED_SUCCESS` upon finding that the key exists.
:c:type:`MEMCACHED_NOTFOUND` will be return if the key is not found.


----
HOME
----

To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`


