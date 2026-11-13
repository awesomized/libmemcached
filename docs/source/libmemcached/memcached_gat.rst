Get and update expiration atomically
=====================================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: char *memcached_gat (memcached_st *ptr, const char *key, size_t key_length, time_t expiration, size_t *value_length, uint32_t *flags, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param key: the key to fetch and touch
    :param key_length: the length of `key` without any terminating zero
    :param expiration: new expiration as a unix timestamp or as relative expiration time in seconds
    :param value_length: pointer filled with the length of the returned value, or ``NULL``
    :param flags: pointer filled with the flags stored with the value, or ``NULL``
    :param error: pointer filled with the return status, or ``NULL``
    :returns: pointer to the retrieved value, or ``NULL`` on error or cache miss

.. function:: char *memcached_gat_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, time_t expiration, size_t *value_length, uint32_t *flags, memcached_return_t *error)

    :param ptr: pointer to initialized `memcached_st` struct
    :param group_key: the key namespace used to select the server
    :param group_key_length: the length of `group_key` without any terminating zero
    :param key: the key to fetch and touch
    :param key_length: the length of `key` without any terminating zero
    :param expiration: new expiration as a unix timestamp or as relative expiration time in seconds
    :param value_length: pointer filled with the length of the returned value, or ``NULL``
    :param flags: pointer filled with the flags stored with the value, or ``NULL``
    :param error: pointer filled with the return status, or ``NULL``
    :returns: pointer to the retrieved value, or ``NULL`` on error or cache miss

DESCRIPTION
-----------

:func:`memcached_gat` (Get And Touch) atomically fetches the value for a key
and updates its expiration time in a single round trip to the server.  It
combines the semantics of :func:`memcached_get` and :func:`memcached_touch`:
the current value is returned to the caller while the TTL is refreshed.

:func:`memcached_gat_by_key` works identically but accepts a `group_key`
that controls which server the key is located on, enabling key partitioning.

Both functions support the text protocol (``gat``/``gats``) and the binary
protocol (``GATK`` opcode).  When `MEMCACHED_BEHAVIOR_SUPPORT_CAS` is
enabled the text protocol uses ``gats``, and the response includes a CAS
token accessible via :func:`memcached_result_cas`.

The returned value must be released by the caller using :manpage:`free(3)`.

These functions are not supported when `MEMCACHED_BEHAVIOR_USE_UDP` is set;
that behavior returns `MEMCACHED_NOT_SUPPORTED`.

RETURN VALUE
------------

On success, a pointer to the retrieved value is returned and `*error` is set
to `MEMCACHED_SUCCESS`.  The caller must free this pointer with
:manpage:`free(3)`.

``NULL`` is returned when:

* The key does not exist â `*error` is set to `MEMCACHED_NOTFOUND`.
* A network or protocol error occurred â `*error` describes the failure.

Use :func:`memcached_strerror` to convert a :type:`memcached_return_t` value
to a human-readable string.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_get(3)`
    :manpage:`memcached_touch(3)`
    :manpage:`memcached_strerror(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_get`
    * :doc:`memcached_touch`
    * :doc:`memcached_strerror`
