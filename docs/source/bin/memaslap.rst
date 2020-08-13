==================================================
memaslap - Load testing  and benchmarking a server
==================================================

SYNOPSIS
--------

memaslap [options]

.. program:: memaslap

.. option:: --help

.. envvar:: MEMCACHED_SERVERS

DESCRIPTION
-----------

:program:`memaslap` is a load generation and benchmark tool for memcached
servers. It generates configurable workload such as threads, concurrency,
connections, run time, overwrite, miss rate, key size, value size, get/set 
proportion, expected throughput, and so on. Furthermore, it also tests data
verification, expire-time verification, UDP, binary protocol, facebook test,
replication test, multi-get and reconnection, etc.

Memaslap manages network connections like memcached with
libevent. Each thread of memaslap is bound with a CPU core, all
the threads don't communicate with each other, and there are several socket
connections in each thread. Each connection keeps key size distribution,
value size distribution, and command distribution by itself.

You can specify servers via the :option:`memslap --servers` option or via the
environment variable :envvar:`MEMCACHED_SERVERS`.

FEATURES
--------

Memslap is developed to for the following purposes:

Manages network connections with libevent asynchronously.

Set both TCP and UDP up to use non-blocking IO.

Improves parallelism: higher performance in multi-threads environments.

Improves time efficiency: faster processing speed.

Generates key and value more efficiently; key size distribution and value size distribution are configurable.

Supports get, multi-get, and set commands; command distribution is configurable.

Supports controllable miss rate and overwrite rate.

Supports data and expire-time verification.

Supports dumping statistic information periodically.

Supports thousands of TCP connections.

Supports binary protocol.

Supports facebook test (set with TCP and multi-get with UDP) and replication test.

DETAILS
-------

Effective implementation of network.
____________________________________

For memaslap, both TCP and UDP use non-blocking network IO. All
the network events are managed by libevent as memcached. The network module
of memaslap is similar to memcached. Libevent can ensure
memaslap can handle network very efficiently.

Effective implementation of multi-threads and concurrency
_________________________________________________________

Memslap has the similar implementation of multi-threads to
memcached. Memslap creates one or more self-governed threads;
each thread is bound with one CPU core if the system tests setting CPU
core affinity.

In addition, each thread has a libevent to manage the events of the network;
each thread has one or more self-governed concurrency; and each
concurrency has one or more socket connections. All the concurrent tasks don't
communicate with each other even though they are in the same thread.

Memslap can create thousands of socket connections, and each
concurrency has tens of socket connections. Each concurrency randomly or
sequentially selects one socket connection from its socket connection pool
to run, so memaslap can ensure each concurrency handles one
socket connection at any given time. Users can specify the number of
concurrency and socket connections of each concurrency according to their
expected workload.

Effective implementation of generating key and value
____________________________________________________

In order to improve time efficiency and space efficiency,
memaslap creates a random characters table with 10M characters. All the
suffixes of keys and values are generated from this random characters table.

Memslap uses the offset in the character table and the length
of the string to identify a string. It can save much memory.
Each key contains two parts, a prefix and a suffix. The prefix is an
uint64_t, 8 bytes. In order to verify the data set before, 
memaslap need to ensure each key is unique, so it uses the prefix to identify
a key. The prefix cannot include illegal characters, such as '\r', '\n',
'\0' and ' '. And memaslap has an algorithm to ensure that.

Memslap doesn't generate all the objects (key-value pairs) at
the beginning. It only generates enough objects to fill the task window
(default 10K objects) of each concurrency. Each object has the following
basic information, key prefix, key suffix offset in the character table, key
length, value offset in the character table, and value length.

In the work process, each concurrency sequentially or randomly selects an
object from the window to do set operation or get operation. At the same
time, each concurrency kicks objects out of its window and adds new object
into it.

Simple but useful task scheduling
_________________________________

Memslap uses libevent to schedule all concurrent tasks of
threads, and each concurrency schedules tasks based on the local task
window. Memslap assumes that if each concurrency keeps the same
key distribution, value distribution and commands distribution, from
outside, memaslap keeps all the distribution as a whole. 
Each task window includes a lot of objects, each object stores its basic
information, such as key, value, expire time, and so on. At any time, all
the objects in the window keep the same and fixed key and value
distribution. If an object is overwritten, the value of the object will be
updated. Memslap verifies the data or expire-time according to
the object information stored in the task window.

