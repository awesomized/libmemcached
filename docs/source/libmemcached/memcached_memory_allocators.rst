Use custom allocators for embedded usage
========================================

Manage memory allocator functions

SYNOPSIS
--------

#include <libmemcached/memcached.h>
  Compile and link with -lmemcached

.. function:: memcached_return_t memcached_set_memory_allocators (memcached_st *ptr, memcached_malloc_fn mem_malloc, memcached_free_fn mem_free, memcached_realloc_fn mem_realloc, memcached_calloc_fn mem_calloc, void *context)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param mem_malloc: pointer to a `memcached_malloc_fn` callback
    :param mem_free: pointer to a `memcached_free_fn` callback
    :param mem_realloc: pointer to a `memcached_realloc_fn` callback
    :param mem_calloc: pointer to a `memcached_calloc_fn` callback
    :param context: pointer to a user supplied context
    :returns: `memcached_return_t` indicating success

.. function:: void memcached_get_memory_allocators (memcached_st *ptr, memcached_malloc_fn *mem_malloc, memcached_free_fn *mem_free, memcached_realloc_fn *mem_realloc, memcached_calloc_fn *mem_calloc)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param mem_malloc: pointer to store the address of the `memcached_malloc_fn` callback
    :param mem_free: pointer to store the address of the `memcached_free_fn` callback
    :param mem_realloc: pointer to store the address of the `memcached_realloc_fn` callback
    :param mem_calloc: pointer to store the address of the `memcached_calloc_fn` callback

.. function:: void * memcached_get_memory_allocators_context(const memcached_st *ptr)

    :param ptr: pointer to an initialized `memcached_st` struct
    :returns: pointer to the user supplied context

.. type:: void * (*memcached_malloc_fn) (memcached_st *ptr, const size_t size, void *context)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param size: the number of bytes to allocate
    :param context: pointer to the user supplied context
    :returns: pointer to at least `size` bytes of allocated memory

.. type:: void * (*memcached_realloc_fn) (memcached_st *ptr, void *mem, const size_t size, void *context)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param mem: pointer to previously allocated memory
    :param size: the number of bytes to allocate
    :param context: pointer to the user supplied context
    :returns: pointer to at least `size` bytes of allocated memory

.. type:: void (*memcached_free_fn) (memcached_st *ptr, void *mem, void *context)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param mem: pointer to previously allocated memory
    :param context: pointer to the user supplied context

.. type:: void * (*memcached_calloc_fn) (memcached_st *ptr, size_t nelem, const size_t elsize, void *context)

    :param ptr: pointer to an initialized `memcached_st` struct
    :param nelem: number of elements to allocate
    :param elsize: the number of bytes to allocate per element
    :param context: pointer to the user supplied context
    :returns: pointer to at least `elsize` \* `nelem`  bytes of allocated and zeroed memory

DESCRIPTION
-----------

`libmemcached` allows you to specify your own memory allocators, optimized for
your application. This enables libmemcached to be used inside of applications
that have their own malloc implementation.

:func:`memcached_set_memory_allocators` is used to set the memory allocators
used by the memcached instance specified by ptr. Please note that you cannot
override only one of the memory allocators, you have to specify a complete new
set if you want to override one of them. All of the memory allocation functions
should behave as specified in the C99 standard. Specify NULL as all functions to
reset them to the default values.

:func:`memcached_get_memory_allocators` is used to get the currently used memory
allocators by a memcached handle.

:func:`memcached_get_memory_allocators_context` returns the void \* that was
passed in during the call to :func:`memcached_set_memory_allocators`.

The first argument to the memory allocator functions is a pointer to a memcached
structure, the is passed as const and you will need to clone it in order to make
use of any operation which would modify it.

NOTES
-----

In version 0.38 all functions were modified to have a context void pointer
passed to them. This was so that custom allocators could have their own space
for memory.

RETURN VALUE
------------

:func:`memcached_set_memory_allocators` returns `MEMCACHED_SUCCESS` upon success,
and `MEMCACHED_FAILURE` if you don't pass a complete set of function pointers.

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
