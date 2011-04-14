============================
Creating a hashkit structure
============================


Create and destroy hashkit objects


-------
LIBRARY
-------


C Library for hashing algorithms (libmemcached, -lhashkit)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/hashkit.h>
 
   hashkit_st *hashkit_create(hashkit_st *hash);
 
   hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr);
 
   void hashkit_free(hashkit_st *hash);
 
   bool hashkit_is_allocated(const hashkit_st *hash);



-----------
DESCRIPTION
-----------


The hashkit_create() function initializes a hashkit object for use. If
you pass a NULL argument for hash, then the memory for the object is
allocated. If you specify a pre-allocated piece of memory, that is
initialized for use.

The hashkit_clone() function initializes a hashkit object much like
hashkit_create(), but instead of using default settings it will use
the settings of the ptr hashkit object.

The hashkit_free() frees any resources being consumed by the hashkit
objects that were initialized with hashkit_create() or hashkit_clone().

The hashkit_is_allocated() reports where the memory was allocated 
for a hashkit object.


------------
RETURN VALUE
------------


hashkit_create() and hashkit_clone() will return NULL on failure or
non-NULL on success.

hashkit_is_allocated() returns true if the memory for the hashkit
object was allocated inside of hashkit_create() or hashkit_clone(),
otherwise it is false and was user-supplied memory.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_