Libevent selects which concurrency to handle based on a specific network
event. Then the concurrency selects which command (get or set) to operate
based on the command distribution. If it needs to kick out an old object and
add a new object, in order to keep the same key and value distribution, the
new object must have the same key length and value length.

If memcached server has two cache layers (memory and SSD), running
memaslap with different window sizes can get different cache
miss rates. If memaslap adds enough objects into the windows at
the beginning, and the cache of memcached cannot store all the objects
initialized, then memaslap will get some objects from the second
cache layer. It causes the first cache layer to miss. So the user can
specify the window size to get the expected miss rate of the first cache
layer.

Useful implementation of multi-servers , UDP, TCP, multi-get and binary protocol
________________________________________________________________________________

Because each thread is self-governed, memaslap can assign
different threads to handle different memcached servers. This is just one of
the ways in which memaslap tests multiple servers. The only
limitation is that the number of servers cannot be greater than the number
of threads. The other way to test multiple servers is for replication
test. Each concurrency has one socket connection to each memcached server.
For the implementation, memaslap can set some objects to one
memcached server, and get these objects from the other servers.

By default, Memslap does single get. If the user specifies
multi-get option, memaslap will collect enough get commands and
pack and send the commands together.

Memslap tests both the ASCII protocol and binary protocol,
but it runs on the ASCII protocol by default. 
Memslap by default runs on the TCP protocol, but it also
tests UDP. Because UDP is unreliable, dropped packages and out-of-order
packages may occur. Memslap creates a memory buffer to handle
these problems. Memslap tries to read all the response data of
one command from the server and reorders the response data. If some packages
get lost, the waiting timeout mechanism can ensure half-baked packages will
be discarded and the next command will be sent.

USAGE
-----

Below are some usage samples:

memaslap -s 127.0.0.1:11211 -S 5s

memaslap -s 127.0.0.1:11211 -t 2m -v 0.2 -e 0.05 -b

memaslap -s 127.0.0.1:11211 -F config -t 2m -w 40k -S 20s -o 0.2

memaslap -s 127.0.0.1:11211 -F config -t 2m -T 4 -c 128 -d 20 -P 40k

memaslap -s 127.0.0.1:11211 -F config -t 2m -d 50 -a -n 40

memaslap -s 127.0.0.1:11211,127.0.0.1:11212 -F config -t 2m

memaslap -s 127.0.0.1:11211,127.0.0.1:11212 -F config -t 2m -p 2

The user must specify one server at least to run memaslap. The
rest of the parameters have default values, as shown below:

Thread number = 1                     Concurrency = 16

Run time = 600 seconds                Configuration file = NULL

Key size = 64                         Value size = 1024

Get/set = 9:1                         Window size = 10k

Execute number = 0                    Single get = true

Multi-get = false                     Number of sockets of each concurrency = 1

Reconnect = false                     Data verification = false

Expire-time verification = false      ASCII protocol = true

Binary protocol = false               Dumping statistic information periodically = false

Overwrite proportion = 0%             UDP = false

TCP = true                            Limit throughput = false

Facebook test = false                 Replication test = false

Key size, value size and command distribution.
______________________________________________

All the distributions are read from the configuration file specified by user
with "—cfg_cmd" option. If the user does not specify a configuration file,
memaslap will run with the default distribution (key size = 64,
value size = 1024, get/set = 9:1). For information on how to edit the
configuration file, refer to the "Configuration File" section.

The minimum key size is 16 bytes; the maximum key size is 250 bytes. The
precision of proportion is 0.001. The proportion of distribution will be
rounded to 3 decimal places.

The minimum value size is 1 bytes; the maximum value size is 1M bytes. The
precision of proportion is 0.001. The proportion of distribution will be
rounded to 3 decimal places.
Currently, memaslap only tests set and get commands. And it
testss 100% set and 100% get. For 100% get, it will preset some objects to
the server.

Multi-thread and concurrency
____________________________

The high performance of memaslap benefits from the special
schedule of thread and concurrency. It's important to specify the proper
number of them. The default number of threads is 1; the default number of
concurrency is 16. The user can use "—threads" and "--concurrency" to
specify these variables.

If the system tests setting CPU affinity and the number of threads
specified by the user is greater than 1, memaslap will try to
bind each thread to a different CPU core. So if you want to get the best
performance memaslap, it is better to specify the number of
thread equal to the number of CPU cores. The number of threads specified by
the user can also be less or greater than the number of CPU cores. Because
of the limitation of implementation, the number of concurrencies could be
the multiple of the number of threads.

