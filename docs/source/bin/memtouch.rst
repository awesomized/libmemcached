memtouch
========

SYNOPSIS
--------

.. program:: memtouch

|client_prefix|\touch [options] <key>

DESCRIPTION
-----------

:program:`memtouch` does a "touch" on the specified key.

OPTIONS
-------

.. include:: options/common_get.rst
.. include:: options/hash.rst
.. include:: options/expire.rst

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
* :doc:`/libmemcached/memcached_touch`
