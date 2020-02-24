==============================================
Introducing the C Client Library for memcached
==============================================

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

=======

libMemcached is an open source C/C++ client library and tools for the memcached server (http://memcached.org/).
It has been designed to be light on memory usage, thread safe, and provide full access to server side methods.

libMemcached was designed to provide the greatest number of options to use Memcached. Some of the features provided:

1. Asynchronous and Synchronous Transport Support.
2. Consistent Hashing and Distribution.
3. Tunable Hashing algorithm to match keys.
4. Access to large object support.
5. Local replication.
6. A complete reference guide and documentation to the API.
7. Tools to Manage your Memcached networks.

-----------
DESCRIPTION
-----------


"Memcached is a high-performance, distributed memory object caching
system, generic in nature, but intended for use in speeding up dynamic web
applications by alleviating database load." `http://memcached.org/ <http://memcached.org/>`_

:program:`libmemcached` is a small, thread-safe client library for the
memcached protocol. The code has all been written to allow
for both web and embedded usage. It handles the work behind routing
individual keys to specific servers specified by the developer (and values are
matched based on server order as supplied by the user). It implements
a modular and consistent method of object distribution.

There are multiple implemented routing and hashing methods. See the
:c:func:`memcached_behavior_set` manpage for more information.

All operations are performed against a :c:type:`memcached_st` structure.
These structures can either be dynamically allocated or statically
allocated and then initialized by :c:func:`memcached_create`. Functions have 
been written in order to encapsulate the :c:type:`memcached_st`. It is not
recommended that you operate directly against the structure.

Nearly all functions return a :c:type:`memcached_return_t` value.
This value can be translated to a printable string with 
:c:type:`memcached_strerror`.

Objects are stored on servers by hashing keys. The hash value maps the key to a particular server.
All clients understand how this hashing works, so it is possibly to reliably both push data to a server and retrieve data from a server.

Group keys can be optionally used to group sets of objects with servers. 

Namespaces are supported, and can be used to partition caches so that multiple applications can use the same memcached servers.

:c:type:`memcached_st` structures are thread-safe, but each thread must
contain its own structure (that is, if you want to share these among
threads you must provide your own locking). No global variables are
used in this library.

If you are working with GNU autotools you will want to add the following to
your COPYING to properly include libmemcached in your application.

PKG_CHECK_MODULES(DEPS, libmemcached >= 0.8.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

Some features of the library must be enabled through :c:func:`memcached_behavior_set`.


---------
CONSTANTS
---------


A number of constants have been provided for in the library.


.. c:macro:: MEMCACHED_DEFAULT_PORT
 
 The default port used by memcached(3).
 

.. c:macro:: MEMCACHED_MAX_KEY
 
 Default maximum size of a key (which includes the null pointer). Master keys
 have no limit, this only applies to keys used for storage.
 

.. c:macro:: MEMCACHED_MAX_BUFFER
 
 Default size of read/write buffers (which includes the null pointer).
 

.. c:macro:: MEMCACHED_STRIDE
 
 This is the "stride" used in the consistent hash used between replicas.
 

.. c:macro:: MEMCACHED_MAX_HOST_LENGTH
 
 Maximum allowed size of the hostname.
 

.. c:macro:: LIBMEMCACHED_VERSION_STRING
 
 String value of libmemcached version such as "1.23.4"


.. c:macro:: LIBMEMCACHED_VERSION_HEX
 
 Hex value of the version number. "0x00048000" This can be used for comparing versions based on number.
 

.. c:macro:: MEMCACHED_PREFIX_KEY_MAX_SIZE

 Maximum length allowed for namespacing of a key.



---------------------
THREADS AND PROCESSES
---------------------


When using threads or forked processes it is important to keep one instance
of :c:type:`memcached_st` per process or thread. Without creating your own 
locking structures you can not share a single :c:type:`memcached_st`. However, 
you can call :c:func:`memcached_quit` on a :c:type:`memcached_st` and then use the resulting cloned structure.



--------
SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
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
    :manpage:`libhashkit(3)`
    :manpage:`libmemcached_configuration(3)`
    :manpage:`libmemcached_examples(3)`
    :manpage:`libmemcachedutil(3)`
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
    * :doc:`libhashkit`
    * :doc:`libmemcached_configuration`
    * :doc:`libmemcached_examples`
    * :doc:`libmemcachedutil`
    * :doc:`memcached_analyze`
    * :doc:`memcached_append`
    * :doc:`memcached_auto`
    * :doc:`memcached_behavior`
    * :doc:`memcached_callback`
    * :doc:`memcached_cas`
    * :doc:`memcached_create`
    * :doc:`memcached_delete`
    * :doc:`memcached_dump`
    * :doc:`libmemcached/memcached_exist`
    * :doc:`libmemcached/memcached_fetch`
    * :doc:`memcached_flush`
    * :doc:`memcached_flush_buffers`
    * :doc:`memcached_generate_hash_value`
    * :doc:`memcached_get`
    * :doc:`libmemcached/memcached_last_error_message`
    * :doc:`memcached_memory_allocators`
    * :doc:`memcached_pool`
    * :doc:`memcached_quit`
    * :doc:`memcached_result_st`
    * :doc:`libmemcached/memcached_return_t`
    * :doc:`memcached_sasl`
    * :doc:`memcached_servers`
    * :doc:`memcached_server_st`
    * :doc:`memcached_set`
    * :doc:`libmemcached-1.0/memcached_set_encoding_key`
    * :doc:`memcached_stats`
    * :doc:`memcached_strerror`
    * :doc:`libmemcached-1.0/memcached_touch`
    * :doc:`memcached_user_data`
    * :doc:`memcached_verbosity`
    * :doc:`memcached_version`
