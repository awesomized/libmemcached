Error Codes - memcached_return_t
================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: bool memcached_success(memcached_return_t rc)

.. function:: bool memcached_continue(memcached_return_t rc)

.. function:: bool memcached_failed(memcached_return_t rc)

.. function:: bool memcached_fatal(memcached_return_t rc)

.. c:type:: enum memcached_return_t memcached_return_t

.. enum:: memcached_return_t

    .. enumerator:: MEMCACHED_AUTH_CONTINUE

        Authentication has been paused.

    .. enumerator:: MEMCACHED_AUTH_FAILURE

        The credentials provided are not valid for this server.

    .. enumerator:: MEMCACHED_AUTH_PROBLEM

        An unknown issue has occurred during authentication.

    .. enumerator:: MEMCACHED_BAD_KEY_PROVIDED

        The key provided is not a valid key.

    .. enumerator:: MEMCACHED_BUFFERED

        The request has been buffered.

    .. enumerator:: MEMCACHED_CLIENT_ERROR

        An unknown client error has occurred internally.

    .. enumerator:: MEMCACHED_CONNECTION_BIND_FAILURE

        .. deprecated:: <0.30

        We were not able to bind() to the socket.

    .. enumerator:: MEMCACHED_CONNECTION_FAILURE

        A unknown error has occurred while trying to connect to a server.

    .. enumerator:: MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE

        .. deprecated:: <0.30

        An error has occurred while trying to connect to a server.
        It is likely that either the number of file descriptors need to be increased or you are out of memory.

    .. enumerator:: MEMCACHED_DATA_DOES_NOT_EXIST

        The data requested with the key given was not found.

    .. enumerator:: MEMCACHED_DATA_EXISTS

        The data requested with the key given was not found.

    .. enumerator:: MEMCACHED_DELETED

        The object requested by the key has been deleted.

    .. enumerator:: MEMCACHED_DEPRECATED

        The method that was requested has been deprecated.

    .. enumerator:: MEMCACHED_E2BIG

        Item is too large for the server to store.

    .. enumerator:: MEMCACHED_END

        The server has completed returning all of the objects requested.

    .. enumerator:: MEMCACHED_ERRNO

        An error has occurred in the driver which has set errno.

    .. enumerator:: MEMCACHED_FAIL_UNIX_SOCKET

        A connection was not established with the server via a unix domain socket.

    .. enumerator:: MEMCACHED_FAILURE

        .. deprecated:: <0.30

        A unknown failure has occurred in the server.

    .. enumerator:: MEMCACHED_FETCH_NOTFINISHED

        A request has been made, but the server has not finished the fetch of the last request.

    .. enumerator:: MEMCACHED_HOST_LOOKUP_FAILURE

        A DNS failure has occurred.

    .. enumerator:: MEMCACHED_INVALID_ARGUMENTS

        The arguments supplied to the given function were not valid.

    .. enumerator:: MEMCACHED_INVALID_HOST_PROTOCOL

        The server you are connecting too has an invalid protocol.
        Most likely you are connecting to an older server that does not speak the binary protocol.

    .. enumerator:: MEMCACHED_ITEM

        An item has been fetched (this is an internal error only).

    .. enumerator:: MEMCACHED_KEY_TOO_BIG

        The key that has been provided is too large for the given server.

    .. enumerator:: MEMCACHED_MAXIMUM_RETURN

        This in an internal only state.

    .. enumerator:: MEMCACHED_MEMORY_ALLOCATION_FAILURE

        An error has occurred while trying to allocate memory.

    .. enumerator:: MEMCACHED_NO_KEY_PROVIDED

        .. deprecated:: <0.30
            Use `MEMCACHED_BAD_KEY_PROVIDED` instead.

        No key was provided.

    .. enumerator:: MEMCACHED_NO_SERVERS

        No servers have been added to the memcached_st object.

    .. enumerator:: MEMCACHED_NOTFOUND

        The object requested was not found.

    .. enumerator:: MEMCACHED_NOTSTORED

        The request to store an object failed.

    .. enumerator:: MEMCACHED_NOT_SUPPORTED

        The given method is not supported in the server.

    .. enumerator:: MEMCACHED_PARSE_ERROR

        An error has occurred while trying to parse the configuration string.
        You should use memparse to determine what the error was.

    .. enumerator:: MEMCACHED_PARSE_USER_ERROR

        An error has occurred  in parsing the configuration string.

    .. enumerator:: MEMCACHED_PARTIAL_READ

        The read was only partially successful.

    .. enumerator:: MEMCACHED_PROTOCOL_ERROR

        An unknown error has occurred in the protocol.

    .. enumerator:: MEMCACHED_READ_FAILURE

        A read failure has occurred.

    .. enumerator:: MEMCACHED_SERVER_ERROR

        An unknown error has occurred in the server.

    .. enumerator:: MEMCACHED_SERVER_MARKED_DEAD

        The requested server has been marked dead.

    .. enumerator:: MEMCACHED_SOME_ERRORS

        A multi request has been made, and some undetermined number of errors have occurred.

    .. enumerator:: MEMCACHED_STAT

        A "stat" command has been returned in the protocol.

    .. enumerator:: MEMCACHED_STORED

        The requested object has been successfully stored on the server.

    .. enumerator:: MEMCACHED_SUCCESS

        The request was successfully executed.

    .. enumerator:: MEMCACHED_TIMEOUT

        Operation has timed out.

    .. enumerator:: MEMCACHED_UNKNOWN_READ_FAILURE

        An unknown read failure only occurs when either there is a bug in the server,
        or in rare cases where an ethernet nic is reporting dubious information.

    .. enumerator:: MEMCACHED_UNKNOWN_STAT_KEY

        The server you are communicating with has a stat key which has not be defined in the protocol.

    .. enumerator:: MEMCACHED_VALUE

        A value has been returned from the server (this is an internal condition only).

    .. enumerator:: MEMCACHED_WRITE_FAILURE

        An error has occurred while trying to write to a server.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`

