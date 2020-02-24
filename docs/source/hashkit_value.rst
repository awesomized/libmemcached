Generate hash value
===================

.. index:: object: hashkit_st

Generate a value for the given key

SYNOPSIS
--------

#include <libhashkit-|libhashkit_version|/hashkit.h>
  Compile and link with -lhashkit

.. c:function:: uint32_t hashkit_value(hashkit_st *hash, const char *key, size_t key_length)

DESCRIPTION
-----------

The :c:func:`hashkit_value` function generates a 32-bit hash value from the
given key and key_length. The hash argument is an initialized hashkit
object, and distribution type and hash function is used from this
object while generating the value.

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

    * :doc:`libhashkit`
    * :doc:`hashkit_create`
    * :doc:`hashkit_function`
    * :doc:`hashkit_functions`