1. For 8 CPU cores system

For example:

--threads=2 --concurrency=128

--threads=8 --concurrency=128

--threads=8 --concurrency=256

--threads=12 --concurrency=144

2. For 16 CPU cores system

For example:

--threads=8 --concurrency=128

--threads=16 --concurrency=256

--threads=16 --concurrency=512

--threads=24 --concurrency=288

The memaslap performs very well, when
used to test the performance of memcached servers.
Most of the time, the bottleneck is the network or
the server. If for some reason the user wants to
limit the performance of memaslap, there
are two ways to do this:

Decrease the number of threads and concurrencies.
Use the option "--tps" that memaslap
provides to limit the throughput. This option allows
the user to get the expected throughput. For
example, assume that the maximum throughput is 50
kops/s for a specific configuration, you can specify
the throughput equal to or less than the maximum
throughput using "--tps" option.

Window size
___________

Most of the time, the user does not need to specify the window size. The
default window size is 10k. For Schooner Memcached, the user can specify
different window sizes to get different cache miss rates based on the test
case. Memslap testss cache miss rate between 0% and 100%.
If you use this utility to test the performance of Schooner Memcached, you
can specify a proper window size to get the expected cache miss rate. The
formula for calculating window size is as follows:

Assume that the key size is 128 bytes, and the value size is 2048 bytes, and
concurrency=128.

1. Small cache cache_size=1M, 100% cache miss (all data get from SSD).
win_size=10k

2. cache_size=4G

(1). cache miss rate 0%

win_size=8k

(2). cache miss rate 5%

win_size=11k

3. cache_size=16G

(1). cache miss rate 0%

win_size=32k

(2). cache miss

rate 5%

win_size=46k

The formula for calculating window size for cache miss rate 0%:

cache_size / concurrency / (key_size + value_size) \* 0.5

The formula for calculating window size for cache miss rate 5%:

cache_size / concurrency / (key_size + value_size) \* 0.7

Verification
____________

Memslap testss both data verification and expire-time
verification. The user can use "--verify=" or "-v" to specify the proportion
of data verification. In theory, it testss 100% data verification. The
user can use "--exp_verify=" or "-e" to specify the proportion of
expire-time verification. In theory, it testss 100% expire-time
verification. Specify the "--verbose" options to get more detailed error
information.

For example: --exp_verify=0.01 –verify=0.1 , it means that 1% of the objects
set with expire-time, 10% of the objects gotten will be verified. If the
objects are gotten, memaslap will verify the expire-time and
value.

multi-servers and multi-config
_______________________________

Memslap testss multi-servers based on self-governed thread.
There is a limitation that the number of servers cannot be greater than the
number of threads. Memslap assigns one thread to handle one
server at least. The user can use the "--servers=" or "-s" option to specify
multi-servers.

For example:

--servers=10.1.1.1:11211,10.1.1.2:11212,10.1.1.3:11213 --threads=6 --concurrency=36

The above command means that there are 6 threads, with each thread having 6
concurrencies and that threads 0 and 3 handle server 0 (10.1.1.1); threads 1
and 4 handle server 1 (10.1.1.2); and thread 2 and 5 handle server 2
(10.1.1.3).

All the threads and concurrencies in memaslap are self-governed.

So is memaslap. The user can start up several
memaslap instances. The user can run memaslap on different client
machines to communicate with the same memcached server at the same. It is
recommended that the user start different memaslap on different
machines using the same configuration.

Run with execute number mode or time mode
_________________________________________

The default memaslap runs with time mode. The default run time
is 10 minutes. If it times out, memaslap will exit. Do not
specify both execute number mode and time mode at the same time; just
specify one instead.

For example:

--time=30s (It means the test will run 30 seconds.)

--execute_number=100000 (It means that after running 100000 commands, the test will exit.)

Dump statistic information periodically.
________________________________________

The user can use "--stat_freq=" or "-S" to specify the frequency.

For example:

--stat_freq=20s

Memslap will dump the statistics of the commands (get and set) at the frequency of every 20
seconds.

For more information on the format of dumping statistic information, refer to "Format of Output" section.

Multi-get
_________

The user can use "--division=" or "-d" to specify multi-get keys count.
Memslap by default does single get with TCP. Memslap also testss data 
verification and expire-time verification for multi-get.

