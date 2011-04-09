============
SASL support
============


-------
LIBRARY
-------


C Client Library for memcached (libmemcached, -lmemcached)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/memcached.h>
 
   void memcached_set_sasl_callbacks(memcached_st *ptr,
                                     const sasl_callback_t *callbacks)
 
   const sasl_callback_t *memcached_get_sasl_callbacks(memcached_st *ptr)
 
   memcached_return memcached_set_sasl_auth_data(memcached_st *ptr,
                                                 const char *username,
                                                 const char *password)
   memcached_return memcached_destroy_sasl_auth_data(memcached_st *ptr)



-----------
DESCRIPTION
-----------


libmemcached(3) allows you to plug in your own callbacks function used by
libsasl to perform SASL authentication.

Please note that SASL requires the memcached binary protocol, and you have
to specify the callbacks before you connect to the server.

memcached_set_sasl_auth_data() is a helper function for you defining
the basic functionality for you, but it will store the username and password
in memory. If you choose to use this method you have to call
memcached_destroy_sasl_auth_data before calling memcached_free to avoid
a memory leak. You should NOT call memcached_destroy_sasl_auth_data if you
specify your own callback function with memcached_set_sasl_callbacks().

You as a client user have to initialize libsasl by using sasl_client_init
before enabling it in libmemcached, and you have to shut down libsasl by
calling sasl_done() when you are done using SASL from libmemcached.


------
RETURN
------


memcached_get_sasl_callbacks() returns the callbacks currently used
by this memcached handle.
memcached_get_sasl_set_auth_data() returns MEMCACHED_SUCCESS upon success.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>

Trond Norbye, <trond.norbye@gmail.com>


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
