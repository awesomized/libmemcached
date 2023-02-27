memrm
=====

SYNOPSIS
--------

|client_prefix|\rm [options] <key ...>

.. program:: memrm

Remove key(s) from a collection of memcached servers

DESCRIPTION
-----------

:program:`memrm` removes items, specified by key, from :manpage:`memcached(1)` servers.

OPTIONS
-------

.. include:: options/common_set.rst

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
* :doc:`/libmemcached/memcached_delete`
