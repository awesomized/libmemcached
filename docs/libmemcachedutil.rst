===========
Introducing
===========


Utility library for libmemcached


--------
SYNOPSIS
--------

.. code-block:: c

   cc [ flag ... ] file ... -lmemcachedutil
 
   #include <libmemcached/memcached_util.h>



-----------
DESCRIPTION
-----------


:program:`libmemcachedutil`  is a small and thread-safe client library that 
provides extra functionality built on top of :program:`libmemcached`.


-------
THREADS
-------


Do not try to access an instance of :c:type:`memcached_st` from multiple threads
at the same time. If you want to access memcached from multiple threads
you should either clone the :c:type:`memcached_st`, or use the memcached pool
implementation. see :c:func:`memcached_pool_create`.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`libmemcached(3)` :manpage:`memcached_pool_create(3)` :manpage:`memcached_pool_destroy(3)` :manpage:`memcached_pool_pop(3)` :manpage:`memcached_pool_push(3)`

