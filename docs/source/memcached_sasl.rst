============
SASL support
============

.. index:: object: memcached_st


--------
SYNOPSIS
--------

#include <libmemcached/memcached_pool.h>

.. c:function:: void memcached_set_sasl_callbacks(memcached_st *ptr, const sasl_callback_t *callbacks)

.. c:function:: const sasl_callback_t *memcached_get_sasl_callbacks(memcached_st *ptr)

.. c:function:: memcached_return_t memcached_set_sasl_auth_data(memcached_st *ptr, const char *username, const char *password)

.. c:function:: memcached_return_t memcached_destroy_sasl_auth_data(memcached_st *ptr)

Compile and link with -lmemcached



-----------
DESCRIPTION
-----------


libmemcached(3) allows you to plug in your own callbacks function used by
libsasl to perform SASL authentication.

Please note that SASL requires the memcached binary protocol, and you have
to specify the callbacks before you connect to the server.

:c:func:`memcached_set_sasl_auth_data` is a helper function defining
the basic functionality for you, but it will store the username and password
in memory. If you choose to use this method you have to call
:c:type:`memcached_destroy_sasl_auth_data` before calling 
:c:type:`memcached_free` to avoid a memory leak. You should NOT call 
:c:type:`memcached_destroy_sasl_auth_data` if you specify your own callback 
function with :c:func:`memcached_set_sasl_callbacks`.


------
RETURN
------


:c:func:`memcached_get_sasl_callbacks` returns the callbacks currently used by
this memcached handle. :c:func:`memcached_set_sasl_auth_data` returns
:c:type:`MEMCACHED_SUCCESS` upon success.


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
