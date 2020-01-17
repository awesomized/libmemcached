include_directories(..)

add_library(memcachedinternal
        ../libmemcached/allocators.cc
        ../libmemcached/analyze.cc
        ../libmemcached/array.c
        ../libmemcached/auto.cc
        ../libmemcached/backtrace.cc
        ../libmemcached/behavior.cc
        ../libmemcached/byteorder.cc
        ../libmemcached/callback.cc
        ../libmemcached/connect.cc
        ../libmemcached/delete.cc
        ../libmemcached/do.cc
        ../libmemcached/dump.cc
        ../libmemcached/encoding_key.cc
        ../libmemcached/error.cc
        ../libmemcached/exist.cc
        ../libmemcached/fetch.cc
        ../libmemcached/flag.cc
        ../libmemcached/flush.cc
        ../libmemcached/flush_buffers.cc
        ../libmemcached/get.cc
        ../libmemcached/hash.cc
        ../libmemcached/hosts.cc
        ../libmemcached/initialize_query.cc
        ../libmemcached/instance.cc
        ../libmemcached/io.cc
        ../libmemcached/key.cc
        ../libmemcached/memcached.cc
        ../libmemcached/namespace.cc
        ../libmemcached/options.cc
        ../libmemcached/parse.cc
        ../libmemcached/poll.cc
        ../libmemcached/purge.cc
        ../libmemcached/quit.cc
        ../libmemcached/response.cc
        ../libmemcached/result.cc
        ../libmemcached/sasl.cc
        ../libmemcached/server.cc
        ../libmemcached/server_list.cc
        ../libmemcached/stats.cc
        ../libmemcached/storage.cc
        ../libmemcached/strerror.cc
        ../libmemcached/string.cc
        ../libmemcached/touch.cc
        ../libmemcached/udp.cc
        ../libmemcached/verbosity.cc
        ../libmemcached/version.cc
        ../libmemcached/virtual_bucket.c
        )

target_link_libraries(memcachedinternal csl hashkit ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

target_compile_definitions(memcachedinternal PRIVATE -DBUILDING_LIBMEMCACHEDINTERNAL)
