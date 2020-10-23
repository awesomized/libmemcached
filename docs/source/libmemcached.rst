C/C++ Client Library for memcached
==================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

=======

`libmemcached` is an open source C/C++ client library and tools for the
memcached server (http://memcached.org/). It has been designed to be light on
memory usage, thread safe, and provide full access to server side methods.

`libmemcached` was designed to provide the greatest number of options to use
Memcached. Some of the features provided:

1. Asynchronous and Synchronous Transport Support.
2. Consistent Hashing and Distribution.
3. Tunable Hashing algorithm to match keys.
4. Access to large object support.
5. Local replication.
6. A complete reference guide and documentation to the API.
7. Tools to Manage your Memcached networks.

DESCRIPTION
-----------

"Memcached is a high-performance, distributed memory object caching system,
generic in nature, but intended for use in speeding up dynamic web applications
by alleviating database load." `http://memcached.org/ <http://memcached.org/>`_

`libmemcached` is a small, thread-safe client library for the memcached
protocol. The code has all been written to allow for both web and embedded
usage. It handles the work behind routing individual keys to specific servers
specified by the developer (and values are matched based on server order as
supplied by the user). It implements a modular and consistent method of object
distribution.

There are multiple implemented routing and hashing methods. See the
`memcached_behavior_set` manpage for more information.

All operations are performed against a `memcached_st` structure. These
structures can either be dynamically allocated or statically allocated and then
initialized by `memcached_create`. Functions have been written in order to
encapsulate the `memcached_st`. It is not recommended that you operate directly
against the structure.

Nearly all functions return a `memcached_return_t` value. This value can be
translated to a printable string with `memcached_strerror`.

Objects are stored on servers by hashing keys. The hash value maps the key to a
particular server. All clients understand how this hashing works, so it is
possibly to reliably both push data to a server and retrieve data from a server.

Group keys can be optionally used to group sets of objects with servers.

Namespaces are supported, and can be used to partition caches so that multiple
applications can use the same memcached servers.

Some features of the library must be enabled through `memcached_behavior_set`.

THREADS AND PROCESSES
---------------------

No global variables are used in this library.

`memcached_st` structures are thread-safe, but when using threads or forked
processes it is important to keep one instance of `memcached_st` per process or
thread. Without creating your own locking structures you can not share a single
`memcached_st`. However, you can call `memcached_quit` on a `memcached_st` and
then use the resulting cloned structure.

SYSTEMTAP
---------

`libmemcached` can be built to support Systemtap on Linux when enabled at
compile time.

Please see :manpage:`stap(1)` and :manpage:`dtrace(1)` for more information
about Systemtap.

CLIENT PROGRAMS
---------------

`libmemcached` comes with a few useful client programs:

.. only:: man

    :manpage:`memaslap(1)`
    :manpage:`memcapable(1)`
    :manpage:`memcat(1)`
    :manpage:`memcp(1)`
    :manpage:`memdump(1)`
    :manpage:`memerror(1)`
    :manpage:`memexist(1)`
    :manpage:`memflush(1)`
    :manpage:`memparse(1)`
    :manpage:`memping(1)`
    :manpage:`memrm(1)`
    :manpage:`memslap(1)`
    :manpage:`memstat(1)`
    :manpage:`memtouch(1)`

.. only:: html

    * :doc:`bin/memaslap`
    * :doc:`bin/memcapable`
    * :doc:`bin/memcat`
    * :doc:`bin/memcp`
    * :doc:`bin/memdump`
    * :doc:`bin/memerror`
    * :doc:`bin/memexist`
    * :doc:`bin/memflush`
    * :doc:`bin/memparse`
    * :doc:`bin/memping`
    * :doc:`bin/memrm`
    * :doc:`bin/memslap`
    * :doc:`bin/memstat`
    * :doc:`bin/memtouch`

UTILITY LIBRARIES
-----------------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`libmemcachedutil(3)`

.. only:: html

    * :doc:`libhashkit/index`
    * :doc:`libmemcachedutil/index`

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached_configuration(3)`
    :manpage:`libmemcached_examples(3)`

    :manpage:`memcached_analyze(3)`
    :manpage:`memcached_append(3)`
    :manpage:`memcached_auto(3)`
    :manpage:`memcached_behavior(3)`
    :manpage:`memcached_callback(3)`
    :manpage:`memcached_cas(3)`
    :manpage:`memcached_create(3)`
    :manpage:`memcached_delete(3)`
    :manpage:`memcached_dump(3)`
    :manpage:`memcached_exist(3)`
    :manpage:`memcached_fetch(3)`
    :manpage:`memcached_flush(3)`
    :manpage:`memcached_flush_buffers(3)`
    :manpage:`memcached_generate_hash_value(3)`
    :manpage:`memcached_get(3)`
    :manpage:`memcached_last_error_message(3)`
    :manpage:`memcached_memory_allocators(3)`
    :manpage:`memcached_pool(3)`
    :manpage:`memcached_quit(3)`
    :manpage:`memcached_result_st(3)`
    :manpage:`memcached_return_t(3)`
    :manpage:`memcached_sasl(3)`
    :manpage:`memcached_servers(3)`
    :manpage:`memcached_server_st(3)`
    :manpage:`memcached_set(3)`
    :manpage:`memcached_set_encoding_key(3)`
    :manpage:`memcached_stats(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_touch(3)`
    :manpage:`memcached_user_data(3)`
    :manpage:`memcached_verbosity(3)`
    :manpage:`memcached_version(3)`

.. only:: html

    * :manpage:`memcached(1)`

    * :doc:`libmemcached/configuration`
    * :doc:`libmemcached/examples`

