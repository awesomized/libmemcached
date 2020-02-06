=================
Retrieving errors
=================


--------
SYNOPSIS
--------


#include <libmemcached/memcached.h>
 
.. c:function::  const char *memcached_last_error_message(memcached_st *)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

:c:func:`memcached_last_error_message` is used to return the last error
message that the server responded too. If this error came from a specific
server, its hostname and port will be provided in the error message.

------
RETURN
------

memcached_last_error_message returns a const char* which does not need to be
de-allocated. If no error has occurred then it will return NULL.

----
HOME
----

To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------

:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`


