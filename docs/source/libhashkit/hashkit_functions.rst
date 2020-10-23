Available Hashes
================

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. function:: uint32_t hashkit_default(const char *key, size_t key_length)

.. function:: uint32_t hashkit_fnv1_64(const char *key, size_t key_length)

.. function:: uint32_t hashkit_fnv1a_64(const char *key, size_t key_length)

.. function:: uint32_t hashkit_fnv1_32(const char *key, size_t key_length)

.. function:: uint32_t hashkit_fnv1a_32(const char *key, size_t key_length)

.. function:: uint32_t hashkit_crc32(const char *key, size_t key_length)

.. function:: uint32_t hashkit_hsieh(const char *key, size_t key_length)

.. function:: uint32_t hashkit_murmur(const char *key, size_t key_length)

.. function:: uint32_t hashkit_murmur3(const char *key, size_t key_length)

.. function:: uint32_t hashkit_jenkins(const char *key, size_t key_length)

.. function:: uint32_t hashkit_md5(const char *key, size_t key_length)

DESCRIPTION
-----------

These functions generate hash values from a key using a variety of
algorithms. These functions can be used standalone, or will be used
according to the algorithm set with `hashkit_set_function`
or `hashkit_set_distribution_function`.

The `hashkit_hsieh`, `hashkit_murmur` and `hashkit_murmur3` functions are
only available if the library is built with the appropriate flag enabled.

RETURN VALUE
------------

A 32-bit hash value.

SEE ALSO
--------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`hashkit_create(3)`
    :manpage:`hashkit_function(3)`

.. only:: html

    * :doc:`index`
    * :doc:`hashkit_create`
    * :doc:`hashkit_function`