Memslap testss multi-get with both TCP and UDP. Because of
the different implementation of the ASCII protocol and binary protocol,
there are some differences between the two. For the ASCII protocol,
memaslap sends one "multi-get" to the server once. For the
binary protocol, memaslap sends several single get commands
together as "multi-get" to the server.

UDP and TCP
___________

Memslap testss both UDP and TCP. For TCP,
memaslap does not reconnect the memcached server if socket connections are
lost. If all the socket connections are lost or memcached server crashes,
memaslap will exit. If the user specifies the "--reconnect"
option when socket connections are lost, it will reconnect them.

User can use "--udp" to enable the UDP feature, but UDP comes with some
limitations:

UDP cannot set data more than 1400 bytes.

UDP is not tested by the binary protocol because the binary protocol of
memcached does not tests that.

UDP doesn't tests reconnection.

Facebook test
_____________

Set data with TCP and multi-get with UDP. Specify the following options:

"--facebook --division=50"

If you want to create thousands of TCP connections, specify the

"--conn_sock=" option.

For example: --facebook --division=50 --conn_sock=200

The above command means that memaslap will do facebook test,
each concurrency has 200 socket TCP connections and one UDP socket.

Memslap sets objects with the TCP socket, and multi-gets 50
objects once with the UDP socket.

If you specify "--division=50", the key size must be less that 25 bytes
because the UDP packet size is 1400 bytes.

Replication test
________________

For replication test, the user must specify at least two memcached servers.
The user can use "—rep_write=" option to enable feature.

For example:

--servers=10.1.1.1:11211,10.1.1.2:11212 –rep_write=2

The above command means that there are 2 replication memcached servers,
memaslap will set objects to both server 0 and server 1, get
objects which are set to server 0 before from server 1, and also get objects
which are set to server 1 before from server 0. If server 0 crashes,
memaslap will only get objects from server 1. If server 0 comes
back to life again, memaslap will reconnect server 0. If both
server 0 and server 1 crash, memaslap will exit.

Supports thousands of TCP connections
_____________________________________

Start memaslap with "--conn_sock=" or "-n" to enable this
feature. Make sure that your system can tests opening thousands of files
and creating thousands of sockets. However, this feature does not tests
reconnection if sockets disconnect.

For example:

--threads=8 --concurrency=128 --conn_sock=128

The above command means that memaslap starts up 8 threads, each
thread has 16 concurrencies, each concurrency has 128 TCP socket
connections, and the total number of TCP socket connections is 128 \* 128 =
16384.

Supports binary protocol
________________________

Start memaslap with "--binary" or "-B" options to enable this
feature. It testss all the above features except UDP, because the latest
memcached 1.3.3 does not implement binary UDP protocol.

For example:

--binary

Since memcached 1.3.3 doesn't implement binary UDP protocol,
memaslap does not tests UDP. In addition, memcached 1.3.3 does not tests
multi-get. If you specify "--division=50" option, it just sends 50 get
commands together as "multi-get" to the server.

Configuration file
------------------

This section describes the format of the configuration file.  By default
when no configuration file is specified memaslap reads the default
one located at ~/.memaslap.cnf.

Below is a sample configuration file:

.. code-block:: perl

  ---------------------------------------------------------------------------
  #comments should start with '#'
  #key 
  #start_len end_len proportion
  #
  #key length range from start_len to end_len
  #start_len must be equal to or greater than 16
  #end_len must be equal to or less than 250
  #start_len must be equal to or greater than end_len
  #memaslap will generate keys according to the key range
  #proportion: indicates keys generated from one range accounts for the total
  generated keys  
  #
  #example1: key range 16~100 accounts for 80%
  #          key range 101~200 accounts for 10%
  #          key range 201~250 accounts for 10%
  #          total should be 1 (0.8+0.1+0.1 = 1)
  #
  #          16 100 0.8  
  #          101 200 0.1
  #          201 249 0.1
  #
  #example2: all keys length are 128 bytes
  #
  #          128 128 1 
  key
  128 128 1  
  #value 
  #start_len end_len proportion
  #
  #value length range from start_len to end_len
  #start_len must be equal to or greater than 1
  #end_len must be equal to or less than 1M
  #start_len must be equal to or greater than end_len
  #memaslap will generate values according to the value range
  #proportion: indicates values generated from one range accounts for the
  total generated values  
  #
  #example1: value range 1~1000 accounts for 80%
  #          value range 1001~10000 accounts for 10%
  #          value range 10001~100000 accounts for 10%
  #          total should be 1 (0.8+0.1+0.1 = 1)
  #
  #          1 1000 0.8  
  #          1001 10000 0.1
  #          10001 100000 0.1
  #
  #example2: all value length are 128 bytes
  #
  #          128 128 1 
  value
  2048 2048 1
  #cmd
  #cmd_type cmd_proportion
  #
  #currently memaslap only testss get and set command.
  #
  #cmd_type
  #set     0
  #get     1
  #
  #example: set command accounts for 50%
  #         get command accounts for 50%
  #         total should be 1 (0.5+0.5 = 1)
  #
  #         cmd
  #         0    0.5
  #         1    0.5
  cmd
  0    0.1
  1.0 0.9

