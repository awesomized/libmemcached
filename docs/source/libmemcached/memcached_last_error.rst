Retrieving Error Codes and Messages
===================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_last_error(const memcached_st *ptr)

    :param ptr: pointer to an initialized `memcached_st` struct
    :returns: `memcached_return_t` indicating success of last operation

.. function:: const char *memcached_last_error_message(const memcached_st *ptr)

    :param ptr: pointer to an initialized `memcached_st` struct
    :returns: message describing the status of last operation

.. function:: int memcached_last_error_errno(const memcached_st *ptr)

    :param ptr: pointer to an initialized `memcached_st` struct
    :returns: :manpage:`errno(3)` (if any) of last operation

DESCRIPTION
-----------

Retrieve error codes and messages.

RETURN VALUE
------------

`memcached_last_error` returns the last error code.

`memcached_last_error_message` returns the last error message. If this error
came from a specific server, its hostname and port will be provided in the error
message. Any error message will be returned as 'const char \*' which does not
need to be de-allocated. NULL will be returned if no error has occurred.

`memcached_last_error_errno` returns any last local error code obtained from
:manpage:`errno(3)`.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`errno(3)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :manpage:`errno(3)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
