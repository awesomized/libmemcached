memdump
=======

SYNOPSIS
--------

.. program:: memdump

|client_prefix|\dump [options]

Dump a list of keys from a server.

DESCRIPTION
-----------

:program:`memdump`  dumps a list of "keys" from all servers that
it is told to fetch from. Because memcached does not guarantee to
provide all keys it is not possible to get a complete "dump".

OPTIONS
-------

.. include:: options/common_get.rst
.. include:: options/file_out.rst


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
    * :doc:`/libmemcached/memcached_dump`