Format of output
----------------

At the beginning, memaslap displays some configuration information as follows:

servers : 127.0.0.1:11211

threads count: 1

concurrency: 16

run time: 20s

windows size: 10k

set proportion: set_prop=0.10

get proportion: get_prop=0.90

Where
_____

servers : "servers"
 
 The servers used by memaslap.

threads count
 
 The number of threads memaslap runs with.

concurrency
 
 The number of concurrencies memaslap runs with.

run time
 
 How long to run memaslap.

windows size
 
 The task window size of each concurrency.

set proportion
 
 The proportion of set command.

get proportion
 
 The proportion of get command.

The output of dynamic statistics is something like this:

.. code-block:: perl

  ---------------------------------------------------------------------------------------------------------------------------------
  Get Statistics
  Type  Time(s)  Ops   TPS(ops/s)  Net(M/s)  Get_miss  Min(us)  Max(us)
  Avg(us)  Std_dev    Geo_dist  
  Period   5   345826  69165     65.3      0         27      2198     203
  95.43      177.29
  Global  20  1257935  62896     71.8      0         26      3791     224
  117.79     192.60
  Set Statistics

  Type  Time(s)  Ops   TPS(ops/s)  Net(M/s)  Get_miss  Min(us)  Max(us)
  Avg(us)  Std_dev    Geo_dist  
  Period   5    38425   7685      7.3       0         42      628     240
  88.05      220.21
  Global   20   139780  6989      8.0       0         37      3790    253
  117.93     224.83
  Total Statistics

  Type  Time(s)  Ops   TPS(ops/s)  Net(M/s)  Get_miss  Min(us)  Max(us)
  Avg(us)  Std_dev    Geo_dist 
  Period   5   384252   76850     72.5      0        27      2198     207
  94.72      181.18
  Global  20  1397720   69886     79.7      0        26      3791     227
  117.93     195.60
  ---------------------------------------------------------------------------------------------------------------------------------

Where
_____

Get Statistics
 
 Statistics information of get command

Set Statistics
 
 Statistics information of set command

Total Statistics
 
 Statistics information of both get and set command

Period
 
 Result within a period

Global
 
 Accumulated results

Ops
 
 Total operations

TPS
 
 Throughput, operations/second

Net
 
 The rate of network

Get_miss
 
 How many objects can't be gotten

Min
 
 The minimum response time

Max
 
 The maximum response time

Avg:
 
 The average response time

Std_dev
 
 Standard deviation of response time

Geo_dist
 
 Geometric distribution based on natural exponential function

At the end, memaslap will output something like this:

.. code-block:: perl

   ---------------------------------------------------------------------------------------------------------------------------------
   Get Statistics (1257956 events)
     Min:        26
     Max:      3791
     Avg:       224
     Geo:    192.60
     Std:    116.23
                     Log2 Dist:
                       4:        0       10    84490   215345
                       8:   484890   459823    12543      824
                      12:       31
 
    Set Statistics (139782 events)
       Min:        37
       Max:      3790
       Avg:       253
       Geo:    224.84
       Std:    116.83
       Log2 Dist: 
         4:        0        0     4200 16988
         8:    50784    65574 2064      167
         12:        5
    
     Total Statistics (1397738 events)
         Min:        26
         Max:      3791
         Avg:       227
         Geo:    195.60
         Std:    116.60
         Log2 Dist:
           4:        0       10    88690   232333
           8:   535674   525397    14607      991
           12:       36
 
   cmd_get: 1257969
   cmd_set: 139785
   get_misses: 0
   verify_misses: 0
   verify_failed: 0
   expired_get: 0
   unexpired_unget: 0
   written_bytes: 242516030
   read_bytes: 1003702556
   object_bytes: 152086080
   packet_disorder: 0
   packet_drop: 0
   udp_timeout: 0
 
   Run time: 20.0s Ops: 1397754 TPS: 69817 Net_rate: 59.4M/s
   ---------------------------------------------------------------------------------------------------------------------------------

