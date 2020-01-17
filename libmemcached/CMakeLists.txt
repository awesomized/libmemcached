
add_subdirectory(csl)

include_directories(..)

add_library(memcached
        allocators.cc
        analyze.cc
        array.c
        auto.cc
        backtrace.cc
        behavior.cc
        byteorder.cc
        callback.cc
        connect.cc
        delete.cc
        do.cc
        dump.cc
        encoding_key.cc
        error.cc
        exist.cc
        fetch.cc
        flag.cc
        flush.cc
        flush_buffers.cc
        get.cc
        hash.cc
        hosts.cc
        initialize_query.cc
        instance.cc
        io.cc
        key.cc
        memcached.cc
        namespace.cc
        options.cc
        parse.cc
        poll.cc
        purge.cc
        quit.cc
        response.cc
        result.cc
        sasl.cc
        server.cc
        server_list.cc
        stats.cc
        storage.cc
        strerror.cc
        string.cc
        touch.cc
        udp.cc
        verbosity.cc
        version.cc
        virtual_bucket.c
        )

target_link_libraries(memcached csl hashkit ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

target_compile_definitions(memcached PRIVATE -DBUILDING_LIBMEMCACHED)
