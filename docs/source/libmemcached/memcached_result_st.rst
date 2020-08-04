Working with result sets
========================

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcachedutil -lmemcached

.. type:: struct memcached_result_st memcached_result_st

.. function:: memcached_result_st *memcached_result_create (memcached_st *ptr, memcached_result_st *result)

    :param ptr: pointer to initialized `memcached_st` struct
    :param result: pointer to an `memcached_result_st` instance to initialize or
                   nullptr to allocate a new instance
    :returns: pointer to initialized `memcached_result_st` instance

.. function:: void memcached_result_free (memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct

.. function:: const char * memcached_result_key_value (memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the key value associated with the current result object

.. function:: size_t memcached_result_key_length (const memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the key length associated with the current result object

.. function:: const char *memcached_result_value (memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the result value associated with the current result object

.. function:: char *memcached_result_take_value (memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the result value associated with the current result object

.. function:: size_t memcached_result_length (const memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the result length associated with the current result object

.. function:: uint32_t memcached_result_flags (const memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the flags associated with the current result object

.. function:: uint64_t memcached_result_cas (const memcached_result_st *result)

    :param result: pointer to initialized `memcached_result_st` struct
    :returns: the cas associated with the current result object

.. function:: memcached_return_t memcached_result_set_value (memcached_result_st *result, const char *value, size_t length)

    :param result: pointer to initialized `memcached_result_st` struct
    :param value: byte array to set the value of the current result object to
    :param length: the length of `value` wothout any terminating zero
    :returns: `memcached_return_t` indicating success

.. function:: void memcached_result_set_flags (memcached_result_st *result, uint32_t flags)

    :param result: pointer to initialized `memcached_result_st` struct
    :param flags: a new value for the flags field

.. function:: void memcached_result_set_expiration (memcached_result_st *result, time_t expiration)

    :param result: pointer to initialized `memcached_result_st` struct
    :param expiration: a new value for the expiration field


DESCRIPTION
-----------

`libmemcached` can optionally return a :type:`memcached_result_st` which
acts as a result object. The result objects have added benefits over the 
character pointer return values, in that they are forward compatible with new
return items that future memcached servers may implement (the best current 
example of this is the CAS return item). The structures can also be reused, 
which will save on calls to :manpage:`malloc(3)`. It is suggested that you use
result objects over char \* return functions.

The structure of :type:`memcached_result_st` has been encapsulated, you should
not write code directly accessing members of the structure.

:func:`memcached_result_create` will either allocate memory for a
:type:`memcached_result_st` or will initialize a structure passed to it.

:func:`memcached_result_free` will deallocate any memory attached to the
structure. If the structure was also allocated, it will deallocate it.

:func:`memcached_result_key_value` returns the key value associated with the
current result object.

:func:`memcached_result_key_length` returns the key length associated with
the current result object.

:func:`memcached_result_value` returns the result value associated with the
current result object.

:func:`memcached_result_take_value` returns and hands over the result value
associated with the current result object. You must call :manpage:`free(3)` to
release this value, unless you have made use of a custom allocator. Use of a
custom allocator requires that you create your own custom free() to release it.

:func:`memcached_result_length` returns the result length associated with
the current result object.

:func:`memcached_result_flags` returns the flags associated with the
current result object.

:func:`memcached_result_cas` returns the cas associated with the
current result object. This value will only be available if the server
tests it.

:func:`memcached_result_set_value` takes a byte array and a size and sets
the result to this value. This function is used for trigger responses.

:func:`memcached_result_set_flags` takes a result structure and stores a new
value for the flags field.

:func:`memcached_result_set_expiration` takes a result structure and stores
a new value for the expiration field (this is only used by read through
triggers).


RETURN VALUE
------------

Varies, see particular functions. All structures must have
:func:`memcached_result_free` called on them for cleanup purposes. Failure 
to do this will result in leaked memory.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`memcached_memory_allocators(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
    * :doc:`memcached_memory_allocators`
