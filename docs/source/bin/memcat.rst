memcat
======

SYNOPSIS
--------

.. program:: memcat

|client_prefix|\cat [options] key [key...]

Read and output the value of one key or the values of a set of keys.

DESCRIPTION
-----------

:program:`memcat` reads and outputs the value of a single or a set of keys
stored in a :manpage:`memcached(1)` server.

If any key is not found an error is returned.

It is similar to the standard UNIX :manpage:`cat(1)` utility.

OPTIONS
-------

.. include:: options/common_get.rst
.. include:: options/flags_noarg.rst
.. include:: options/hash.rst
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
  :manpage:`libmemcached_configuration(3)`

.. only:: html

  * :doc:`/libmemcached`
  * :doc:`/libmemcached/configuration`
