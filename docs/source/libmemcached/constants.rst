libmemcached Constants and Defaults
===================================

SYNOPSIS
--------

#include <libmemcached-|libmemcached_version|/memcached.h>
  Compile and link with -lmemcached

.. c:macro:: LIBMEMCACHED_VERSION_STRING

    String value of libmemcached version such as "|release|"

.. c:macro:: LIBMEMCACHED_VERSION_HEX

    Hex value of the version number, e.g. "0x00048000".
    This can be used for comparing versions based on number.


.. c:macro:: MEMCACHED_DEFAULT_PORT

    The default port used by `memcached`.

.. c:macro:: MEMCACHED_DEFAULT_TIMEOUT

    Default timeout of 5000 milliseconds.

.. c:macro:: MEMCACHED_DEFAULT_CONNECT_TIMEOUT

    Default connect timeout of 4000 milliseconds.


.. c:macro:: MEMCACHED_MAX_BUFFER

    Default size of read/write buffers (which includes the null pointer).

.. c:macro:: MEMCACHED_MAX_KEY

    Default maximum size of a key (which includes the null pointer). Master keys
    have no limit, this only applies to keys used for storage.

.. c:macro:: MEMCACHED_MAX_NAMESPACE

    Maximum length allowed for namespacing of a key. Defaults to 128.


.. c:macro:: MEMCACHED_MAX_HOST_LENGTH

    Maximum allowed length of the hostname.

.. c:macro:: MEMCACHED_MAX_HOST_SORT_LENGTH

    Length of the host string used for sorting. Used for Ketama.

.. c:macro:: MEMCACHED_MAX_INTEGER_DISPLAY_LENGTH

    Maximum display width of an integer represented as string.


.. c:macro:: MEMCACHED_CONTINUUM_ADDITION

    How many extra slots we should build for in the continuum, defaults to 10.

.. c:macro:: MEMCACHED_EXPIRATION_NOT_ADD

    Value ``0xffffffffU``

.. c:macro:: MEMCACHED_STRIDE

    This is the "stride" used in the consistent hash used between replicas.


.. c:macro:: MEMCACHED_SERVER_FAILURE_LIMIT

    Value 5

.. c:macro:: MEMCACHED_SERVER_FAILURE_RETRY_TIMEOUT

    Value 2

.. c:macro:: MEMCACHED_SERVER_FAILURE_DEAD_TIMEOUT

    Value 0


.. c:macro:: MEMCACHED_VERSION_STRING_LENGTH

    Value 24

DESCRIPTION
-----------

These compile time defaults are provided by `libmemcached` for convenience.
