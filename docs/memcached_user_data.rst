==============================================
Storing custom user information in the client.
==============================================

.. index:: object: memcached_st

Manage user specific data


-------
LIBRARY
-------


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   void *memcached_get_user_data (memcached_st *ptr);
 
   void *memcached_set_user_data (memcached_st *ptr, void *data);



-----------
DESCRIPTION
-----------


libmemcached(3) allows you to store a pointer to a user specific data inside
the memcached_st structure.

:c:func:`memcached_set_user_data()` is used to set the user specific data in the
:c:type:`memcached_st` structure.

:c:func:`memcached_get_user_data()` is used to retrieve the user specific data in the :c:type:`memcached_st` structure.


------
RETURN
------


:c:func:`memcached_set_user_data()` returns the previous value of the user specific data.

:c:func:`memcached_get_user_data()` returns the current value uf the user specific data.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

