==============================================
Introducing the C Client Library for memcached
==============================================

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

Compile and link with -lmemcached


-----------
DESCRIPTION
-----------


"Memcached is a high-performance, distributed memory object caching
system, generic in nature, but intended for use in speeding up dynamic web
applications by alleviating database load." `http://danga.com/memcached/ <http://danga.com/memcached/>`_

\ **libmemcached**\  is a small, thread-safe client library for the
memcached protocol. The code has all been written with an eye to allow
for both web and embedded usage. It handles the work behind routing
particular keys to specific servers that you specify (and values are
matched based on server order as supplied by you). It implements both
a modula and consistent method of object distribution.

There are multiple implemented routing and hashing methods. See the
memcached_behavior_set() manpage.

All operations are performed against a \ ``memcached_st``\  structure.
These structures can either be dynamically allocated or statically
allocated and then initialized by memcached_create(). Functions have been
written in order to encapsulate the \ ``memcached_st``\ . It is not
recommended that you operate directly against the structure.

Nearly all functions return a \ ``memcached_return_t``\  value.
This value can be translated to a printable string with memcached_strerror(3).

Objects are stored on servers by hashing keys. The hash value maps the key to a particular server. All clients understand how this hashing works, so it is possibly to reliably both push data to a server and retrieve data from a server.

Group keys can be optionally used to group sets of objects with servers. 

Namespaces are supported, and can be used to partition caches so that multiple applications can use the same memcached servers.

\ ``memcached_st``\  structures are thread-safe, but each thread must
contain its own structure (that is, if you want to share these among
threads you must provide your own locking). No global variables are
used in this library.

If you are working with GNU autotools you will want to add the following to
your COPYING to properly include libmemcached in your application.

PKG_CHECK_MODULES(DEPS, libmemcached >= 0.8.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

Some features of the library must be enabled through memcached_behavior_set().

Hope you enjoy it!


---------
CONSTANTS
---------


A number of constants have been provided for in the library.


.. c:var:: MEMCACHED_DEFAULT_PORT
 
 The default port used by memcached(3).
 


.. c:var:: MEMCACHED_MAX_KEY
 
 Default maximum size of a key (which includes the null pointer). Master keys
 have no limit, this only applies to keys used for storage.
 


.. c:var:: MEMCACHED_MAX_KEY
 
 Default size of key (which includes the null pointer).
 


.. c:var:: MEMCACHED_STRIDE
 
 This is the "stride" used in the consistent hash used between replicas.
 


.. c:var:: MEMCACHED_MAX_HOST_LENGTH
 
 Maximum allowed size of the hostname.
 

.. c:var:: LIBMEMCACHED_VERSION_STRING
 
 String value of libmemcached version such as "1.23.4"


.. c:var:: LIBMEMCACHED_VERSION_HEX
 
 Hex value of the version number. "0x00048000" This can be used for comparing versions based on number.
 



---------------------
THREADS AND PROCESSES
---------------------


When using threads or forked processes it is important to keep an instance
of \ ``memcached_st``\  per process or thread. Without creating your own locking
structures you can not share a single \ ``memcached_st``\ . You can though call
memcached_quit(3) on a \ ``memcached_st``\  and then use the resulting cloned
structure.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached_examples(3)`
:manpage:`libmemcached(1)` :manpage:`memcat(1)` :manpage:`memcp(1)`
:manpage:`memflush(1)` :manpage:`memrm(1)` :manpage:`memslap(1)`
:manpage:`memstat(1)` :manpage:`memcached_fetch(3)`
:manpage:`memcached_replace(3)` :manpage:`memcached_server_list_free(3)`
:manpage:`libmemcached_examples(3)` :manpage:`memcached_clone(3)`
:manpage:`memcached_free(3)` :manpage:`memcached_server_add(3)`
:manpage:`memcached_server_push(3)` :manpage:`memcached_add(3)`
:manpage:`memcached_get(3)` :manpage:`memcached_server_count(3)`
:manpage:`memcached_create(3)` :manpage:`memcached_increment(3)`
:manpage:`memcached_server_list(3)` :manpage:`memcached_set(3)`
:manpage:`memcached_decrement(3)` :manpage:`memcached_mget(3)`
:manpage:`memcached_server_list_append(3)` :manpage:`memcached_strerror(3)`
:manpage:`memcached_delete(3)` :manpage:`memcached_quit(3)`
:manpage:`memcached_server_list_count(3)` :manpage:`memcached_verbosity(3)`
:manpage:`memcached_server_add_unix_socket(3)`
:manpage:`memcached_result_create(3)`  :manpage:`memcached_result_free(3)`
:manpage:`memcached_result_key_value(3)`
:manpage:`memcached_result_key_length(3)`
:manpage:`memcached_result_value(3)`  :manpage:`memcached_result_length(3)`
:manpage:`memcached_result_flags(3)`  :manpage:`memcached_result_cas(3)`
:manpage:`memcached_result_st(3)` :manpage:`memcached_append(3)`
:manpage:`memcached_prepend(3)` :manpage:`memcached_fetch_result(3)`
:manpage:`memerror(1)` :manpage:`memcached_get_by_key(3)`
:manpage:`memcached_mget_by_key(3)` :manpage:`memcached_delete_by_key(3)`
:manpage:`memcached_fetch_execute(3)` :manpage:`memcached_callback_get(3)`
:manpage:`memcached_callback_set(3)` :manpage:`memcached_version(3)`
:manpage:`memcached_lib_version(3)` :manpage:`memcached_result_set_value(3)`
:manpage:`memcached_dump(3)` :manpage:`memdump(1)`
:manpage:`memcached_set_memory_allocators(3)`
:manpage:`memcached_get_memory_allocators(3)`
:manpage:`memcached_get_user_data(3)` :manpage:`memcached_set_user_data(3)`
