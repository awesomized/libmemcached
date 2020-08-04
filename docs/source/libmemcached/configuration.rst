libmemcached Configuration
==========================

SYNOPSIS
--------

#include <libmemcached-|libmemcached_version|/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_st *memcached(const char *string, size_t string_length)

    :param string: configuration string
    :param string_length: length of the configuration string without any terminating zero
    :returns: allocated and initialized `memcached_st` struct

.. function:: memcached_return_t libmemcached_check_configuration(const char *option_string, size_t length, char *error_buffer, size_t error_buffer_size)

    :param option_string: configuration string
    :param length: length of the configuration string without any terminating zero
    :param error_buffer: buffer used to store any error message
    :param error_buffer_size: available size of the `error_buffer`
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`libmemcached` implements a custom language for configuring and modifying
servers. By passing in an option string you can generate a `memcached_st` object
that you can use in your application directly.

General Options:
~~~~~~~~~~~~~~~~

.. describe::  --SERVER=<servername>:<optional_port>/?<optional_weight>

    Provide a servername to be used by the client.

    Providing a weight will cause weighting to occur with all hosts with each
    server getting a default weight of 1.

.. describe:: --SOCKET=\"<filepath>/?<optional_weight>\"

    Provide a filepath to a UNIX socket file. Providing a weight will cause
    weighting to occur with all hosts with each server getting a default weight
    of 1.

.. describe:: --VERIFY-KEY

    Verify that keys that are being used fit within the design of the protocol
    being used.

.. describe:: --REMOVE_FAILED_SERVERS

    Enable the behavior `MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS`.

.. describe:: --BINARY-PROTOCOL

    Force all connections to use the binary protocol.

.. describe:: --BUFFER-REQUESTS

    Please see `MEMCACHED_BEHAVIOR_BUFFER_REQUESTS`.

.. describe:: --CONFIGURE-FILE=

    Provide a configuration file to be used to load requests. Beware that by 
    using a configuration file `libmemcached` will reset `memcached_st` based 
    on information only contained in the file.

.. describe:: --CONNECT-TIMEOUT=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT`. 

.. describe:: --DISTRIBUTION=

    Set the distribution model used by the client.
    See `memcached_behavior_set` for more details.

.. describe:: --HASH=

    Set the hashing algorithm used for placing keys on servers.

.. describe:: --HASH-WITH-NAMESPACE

    When enabled the prefix key will be added to the key when determining which
    server to store the data in.

.. describe:: --NOREPLY

    Enable "no reply" for all calls that support this. It is highly recommended
    that you use this option with the binary protocol only.

.. describe:: --NUMBER-OF-REPLICAS=

    Set the number of servers that keys will be replicated to.

.. describe:: --RANDOMIZE-REPLICA-READ

    Select randomly the server within the replication pool to read from.

.. describe:: --SORT-HOSTS

    When adding new servers always calculate their distribution based on sorted 
    naming order.

.. describe:: --SUPPORT-CAS

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_SUPPORT_CAS`

.. describe:: --USE-UDP

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_USE_UDP`

.. describe:: --NAMESPACE=

    A namespace is a container that provides context for keys, only other
    requests that know the namespace can access these values. This is
    accomplished by prepending the namespace value to all keys.

Memcached Pool Options:
~~~~~~~~~~~~~~~~~~~~~~~

.. describe:: --POOL-MIN

    Initial size of pool.

.. describe:: --POOL-MAX

    Maximize size of the pool.

I/O Options:
~~~~~~~~~~~~

.. describe:: --TCP-NODELAY

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_TCP_NODELAY`

.. describe:: --TCP-KEEPALIVE

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_TCP_KEEPALIVE`

.. describe:: --RETRY-TIMEOUT=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_RETRY_TIMEOUT`

.. describe:: --SERVER-FAILURE-LIMIT=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT`

.. describe:: --SND-TIMEOUT=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_SND_TIMEOUT`

.. describe:: --SOCKET-RECV-SIZE=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE`

.. describe:: --SOCKET-SEND-SIZE=

    See `memcached_behavior_set` for `MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE`

.. describe:: --POLL-TIMEOUT=

    Set the timeout used by :manpage:`poll(3)`.

.. describe:: --IO-BYTES-WATERMARK=

.. describe:: --IO-KEY-PREFETCH=

.. describe:: --IO-MSG-WATERMARK=

.. describe:: --TCP-KEEPIDLE

.. describe:: --RCV-TIMEOUT=

Other Options:
~~~~~~~~~~~~~~

.. describe:: INCLUDE

    Include a file in configuration.
    Unlike ``--CONFIGURE-FILE=`` this will not reset `memcached_st`.

.. describe:: RESET

    Reset `memcached_st` and continue to process.

.. describe:: END

    End configuration processing.

.. describe:: ERROR

    End configuration processing and throw an error.

ENVIRONMENT
-----------

.. envvar:: LIBMEMCACHED

RETURN VALUE
------------

`memcached` returns a pointer to the `memcached_st` that was created (or
initialized).  On an allocation failure, it returns NULL.

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
