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

.. include:: options/common_set.rst
.. include:: /bin/options/flags_reqarg.rst
.. include:: options/udp.rst

.. option:: -S|--set

    Issue *SET* command(s). This is the default mode.
    See also :option:`-A|--add` and :option:`-R|--replace`.

.. option:: -A|--add

    Issue *ADD* command(s).

.. option:: -R|--replace

    Issue *REPLACE* command(s).

.. option:: -.|--basename

    Use basename of path as key (default).
    
.. option:: -+|--relative

    Use relative path (as passed), instead of basename only.
    
.. option:: -/|--absolute

    Use absolute path (real path), instead of basename only.
    
    
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
