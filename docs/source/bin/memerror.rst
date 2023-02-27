memerror
========

SYNOPSIS
--------

.. program:: memerror

|client_prefix|\error [options] <error code>

Translate a memcached error code into a string.

DESCRIPTION
-----------

:program:`memerror` translates an error code from `libmemcached` into a human
readable string.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst


.. include:: common/env.rst

NOTES
-----

.. include:: common/note_program_prefix.rst

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`

.. only:: html

    * :doc:`/libmemcached`
    * :doc:`/libmemcached/index_errors`
