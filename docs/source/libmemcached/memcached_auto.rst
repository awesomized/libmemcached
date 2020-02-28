====================================
Incrementing and Decrementing Values
====================================

.. index:: object: memcached_st

--------
SYNOPSIS
--------

#include <libmemcached/memcached.h>

.. function:: memcached_return_t memcached_increment (memcached_st *ptr, const char *key, size_t key_length, uint32_t offset, uint64_t *value)

.. function:: memcached_return_t memcached_decrement (memcached_st *ptr, const char *key, size_t key_length, uint32_t offset, uint64_t *value)

.. function:: memcached_return_t memcached_increment_with_initial (memcached_st *ptr, const char *key, size_t key_length, uint64_t offset, uint64_t initial, time_t expiration, uint64_t *value)

.. function:: memcached_return_t memcached_decrement_with_initial (memcached_st *ptr, const char *key, size_t key_length, uint64_t offset, uint64_t initial, time_t expiration, uint64_t *value)

.. function::  memcached_return_t memcached_increment_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, uint32_t offset, uint64_t *value)

.. function:: memcached_return_t memcached_decrement_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, uint32_t offset, uint64_t *value)

.. function:: memcached_return_t memcached_increment_with_initial_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, uint64_t offset, uint64_t initial, time_t expiration, uint64_t *value)

.. function:: memcached_return_t memcached_decrement_with_initial_by_key (memcached_st *ptr, const char *group_key, size_t group_key_length, const char *key, size_t key_length, uint64_t offset, uint64_t initial, time_t expiration, uint64_t *value)

Compile and link with -lmemcached

-----------
DESCRIPTION
-----------

:manpage:`memcached(1)` servers have the ability to increment and decrement keys
(overflow and underflow are not detected). This gives you the ability to use
memcached to generate shared sequences of values.

memcached_increment takes a key and key length and increments the value by
the offset passed to it. The value is then returned via the uint32_t
value pointer you pass to it.

memcached_decrement takes a key and keylength and decrements the value by
the offset passed to it. The value is then returned via the uint32_t
value pointer you pass to it.

memcached_increment_with_initial takes a key and keylength and increments
the value by the offset passed to it. If the object specified by key does
not exist, one of two things may happen: If the expiration value is
MEMCACHED_EXPIRATION_NOT_ADD, the operation will fail. For all other
expiration values, the operation will succeed by seeding the value for that
key with a initial value to expire with the provided expiration time. The
flags will be set to zero.The value is then returned via the uint32_t
value pointer you pass to it. memcached_increment_with_initial is only available 
when using the binary protocol.

memcached_decrement_with_initial takes a key and keylength and decrements
the value by the offset passed to it. If the object specified by key does
not exist, one of two things may happen: If the expiration value is
MEMCACHED_EXPIRATION_NOT_ADD, the operation will fail. For all other
expiration values, the operation will succeed by seeding the value for that
key with a initial value to expire with the provided expiration time. The
flags will be set to zero.The value is then returned via the uint32_t
value pointer you pass to it. memcached_decrement_with_initial is only available
when using the binary protocol.

:func:`memcached_increment_by_key`, :func:`memcached_decrement_by_key`,
:func:`memcached_increment_with_initial_by_key`, and
:func:`memcached_decrement_with_initial_by_key` are master key equivalents of the above.

------
RETURN
------


A value of type :type:`memcached_return_t`  is returned.
On success that value will be `MEMCACHED_SUCCESS`.
Use memcached_strerror to translate this value to a printable string.

--------
SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
