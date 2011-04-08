=======
memslap
=======


memslap - Load testing and benchmarking tool for memcached


--------
SYNOPSIS
--------


.. program::  memslap [options]


-----------
DESCRIPTION
-----------


\ **memslap**\  is a load generation and benchmark tool for memcached(1)
servers. It generates configurable workload such as threads, concurrencies, connections,
run time, overwrite, miss rate, key size, value size, get/set proportion,
expected throughput, and so on. 

You can specify servers via the \ **--servers**\  option or via the
environment variable \ ``MEMCACHED_SERVERS``\ .


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)` :manpage:`memcached_strerror(3)`
