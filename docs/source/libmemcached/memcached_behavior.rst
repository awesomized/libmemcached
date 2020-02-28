Behaviors of the library
========================

Manipulate the behavior of a memcached_st structure. 

.. index:: object: memcached_st

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. c:type:: typedef enum memcached_behavior_t memcached_behavior_t;

.. function:: uint64_t memcached_behavior_get (memcached_st *ptr, memcached_behavior_t flag)

.. function:: memcached_return_t memcached_behavior_set (memcached_st *ptr, memcached_behavior_t flag, uint64_t data)

    .. versionchanged:: 0.17
        The `data` argument of :func:`memcached_behavior_set` was changed in
        from taking a pointer to data value, to taking a uin64_t.

.. c:type:: enum memcached_behavior_t memcached_behavior_t

.. enum:: memcached_behavior_t

    .. enumerator:: MEMCACHED_BEHAVIOR_USE_UDP

        Causes `libmemcached` to use the UDP transport when communicating with a
        memcached server. Not all I/O operations are tested when this behavior
        is enabled.

        The following operations will return `MEMCACHED_NOT_SUPPORTED` when
        executed with `MEMCACHED_BEHAVIOR_USE_UDP` enabled:

        * :func:`memcached_version`,
        * :func:`memcached_stat`,
        * :func:`memcached_get`,
        * :func:`memcached_get_by_key`,
        * :func:`memcached_mget`,
        * :func:`memcached_mget_by_key`,
        * :func:`memcached_fetch`,
        * :func:`memcached_fetch_result`,
        * :func:`memcached_fetch_execute`.

        All other operations are tested but are executed in a 'fire-and-forget'
        mode, in which once the client has executed the operation, no attempt
        will be made to ensure the operation has been received and acted on by
        the server.

        `libmemcached` does not allow TCP and UDP servers to be shared within
        the same `libmemcached` client 'instance'. An attempt to add a TCP
        server when this behavior is enabled will result in a
        `MEMCACHED_INVALID_HOST_PROTOCOL`, as will attempting to add a UDP
        server when this behavior has not been enabled.

    .. enumerator:: MEMCACHED_BEHAVIOR_NO_BLOCK

        Causes `libmemcached` to use asynchronous IO. This is the fastest
        transport available for storage functions.

    .. enumerator:: MEMCACHED_BEHAVIOR_SND_TIMEOUT

        This sets the microsecond behavior of the socket against the SO_SNDTIMEO
        flag.

        In cases where you cannot use non-blocking IO this will allow you to
        still have timeouts on the sending of data.

    .. enumerator:: MEMCACHED_BEHAVIOR_RCV_TIMEOUT

        This sets the microsecond behavior of the socket against the SO_RCVTIMEO
        flag.

        In cases where you cannot use non-blocking IO this will allow you to
        still have timeouts on the reading of data.

    .. enumerator:: MEMCACHED_BEHAVIOR_TCP_NODELAY

        Turns on the no-delay feature for connecting sockets (may be faster in
        some environments).

    .. enumerator:: MEMCACHED_BEHAVIOR_HASH

        Set the hash algorithm used for keys.

        The value can be set to either:

        * `MEMCACHED_HASH_DEFAULT`,
        * `MEMCACHED_HASH_MD5`,
        * `MEMCACHED_HASH_CRC`,
        * `MEMCACHED_HASH_FNV1_64`,
        * `MEMCACHED_HASH_FNV1A_64`,
        * `MEMCACHED_HASH_FNV1_32`,
        * `MEMCACHED_HASH_FNV1A_32`,
        * `MEMCACHED_HASH_JENKINS`,
        * `MEMCACHED_HASH_HSIEH`, and
        * `MEMCACHED_HASH_MURMUR`.

        Each hash has its advantages and its weaknesses. If you don't know or
        don't care, just go with the default.

        Support for `MEMCACHED_HASH_HSIEH` is a compile time option that is
        disabled by default. To enable tests for this hashing algorithm,
        configure and build libmemcached with the Hsieh hash enabled.

    .. enumerator:: MEMCACHED_BEHAVIOR_DISTRIBUTION

        Using this you can enable different means of distributing values to
        servers.

        The default method is `MEMCACHED_DISTRIBUTION_MODULA`.

        You can enable consistent hashing by setting
        `MEMCACHED_DISTRIBUTION_CONSISTENT`. Consistent hashing delivers better
        distribution and allows servers to be added to the cluster with minimal
        cache losses.

        Currently `MEMCACHED_DISTRIBUTION_CONSISTENT` is an alias for the
        value type:`MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA`.

    .. enumerator:: MEMCACHED_BEHAVIOR_CACHE_LOOKUPS

        .. deprecated:: 0.46(?)
            DNS lookups are now always cached until an error occurs with the
            server.

       Memcached can cache named lookups so that DNS lookups are made only once.

    .. enumerator:: MEMCACHED_BEHAVIOR_SUPPORT_CAS

        Support CAS operations (this is not enabled by default at this point in
        the server since it imposes a slight performance penalty).

    .. enumerator:: MEMCACHED_BEHAVIOR_KETAMA

        Sets the default distribution to
        `MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA` and the hash to
        `MEMCACHED_HASH_MD5`.

    .. enumerator:: MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED

        Sets the default distribution to
        `MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA` with the weighted tests.
        Makes the default hashing algorithm for keys use MD5.

    .. enumerator:: MEMCACHED_BEHAVIOR_KETAMA_HASH

        Sets the hashing algorithm for host mapping on continuum.

        The value can be set to either:

        * `MEMCACHED_HASH_DEFAULT`,
        * `MEMCACHED_HASH_MD5`,
        * `MEMCACHED_HASH_CRC`,
        * `MEMCACHED_HASH_FNV1_64`,
        * `MEMCACHED_HASH_FNV1A_64`,
        * `MEMCACHED_HASH_FNV1_32`, and
        * `MEMCACHED_HASH_FNV1A_32`.

    .. enumerator:: MEMCACHED_BEHAVIOR_KETAMA_COMPAT

        Sets the compatibility mode. The value can be set to either
        `MEMCACHED_KETAMA_COMPAT_LIBMEMCACHED` (this is the default) or
        `MEMCACHED_KETAMA_COMPAT_SPY` to be compatible with the SPY Memcached
        client for Java.

    .. enumerator:: MEMCACHED_BEHAVIOR_POLL_TIMEOUT

        Modify the timeout in milliseconds value that is used by poll. The
        default value is -1. An signed int must be passed to
        `memcached_behavior_set` to change this value (this requires casting).
        For `memcached_behavior_get` a 'signed int' value will be cast and
        returned as 'unsigned long long'.

    .. enumerator:: MEMCACHED_BEHAVIOR_USER_DATA

        .. deprecated:: < 0.30

    .. enumerator:: MEMCACHED_BEHAVIOR_BUFFER_REQUESTS

        Enabling buffered IO causes commands to "buffer" instead of being sent.
        Any action that gets data causes this buffer to be be sent to the remote
        connection. Quiting the connection or closing down the connection will
        also cause the buffered data to be pushed to the remote connection.

    .. enumerator:: MEMCACHED_BEHAVIOR_VERIFY_KEY

        Enabling this will cause `libmemcached` to test all keys to verify that
        they are valid keys.

    .. enumerator:: MEMCACHED_BEHAVIOR_SORT_HOSTS

        Enabling this will cause hosts that are added to be placed in the host
        list in sorted order. This will defeat consistent hashing.

    .. enumerator:: MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT

        In non-blocking mode this changes the value of the timeout during socket
        connection in milliseconds. Specifying -1 means an infinite time‐out.

    .. enumerator:: MEMCACHED_BEHAVIOR_BINARY_PROTOCOL

        Enable the use of the binary protocol. Please note that you cannot
        toggle this flag on an open connection.

    .. enumerator:: MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK

        Set this value to tune the number of messages that may be sent before
        `libmemcached` should start to automatically drain the input queue.

        Setting this value to high, may cause `libmemcached` to deadlock (trying
        to send data, but the send will block because the input buffer in the
        kernel is full).

    .. enumerator:: MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK

        Set this value to tune the number of bytes that may be sent before
        `libmemcached` should start to automatically drain the input queue (need
        at least 10 IO requests sent without reading the input buffer).

        Setting this value to high, may cause libmemcached to deadlock (trying to
        send data, but the send will block because the input buffer in the kernel
        is full).

    .. enumerator:: MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH

        The binary protocol works a bit different than the textual protocol in
        that a multiget is implemented as a pipe of single get-operations which
        are sent to the server in a chunk.

        If you are using large multigets from your application, you may improve
        the latency of the gets by setting this value so you send out the first
        chunk of requests when you hit the specified limit.  It allows the
        servers to start processing the requests to send the data back while the
        rest of the requests are created and sent to the server.

    .. enumerator:: MEMCACHED_BEHAVIOR_NOREPLY

        Set this value to specify that you really don't care about the result
        from your storage commands (set, add, replace, append, prepend).

    .. enumerator:: MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS

        Specify the numbers of replicas `libmemcached` should store of each item
        (on different servers).

        This replication does not dedicate certain memcached servers to store
        the replicas in, but instead it will store the replicas together with
        all of the other objects (on the 'n' next servers specified in your
        server list).

    .. enumerator:: MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ

        Allows randomizing the replica reads starting point. Normally the read
        is done from primary server and in case of miss the read is done from
        primary + 1, then primary + 2 all the way to 'n' replicas.

        If this option is set on the starting point of the replica reads is
        randomized between the servers.  This allows distributing read load to
        multiple servers with the expense of more write traffic.

    .. enumerator:: MEMCACHED_BEHAVIOR_CORK

        .. deprecated:: ?
            This open has been deprecated with the behavior now built and used
            appropriately on selected platforms.

    .. enumerator:: MEMCACHED_BEHAVIOR_KEEPALIVE

        Enable TCP_KEEPALIVE behavior.

    .. enumerator:: MEMCACHED_BEHAVIOR_KEEPALIVE_IDLE

        Specify time, in seconds, to mark a connection as idle. This is only
        available as an option Linux.

    .. enumerator:: MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE

        Find the current size of SO_SNDBUF. A value of 0 means either an error
        occurred or no hosts were available. It is safe to assume system default
        if this occurs.

        If an error occurs you can check the last cached errno to find the
        specific error.

    .. enumerator:: MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE

        Find the current size of SO_RCVBUF. A value of 0 means either an error
        occurred or no hosts were available. It is safe to assume system default
        if this occurs.

        If an error occurs you can check the last cached errno to find the
        specific error.

    .. enumerator:: MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT

        .. deprecated:: 0.48
            See `MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS`

        Set this value to enable the server be removed after continuous
        `MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT` times connection failure.

    .. enumerator:: MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS

        .. deprecated:: 0.48
            See `MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS`

        If enabled any hosts which have been flagged as disabled will be removed
        from the list of servers in the `memcached_st` structure. This must be
        used in combination with `MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT`.

    .. enumerator:: MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS

       If enabled any hosts which have been flagged as disabled will be removed
       from the list of servers in the `memcached_st` structure.

    .. enumerator:: MEMCACHED_BEHAVIOR_RETRY_TIMEOUT

        When enabled a host which is problematic will only be checked for usage
        based on the amount of time set by this behavior. The value is in
        seconds.

    .. enumerator:: MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY

        When enabled the prefix key will be added to the key when determining
        server by hash. See `MEMCACHED_CALLBACK_NAMESPACE` for additional
        information.

DESCRIPTION
-----------

`libmemcached` behavior can be modified by using :func:`memcached_behavior_set`.
Default behavior is the library strives to be quick and accurate. Some behavior,
while being faster, can also result in not entirely accurate behavior (for
instance, :func:`memcached_set` will always respond with `MEMCACHED_SUCCESS`).

:func:`memcached_behavior_get` takes a behavior flag and returns whether or not
that behavior is currently enabled in the client.

:func:`memcached_behavior_set` changes the value of a particular option of the
client. It takes both a flag (listed below) and a value. For simple on or off
options you just need to pass in a value of 1. Calls to
:func:`memcached_behavior_set` will flush and reset all connections.

RETURN VALUE
------------

:func:`memcached_behavior_get` returns either the current value of the key, or 0
or 1 on simple flag behaviors (1 being enabled). :func:`memcached_behavior_set`
returns failure or success.

NOTES
-----

The `data` argument of :func:`memcached_behavior_set` was changed in version
0.17 from taking a pointer to data value, to taking a uin64_t.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`setsockopt(3)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :manpage:`setsockopt(3)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
