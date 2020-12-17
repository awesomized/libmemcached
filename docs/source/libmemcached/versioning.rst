libmemcached Versioning
=======================

Libmemcached is laid out by interface version and strives to adhere to the
``Semantic Versioning Specification`` since version ``1.1``.

.. seealso::
    `https://semver.org/ <http://semver.org/>`_

The 1.x version would be found in: ``libmemcached-1/memcached.h``. The historic 
``libmemcached/memcached.h`` includes ``libmemcached-1/memcached.h`` and on
previously supported platforms ``libmemcached-1.0`` points to ``libmemcached-1``. 
