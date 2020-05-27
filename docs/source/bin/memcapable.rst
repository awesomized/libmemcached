memcapable
==========

SYNOPSIS
--------

.. program:: memcapable

memcapable [options]

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

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)`
  :manpage:`libmemcached(3)`

.. only:: html

  * :doc:`/libmemcached`
