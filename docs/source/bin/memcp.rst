memcp
=====

SYNOPSIS
--------

.. program:: memcp

memcp [options] \-\-servers <hostname[:port]...> <file...>

Copy files to a collection of memcached servers.

DESCRIPTION
-----------

:program:`memcp` copies one or more files into :manpage:`memcached(1)` servers.
It is similar to the standard UNIX :manpage:`cp(1)` command.

The key names will be the names of the files, without any directory path.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/expire.rst
.. include:: options/flag.rst
.. include:: options/hash.rst
.. include:: options/sasl.rst
.. include:: options/udp.rst

.. option:: --buffer

    Enable internal buffering of commands.

.. option:: --set

    Issue *SET* command(s). This is the default mode.
    See also :option:`--add` and :option:`--replace`.

.. option:: --add

    Issue *ADD* command(s).

.. option:: --replace

    Issue *REPLACE* command(s).

ENVIRONMENT
-----------

.. envvar:: MEMCACHED_SERVERS

    Specify the list of servers.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_behavior(3)`

.. only:: html

    * :doc:`/libmemcached`
    * :doc:`/libmemcached/configuration`
    * :doc:`/libmemcached/memcached_behavior`
