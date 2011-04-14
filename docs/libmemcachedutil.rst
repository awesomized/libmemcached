===========
Introducing
===========


Utility library for libmemcached


-------
LIBRARY
-------


C Client Library containing utility functions for libmemcached (libmemcachedutil, -lmemcachedutil -lmemcached)


--------
SYNOPSIS
--------


.. code-block:: perl

   cc [ flag ... ] file ... -lmemcachedutil
 
   #include <libmemcached/memcached_util.h>



-----------
DESCRIPTION
-----------


\ **libmemcachedutil**\  is a small and thread-safe client library that provides
extra functionality built on top of \ **libmemcached**\ .


-------
THREADS
-------


Do not try to access an instance of \ ``memcached_st``\  from multiple threads
at the same time. If you want to access memcached from multiple threads
you should either clone the \ ``memcached_st``\ , or use the memcached pool
implementation. see memcached_pool_create(3).


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`libmemcached(3)`
:manpage:`memcached_pool_create(3)` :manpage:`memcached_pool_destroy(3)` :manpage:`memcached_pool_pop(3)` :manpage:`memcached_pool_push(3)`

