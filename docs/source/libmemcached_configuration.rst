========================
Configuring Libmemcached
========================

.. highlightlang:: c

-------- 
SYNOPSIS 
--------

.. describe:: #include <libmemcached-1.0/memcached.h>

.. c:function:: memcached_st *\
                memcached(const char *string, size_t string_length)

.. c:function:: memcached_return_t \
                libmemcached_check_configuration(const char *option_string, size_t length, \
                                                 char *error_buffer, size_t error_buffer_size)

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------

Libmemcached implements a custom language for configuring and modifying
servers. By passing in an option string you can generate a ``memcached_st`` object
that you can use in your application directly.

General Options:
****************

.. option::  --SERVER=<servername>:<optional_port>/?<optional_weight>

Provide a servername to be used by the client.

Providing a weight will cause weighting to occur with all hosts with each server getting a default weight of 1.

.. option:: --SOCKET=\"<filepath>/?<optional_weight>\"

Provide a filepath to a UNIX socket file. Providing a weight will cause weighting to occur with all hosts with each server getting a default weight of 1.

.. option:: --VERIFY-KEY

Verify that keys that are being used fit within the design of the protocol being used.

.. option:: --REMOVE_FAILED_SERVERS

Enable the behavior :c:type:`MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS`.

.. option:: --BINARY-PROTOCOL

Force all connections to use the binary protocol.

.. option:: --BUFFER-REQUESTS

Please see :c:type:`MEMCACHED_BEHAVIOR_BUFFER_REQUESTS`.

.. option:: --CONFIGURE-FILE=

Provide a configuration file to be used to load requests. Beware that by using a configuration file libmemcached will reset memcached_st based on information only contained in the file.

.. option:: --CONNECT-TIMEOUT=

Please see :c:type:`MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT`. 

.. option:: --DISTRIBUTION=

Set the distribution model used by the client.  See :c:func:`memcached_behavior_set` for more details.

.. option:: --HASH=

Set the hashing algorithm used for placing keys on servers.

.. option:: --HASH-WITH-NAMESPACE

When enabled the prefix key will be added to the key when determining which
server to store the data in.

.. option:: --NOREPLY

Enable "no reply" for all calls that support this. It is highly recommended
that you use this option with the binary protocol only.

.. option:: --NUMBER-OF-REPLICAS=

Set the number of servers that keys will be replicated to.

.. option:: --RANDOMIZE-REPLICA-READ

Select randomly the server within the replication pool to read from.

.. option:: --SORT-HOSTS

When adding new servers always calculate their distribution based on sorted naming order.

.. option:: --SUPPORT-CAS

See :manpage:`memcached_behavior_set(3)` for :c:type:`MEMCACHED_BEHAVIOR_SUPPORT_CAS`

.. option:: --USE-UDP

See :manpage:`memcached_behavior_set(3)` for :c:type:`MEMCACHED_BEHAVIOR_USE_UDP`

.. option:: --NAMESPACE=

A namespace is a container that provides context for keys, only other
requests that know the namespace can access these values. This is
accomplished by prepending the namespace value to all keys.


***********************
Memcached Pool Options:
***********************

.. option:: --POOL-MIN

Initial size of pool.

.. option:: --POOL-MAX

Maximize size of the pool.

************
I/O Options:
************

.. option:: --TCP-NODELAY

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_TCP_NODELAY

.. option:: --TCP-KEEPALIVE

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_TCP_KEEPALIVE

.. option:: --RETRY-TIMEOUT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_RETRY_TIMEOUT

.. option:: --SERVER-FAILURE-LIMIT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT

.. option:: --SND-TIMEOUT=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SND_TIMEOUT

.. option:: --SOCKET-RECV-SIZE=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE

.. option:: --SOCKET-SEND-SIZE=

See :manpage:`memcached_behavior_set(3)` for MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE

.. option:: --POLL-TIMEOUT=

That sets the value of the timeout used by :manpage: `poll()`.

.. option:: --IO-BYTES-WATERMARK=

.. option:: --IO-KEY-PREFETCH=

.. option:: --IO-MSG-WATERMARK=

.. option:: --TCP-KEEPIDLE

.. option:: --RCV-TIMEOUT=



Other Options:
**************


.. option:: INCLUDE

Include a file in configuration. Unlike --CONFIGURE-FILE= this will not reset memcached_st

.. option:: RESET

Reset memcached_st and continue to process.

.. option:: END

End configuration processing.

.. option:: ERROR

End configuration processing and throw an error.


-----------
ENVIRONMENT
-----------

.. envvar:: LIBMEMCACHED


------
RETURN
------


:c:func:`memcached()` returns a pointer to the memcached_st that was
created (or initialized).  On an allocation failure, it returns NULL.



------- 
EXAMPLE 
-------


.. code-block:: c

   const char *config_string=
     "--SERVER=host10.example.com "
     "--SERVER=host11.example.com "
     "--SERVER=host10.example.com";
   memcached_st *memc= memcached(config_string, strlen(config_string));
   {
     // ...
   }
   memcached_free(memc);



--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
