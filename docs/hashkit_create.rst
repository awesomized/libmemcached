============================
Creating a hashkit structure
============================

.. highlightlang:: c

.. index:: object: hashkit_st

--------
SYNOPSIS
--------

#include <libhashkit/hashkit.h>
 
.. c:type:: hashkit_st

.. c:function:: hashkit_st *hashkit_create(hashkit_st *hash)
 
.. c:function:: hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr)

.. c:function:: void hashkit_free(hashkit_st *hash)

.. c:function:: bool hashkit_is_allocated(const hashkit_st *hash)

Compile and link with -lhashkit

-----------
DESCRIPTION
-----------


The :c:func:`hashkit_create` function initializes a hashkit object for use. If
you pass a NULL argument for hash, then the memory for the object is
allocated. If you specify a pre-allocated piece of memory, that is
initialized for use.

The :c:func:`hashkit_clone` function initializes a hashkit object much like
:c:func:`hashkit_create`, but instead of using default settings it will use
the settings of the ptr hashkit object.

The :c:func:`hashkit_free` frees any resources being consumed by the hashkit
objects that were initialized with :c:func:`hashkit_create` or :c:func:`hashkit_clone`.

The :c:func:`hashkit_is_allocated` reports where the memory was allocated 
for a hashkit object.


------------
RETURN VALUE
------------


:c:func:`hashkit_create` and :c:func:`hashkit_clone` will return NULL on 
failure or non-NULL on success.

:c:func:`hashkit_is_allocated` returns true if the memory for the hashkit
object was allocated inside of :c:func:`hashkit_create` or 
:c:func:`hashkit_clone`, otherwise it is false and was user-supplied memory.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_



--------
SEE ALSO
--------


:manpage:`hashkit_create(3)` :manpage:`hashkit_value(3)` :manpage:`hashkit_set_hash_fn(3)`

