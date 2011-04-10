========================
Configuring Libmemcached
========================

-------- 
SYNOPSIS 
--------


.. c:function:: memcached_st *memcached_create_with_options(const char *string, size_t string_length)


.. c:function:: memcached_return_t libmemcached_check_configuration(const char *option_string, size_t length, char *error_buffer, size_t error_buffer_size)

Compile and link with -lmemcached



-----------
DESCRIPTION
-----------

Libmemcached implements a custom language for configuring and modifying
servers. By passing in an option string you can generate a memcached_st object
that you can use in your application directly.


.. describe:: --SERVER=<servername>:<optional_port>/?<optional_weight>

Provide a servername to be used by the client. Providing a weight will cause weighting to occur with all hosts with each server getting a default weight of 1.

.. describe:: --VERIFY-KEY

Verify that keys that are being used fit within the design of the protocol being used.

.. describe:: --AUTO-EJECT_HOSTS

Enable the behavior MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS.

.. describe:: --BINARY-PROTOCOL

Force all connections to use the binary protocol.

.. describe:: --BUFFER-REQUESTS

Enable MEMCACHED_BEHAVIOR_BUFFER_REQUESTS.

.. describe:: --CONFIGURE-FILE=

Provide a configuration file to be used to load requests. Beware that by using a configuration file libmemcached will reset memcached_st based on information only contained in the file.

.. describe:: --CONNECT-TIMEOUT=

Enable MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT. 

.. describe:: --DISTRIBUTION=

Set the distribution model used by the client.  See :manpage:`` for more details.

.. describe:: --HASH=

Set the hashing alogrthm used for placing keys on servers.

.. describe:: --HASH-WITH-PREFIX_KEY

.. describe:: --IO-BYTES-WATERMARK=

.. describe:: --IO-KEY-PREFETCH=

.. describe:: --IO-MSG-WATERMARK=

.. describe:: --NOREPLY

.. describe:: --NUMBER-OF-REPLICAS=

Set the nummber of servers that keys will be replicated to.

.. describe:: --POLL-TIMEOUT=

.. describe:: --RANDOMIZE-REPLICA-READ

Select randomly the server within the replication pool to read from.

.. describe:: --RCV-TIMEOUT=

.. describe:: --RETRY-TIMEOUT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_RETRY_TIMEOUT

.. describe:: --SERVER-FAILURE-LIMIT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT

.. describe:: --SND-TIMEOUT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SND_TIMEOUT

.. describe:: --SOCKET-RECV-SIZE=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE

.. describe:: --SOCKET-SEND-SIZE=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE

.. describe:: --SORT-HOSTS

When adding new servers always calculate their distribution based on sorted naming order.

.. describe:: --SUPPORT-CAS

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SUPPORT_CAS

.. describe:: --TCP-NODELAY

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_TCP_NODELAY

.. describe:: --TCP-KEEPALIVE

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_TCP_KEEPALIVE

.. describe:: --TCP-KEEPIDLE

.. describe:: --USE-UDP

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_USE_UDP

.. describe:: --PREFIX-KEY=

.. describe:: INCLUDE

Include a file in configuration. Unlike --CONFIGURE-FILE= this will not reset memcached_st

.. describe:: RESET

Reset memcached_st and continue to process.

.. describe:: END

End configutation processing.

.. describe:: ERROR

End configutation processing and throw an error.


------
RETURN
------


memcached_create_with_options() returns a pointer to the memcached_st that was
created (or initialized).  On an allocation failure, it returns NULL.


----
HOME
----


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_



--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
