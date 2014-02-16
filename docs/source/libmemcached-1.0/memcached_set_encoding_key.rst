==================
Set encryption key
==================

.. index:: object: memcached_st

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
 
.. c:function:: void memcached_set_encoding_key (memcached_st *ptr, const char *string, const size_t string_length)

Compile and link with -lmemcached

-----------
DESCRIPTION
-----------


:c:func:`memcached_set_encoding_key` sets the key that will be used to encrypt and decrypt data as it is sent and recieved from the server.

Currently only AES is is supported.


------
RETURN
------


A value of type :c:type:`memcached_return_t` is returned On success that value
will be :c:type:`MEMCACHED_SUCCESS`.  Use :c:func:`memcached_strerror` to
translate this value to a printable string.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
