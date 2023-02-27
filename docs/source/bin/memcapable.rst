memcapable
==========

SYNOPSIS
--------

.. program:: memcapable

|client_prefix|\capable [options]

Check a memcached server's capabilities and compatibility.

DESCRIPTION
-----------

:program:`memcapable` connects to the specified memcached server and tries to
determine its capabilities by running various commands and verifying the response.

OPTIONS
-------

.. option:: -h hostname

    Specify the hostname to connect to. The default is \ *localhost*\ .

.. option:: -p port

    Specify the port number to connect to. The default is \ *11211*\ .

.. option:: -c

    :manpage:`abort(3)` when detecting an error from the server.

.. option:: -v

    Print out the comparison when it detects an error from the server.

.. option:: -t n

    Set the timeout for an IO operation to/from the server to \ *n*\  seconds.


NOTES
-----

.. include:: common/note_program_prefix.rst
.. include:: common/note_contrib_options.rst


SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)`
  :manpage:`libmemcached(3)`

.. only:: html

  * :doc:`/libmemcached`
