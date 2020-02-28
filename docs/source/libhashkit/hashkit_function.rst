Set Hash Function
=================

.. index:: object: hashkit_st
.. index:: object: hashkit_hash_fn

Set hash functions to use for calculating values for keys

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. type:: uint32_t (*hashkit_hash_fn)(const char *key, size_t key_length, void *context)

.. function:: hashkit_return_t hashkit_set_function(hashkit_st *hash, hashkit_hash_algorithm_t hash_algorithm)

.. function:: hashkit_return_t hashkit_set_custom_function(hashkit_st *hash, hashkit_hash_fn function, void *context)

.. function:: hashkit_hash_algorithm_t hashkit_get_function(const hashkit_st *hash)

.. function:: hashkit_return_t hashkit_set_distribution_function(hashkit_st *hash, hashkit_hash_algorithm_t hash_algorithm)

.. function:: hashkit_return_t hashkit_set_custom_distribution_function(hashkit_st *self, hashkit_hash_fn function, void *context)

.. function:: hashkit_hash_algorithm_t hashkit_get_distribution_function(const hashkit_st *self)

DESCRIPTION
-----------

These functions are used to set and retrieve the key and distribution hash functions.


RETURN VALUE
------------

:func:`hashkit_set_function`, :func:`hashkit_set_custom_function` and the distribution equivalents
return :type:`hashkit_return_t` `HASHKIT_SUCCESS` on success.

:func:`hashkit_get_function` and :func:`hashkit_get_distribution_function` return :type:`hashkit_hash_algorithm_t`
indicating the hash function used.

SEE ALSO
--------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`hashkit_create(3)`
    :manpage:`hashkit_functions(3)`

.. only:: html

    * :doc:`../libhashkit`
    * :doc:`hashkit_create`
    * :doc:`hashkit_functions`

