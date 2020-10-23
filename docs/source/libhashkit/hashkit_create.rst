Creating a hashkit structure
============================

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. type:: struct hashkit_st hashkit_st

.. function:: hashkit_st *hashkit_create(hashkit_st *hash)

    :param hash: memory address of a `hashkit_st` struct;
        if a nullptr is passed, the struct will be dynamically allocated by libhashkit
    :returns: pointer to initialized `hashkit_st` structure

.. function:: hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr)

    :param destination: memory address of a `hashkit_st` struct;
        if a nullptr is passed, the struct will be dynamically allocated by libhashkit
    :param ptr: pointer of the `hashkit_st` struct to copy
    :returns: pointer to a `hashkit_st` structure (`destination`, if not nullptr), initialized from `ptr`

.. function:: void hashkit_free(hashkit_st *hash)

    :param hash: pointer to an initialized `hashkit_st` struct

.. function:: bool hashkit_is_allocated(const hashkit_st *hash)

    :param hash: pointer to an initialized `hashkit_st` struct
    :returns: bool, whether the `hash` struct was dynamically allocated

DESCRIPTION
-----------

The `hashkit_create` function initializes a hashkit object for use. If you pass
a nullptr argument for hash, then the memory for the object is allocated. If you
specify a pre-allocated piece of memory, that is initialized for use.

The `hashkit_clone` function initializes a hashkit object much like
`hashkit_create`, but instead of using default settings it will use the settings
of the ptr hashkit object.

The `hashkit_free` frees any resources being consumed by the hashkit objects
that were initialized with `hashkit_create` or `hashkit_clone`.

The `hashkit_is_allocated` reports whether the memory was allocated for a hashkit
object.

RETURN VALUE
------------

`hashkit_create` and `hashkit_clone` will return nullptr on failure or pointer
to `hashkit_st` on success.

`hashkit_is_allocated` returns true if the memory for the hashkit object was
allocated inside of `hashkit_create` or `hashkit_clone`, otherwise it is false
and was user-supplied memory.

SEE ALSO
--------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`hashkit_value(3)`
    :manpage:`hashkit_function3)`

.. only:: html

    * :doc:`index`
    * :doc:`hashkit_value`
    * :doc:`hashkit_function`
