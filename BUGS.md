# Bugs, Known Issues and Insufficiencies

## libhashkit

libhashkit is not usable for general purpose hashing, because it is geared to
usage by libmemcached.

### MurMur

Hashkit's MurMur/MurMur3 are limited to the lower 32 bits.

### crc32

Commit "[More Hashing methods](https://github.com/m6w6/libmemcached/commits/1207354f)"
from October 2007 first released in v0.8, which main intention seems to have
been to add FNV1 hash algos, changed the result of the crc32 hash to only its
upper 16 bits sans MSB, without any additional comment.

The implementations referred to in the file header (Postgres and BSD)
do not exhibit this behavior.

A [bug report](https://bugs.launchpad.net/libmemcached/+bug/604178) was
filed three years later on launchpad, which was marked `Won't fix` with
the comment that it was for compatibility with other "drivers", which
supposedly refers to other memcached client libraries.


## libmemcached

### Replication

This is a somewhat badly named feature, because it **does not** provide
any of the guaranties one would expect from a proper replication.

One can set the intended number of additional servers where data should
be stored with the behavior `MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS` and
specify whether `MGET`s/`GET`s should read from a random server with 
`MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ`. `DELETE`s will try to 
delete the key from all replicas.

The binary protocol is required and any other command is unaffected.
