================
Available Hashes
================


Various hash functions to use for calculating values for keys


-------
LIBRARY
-------


C Library for hashing algorithms (libmemcached, -lhashkit)


--------
SYNOPSIS
--------



.. code-block:: perl

   #include <libmemcached/hashkit.h>
 
   uint32_t hashkit_default(const char *key, size_t key_length);
   uint32_t hashkit_fnv1_64(const char *key, size_t key_length);
   uint32_t hashkit_fnv1a_64(const char *key, size_t key_length);
   uint32_t hashkit_fnv1_32(const char *key, size_t key_length);
   uint32_t hashkit_fnv1a_32(const char *key, size_t key_length);
   uint32_t hashkit_crc32(const char *key, size_t key_length);
   uint32_t hashkit_hsieh(const char *key, size_t key_length);
   uint32_t hashkit_murmur(const char *key, size_t key_length);
   uint32_t hashkit_jenkins(const char *key, size_t key_length);
   uint32_t hashkit_md5(const char *key, size_t key_length);



-----------
DESCRIPTION
-----------


These functions generate hash values from a key using a variety of
algorithms. These functions can be used standalone, or as arguments
to hashkit_set_hash_fn(3) or hashkit_set_continuum_hash_fn(3).

The hashkit_hsieh() is only available if the library is built with
the appropriate flag enabled.


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


hashkit_create(3) hashkit_value(3) hashkit_set_hash_fn(3)
hashkit_set_continuum_hash_fn(3)

