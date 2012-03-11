================================
Error Codes (memcached_return_t)
================================

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. c:type:: memcached_return_t

.. c:function:: const char *libmemcached_strerror(memcached_return_t rc)

.. c:function:: bool libmemcached_success(memcached_return_t rc)

.. c:function:: bool libmemcached_failure(memcached_return_t rc)

.. c:function:: bool libmemcache_continue(memcached_return_t rc)

.. c:function:: bool memcached_success(memcached_return_t)

.. c:function:: bool memcached_failure(memcached_return_t)


:c:func:`memcached_success` return true if :c:type:`MEMCACHED_SUCCESS` tested true.

:c:func:`memcached_failure` return true if any value other then :c:type:`MEMCACHED_SUCCESS` was provided.


Libmemcached return types:
++++++++++++++++++++++++++


:c:type:`MEMCACHED_SUCCESS`

:c:type:`MEMCACHED_FAILURE`

:c:type:`MEMCACHED_HOST_LOOKUP_FAILURE`

:c:type:`MEMCACHED_CONNECTION_FAILURE`

:c:type:`MEMCACHED_CONNECTION_BIND_FAILURE`

:c:type:`MEMCACHED_WRITE_FAILURE`

:c:type:`MEMCACHED_READ_FAILURE`

:c:type:`MEMCACHED_UNKNOWN_READ_FAILURE`

:c:type:`MEMCACHED_PROTOCOL_ERROR`

:c:type:`MEMCACHED_CLIENT_ERROR`

:c:type:`MEMCACHED_SERVER_ERROR`

:c:type:`MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE`

:c:type:`MEMCACHED_DATA_EXISTS`

:c:type:`MEMCACHED_DATA_DOES_NOT_EXIST`

:c:type:`MEMCACHED_NOTSTORED`

:c:type:`MEMCACHED_STORED`

:c:type:`MEMCACHED_NOTFOUND`

:c:type:`MEMCACHED_MEMORY_ALLOCATION_FAILURE`

:c:type:`MEMCACHED_PARTIAL_READ`

:c:type:`MEMCACHED_SOME_ERRORS`

:c:type:`MEMCACHED_NO_SERVERS`

:c:type:`MEMCACHED_END`

:c:type:`MEMCACHED_DELETED`

:c:type:`MEMCACHED_VALUE`

:c:type:`MEMCACHED_STAT`

:c:type:`MEMCACHED_ITEM`

:c:type:`MEMCACHED_ERRNO`

:c:type:`MEMCACHED_FAIL_UNIX_SOCKET`

:c:type:`MEMCACHED_NOT_SUPPORTED`

:c:type:`MEMCACHED_NO_KEY_PROVIDED`

:c:type:`MEMCACHED_FETCH_NOTFINISHED`

:c:type:`MEMCACHED_TIMEOUT`

:c:type:`MEMCACHED_BUFFERED`

:c:type:`MEMCACHED_BAD_KEY_PROVIDED`

:c:type:`MEMCACHED_INVALID_HOST_PROTOCOL`

:c:type:`MEMCACHED_SERVER_MARKED_DEAD`

:c:type:`MEMCACHED_UNKNOWN_STAT_KEY`

:c:type:`MEMCACHED_E2BIG`

:c:type:`MEMCACHED_INVALID_ARGUMENTS`

:c:type:`MEMCACHED_KEY_TOO_BIG`

:c:type:`MEMCACHED_AUTH_PROBLEM`

:c:type:`MEMCACHED_AUTH_FAILURE`

:c:type:`MEMCACHED_AUTH_CONTINUE`

:c:type:`MEMCACHED_PARSE_ERROR`

:c:type:`MEMCACHED_PARSE_USER_ERROR`

:c:type:`MEMCACHED_DEPRECATED`
   
--------
SEE ALSO
--------

:manpage:`memcached` :manpage:`libmemcached` :manpage:`memcached_client_error` or :manpage:`memcached_worker_error`

