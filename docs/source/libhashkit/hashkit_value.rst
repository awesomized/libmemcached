Generate hash value
===================

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. function:: uint32_t hashkit_value(hashkit_st *hash, const char *key, size_t key_length)

    :param hash: pointer to an initialized `hashkit_st` struct
    :param key: the key to genereate a hash of
    :param key_length: the length of the `key` without any terminating zero byte

DESCRIPTION
-----------

The `hashkit_value` function generates a 32-bit hash value from the given key
and key_length. The hash argument is an initialized hashkit object, and
distribution type and hash function is used from this object while generating
the value.

RETURN VALUE
------------

A 32-bit hash value.

SEE ALSO
--------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`hashkit_create(3)`
    :manpage:`hashkit_function(3)`
    :manpage:`hashkit_functions(3)`

.. only:: html

    * :doc:`index`
    * :doc:`hashkit_create`
    * :doc:`hashkit_function`
    * :doc:`hashkit_functions`

