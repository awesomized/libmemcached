Set Hash Function
=================

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. type:: uint32_t (*hashkit_hash_fn)(const char *key, size_t key_length, void *context)

    :param key: the key to generate a hash of
    :param key_length: the length of the `key` without any terminating zero byte
    :param context: the custom hash function context set through `hashkit_set_custom_function` or `hashkit_set_custom_distribution_function`
    :returns: the custom hash function should return a hash value for `key` as an unsigned 32bit integer

.. c:type:: enum hashkit_return_t hashkit_return_t

.. enum:: hashkit_return_t

    .. enumerator:: HASHKIT_SUCCESS

        Operation succeeded.

    .. enumerator:: HASHKIT_FAILURE

        Operation failed.

    .. enumerator:: HASHKIT_MEMORY_ALLOCATION_FAILURE

        Memory allocation failed.

    .. enumerator:: HASHKIT_INVALID_HASH

        Invalid `hashkit_hash_algorithm_t` passed.

    .. enumerator:: HASHKIT_INVALID_ARGUMENT

        Invalid argument passed.

.. c:type:: enum hashkit_hash_algorithm_t hashkit_hash_algorithm_t

.. enum:: hashkit_hash_algorithm_t

    .. enumerator:: HASHKIT_HASH_DEFAULT

        Default hash algorithm (one_at_a_time).

    .. enumerator:: HASHKIT_HASH_MD5
    .. enumerator:: HASHKIT_HASH_CRC
    .. enumerator:: HASHKIT_HASH_FNV1_64
    .. enumerator:: HASHKIT_HASH_FNV1A_64
    .. enumerator:: HASHKIT_HASH_FNV1_32
    .. enumerator:: HASHKIT_HASH_FNV1A_32
    .. enumerator:: HASHKIT_HASH_HSIEH

        Only available if `libhashkit` hash been built with HSIEH support.

    .. enumerator:: HASHKIT_HASH_MURMUR

        Only available if `libhashkit` has been built with MURMUR support.

    .. enumerator:: HASHKIT_HASH_MURMUR3

        Only available if `libhashkit` has been built with MURMUR support.

    .. enumerator:: HASHKIT_HASH_JENKINS
    .. enumerator:: HASHKIT_HASH_CUSTOM

        Use custom `hashkit_hash_fn` function set through `hashkit_set_custom_function` or `hashkit_set_custom_distribution_function`.

.. function:: hashkit_return_t hashkit_set_function(hashkit_st *hash, hashkit_hash_algorithm_t hash_algorithm)

    :param hash: pointer to an initialized `hashkit_st` struct
    :param hash_algorithm: valid `hashkit_hash_algorithm_t` constant
    :returns: `hashkit_return_t` indicating success or failure

.. function:: hashkit_return_t hashkit_set_custom_function(hashkit_st *hash, hashkit_hash_fn function, void *context)

    :param hash: pointer to initialized `hashkit_st` struct
    :param function: `hashkit_hash_fn` function pointer to use as hash function for `HASHKIT_HASH_CUSTOM`
    :param context: pointer to an opaque user managed context for the custom hash function
    :returns: `hashkit_return_t` indicating success or failure

.. function:: hashkit_hash_algorithm_t hashkit_get_function(const hashkit_st *hash)

    :param hash: pointer to an initialized `hashkit_st` struct
    :returns: `hashkit_hash_algorithm_t` indicating the currently set hash algorithm to use

.. function:: hashkit_return_t hashkit_set_distribution_function(hashkit_st *hash, hashkit_hash_algorithm_t hash_algorithm)

    :param hash: pointer to an initialized `hashkit_st` struct
    :param hash_algorithm: valid `hashkit_hash_algrothm_t` constant
    :returns: `hashkit_return_t` indicating success or failure

.. function:: hashkit_return_t hashkit_set_custom_distribution_function(hashkit_st *hash, hashkit_hash_fn function, void *context)

    :param hash: pointer to initialized `hashkit_st` struct
    :param function: `hashkit_hash_fn` function pointer to use as distribution hash function for `HASHKIT_HASH_CUSTOM`
    :param context: pointer to an opaque user managed context for the custom distribution hash function

.. function:: hashkit_hash_algorithm_t hashkit_get_distribution_function(const hashkit_st *hash)

    :param hash: pointer to an initialized `hashkit_st` struct
    :returns: `hashkit_hash_algorithm_t` indicating the currently set distribution hash algorithm to use

DESCRIPTION
-----------

These functions are used to set and retrieve the key and distribution hash functions.

RETURN VALUE
------------

`hashkit_set_function`, `hashkit_set_custom_function` and the distribution
equivalents return `hashkit_return_t::HASHKIT_SUCCESS` on success.

`hashkit_get_function` and `hashkit_get_distribution_function` return
`hashkit_hash_algorithm_t` indicating the hash function used.

SEE ALSO
--------

.. only:: man

    :manpage:`libhashkit(3)`
    :manpage:`hashkit_create(3)`
    :manpage:`hashkit_functions(3)`

.. only:: html

    * :doc:`index`
    * :doc:`hashkit_create`
    * :doc:`hashkit_functions`

