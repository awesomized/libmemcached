=============
hashkit_value
=============


Generate a value for the given key


-------
LIBRARY
-------


C Library for hashing algorithms (libmemcached, -lhashkit)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/hashkit.h>
 
   uint32_t hashkit_value(hashkit_st *hash,
                          const char *key,
                          size_t key_length);



-----------
DESCRIPTION
-----------


The hashkit_value() function generates a 32-bit hash value from the
given key and key_length. The hash argument is an initialized hashkit
object, and distribution type and hash function is used from this
object while generating the value.


------------
RETURN VALUE
------------


A 32-bit hash value.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


hashkit_create(3) hashkit_set_distribution(3) hashkit_set_hash_fn(3)