Where
_____

Get Statistics
 
 Get statistics of response time

Set Statistics
 
 Set statistics of response time

Total Statistics
 
 Both get and set statistics of response time

Min
 
 The accumulated and minimum response time

Max
 
 The accumulated and maximum response time

Avg
 
 The accumulated and average response time

Std
 
 Standard deviation of response time

Log2 Dist
 
 Geometric distribution based on logarithm 2

cmd_get
 
 Total get commands done

cmd_set
 
 Total set commands done

get_misses
 
 How many objects can't be gotten from server

verify_misses
 
 How many objects need to verify but can't get them

verify_failed
 
 How many objects with insistent value

expired_get
 
 How many objects are expired but we get them

unexpired_unget
 
 How many objects are unexpired but we can't get them

written_bytes
 
 Total written bytes

read_bytes
 
 Total read bytes

object_bytes
 
 Total object bytes

packet_disorder
 
 How many UDP packages are disorder

packet_drop
 
 How many UDP packages are lost

udp_timeout
 
 How many times UDP time out happen

Run time
 
 Total run time

Ops
 
 Total operations

TPS
 
 Throughput, operations/second

Net_rate
 
 The average rate of network

OPTIONS
-------

-s, --servers=
    List one or more servers to connect. Servers count must be less than
    threads count. e.g.: --servers=localhost:1234,localhost:11211

-T, --threads=
    Number of threads to startup, better equal to CPU numbers. Default 8.

-c, --concurrency=
    Number of concurrency to simulate with load. Default 128.

-n, --conn_sock=
    Number of TCP socks per concurrency. Default 1.

-x, --execute_number=
    Number of operations(get and set) to execute for the
    given test. Default 1000000.

-t, --time=
    How long the test to run, suffix: s-seconds, m-minutes, h-hours,
    d-days e.g.: --time=2h.

-F, --cfg_cmd=
    Load the configure file to get command,key and value distribution list.

-w, --win_size=
    Task window size of each concurrency, suffix: K, M e.g.: --win_size=10k.
    Default 10k.

-X, --fixed_size=
    Fixed length of value.

-v, --verify=
    The proportion of date verification, e.g.: --verify=0.01

-d, --division=
    Number of keys to multi-get once. Default 1, means single get.

-S, --stat_freq=
    Frequency of dumping statistic information. suffix: s-seconds,
    m-minutes, e.g.: --resp_freq=10s.

-e, --exp_verify=
    The proportion of objects with expire time, e.g.: --exp_verify=0.01.
    Default no object with expire time

-o, --overwrite=
    The proportion of objects need overwrite, e.g.: --overwrite=0.01.
    Default never overwrite object.

-R, --reconnect
    Reconnect tests, when connection is closed it will be reconnected.

-U, --udp
    UDP tests, default memaslap uses TCP, TCP port and UDP port of
    server must be same.

-a, --facebook
    Whether it enables facebook test feature, set with TCP and multi-get with UDP.

-B, --binary
    Whether it enables binary protocol. Default with ASCII protocol.

-P, --tps=
    Expected throughput, suffix: K, e.g.: --tps=10k.

-p, --rep_write=
    The first nth servers can write data, e.g.: --rep_write=2.

-b, --verbose
    Whether it outputs detailed information when verification fails.

-h, --help
    Display this message and then exit.

-V, --version
    Display the version of the application and then exit.

EXAMPLES
--------

memaslap -s 127.0.0.1:11211 -S 5s

memaslap -s 127.0.0.1:11211 -t 2m -v 0.2 -e 0.05 -b

memaslap -s 127.0.0.1:11211 -F config -t 2m -w 40k -S 20s -o 0.2

memaslap -s 127.0.0.1:11211 -F config -t 2m -T 4 -c 128 -d 20 -P 40k

memaslap -s 127.0.0.1:11211 -F config -t 2m -d 50 -a -n 40

memaslap -s 127.0.0.1:11211,127.0.0.1:11212 -F config -t 2m

memaslap -s 127.0.0.1:11211,127.0.0.1:11212 -F config -t 2m -p 2

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)` :manpage:`libmemcached(3)`
