
add_executable(internals internals.cc string.cc)
target_link_libraries(internals PRIVATE libtest libmemcachedinternal Threads::Threads)
target_include_directories(internals PRIVATE ../..)
add_test(internals internals)

add_executable(sasl
        basic.cc
        debug.cc
        deprecated.cc
        error_conditions.cc
        ketama.cc
        namespace.cc
        parser.cc
        pool.cc
        print.cc
        replication.cc
        sasl.cc
        virtual_buckets.cc
        callback_counter.cc
        fetch_all_results.cc
        generate.cc
        )
target_link_libraries(sasl PRIVATE
        libclient_utilities
        libhashkit
        libmemcached
        libmemcachedutil
        libtest
        Threads::Threads
        )
target_include_directories(sasl PRIVATE ../..)
add_test(sasl sasl)

add_executable(atomsmasher atomsmasher.cc)
add_executable(testplus plus.cpp)

foreach(TEST IN ITEMS atomsmasher testplus)
    target_sources(${TEST} PRIVATE
        callback_counter.cc
        debug.cc
        fetch_all_results.cc
        generate.cc
        print.cc
        )
    target_link_libraries(${TEST} PRIVATE
            libclient_utilities
            libhashkit
            libmemcached
            libmemcachedutil
            libtest
            )
    target_include_directories(${TEST} PRIVATE ../..)
    add_test(${TEST} ${TEST})
endforeach()


find_library(LIBUUID uuid REQUIRED)

add_executable(testapp all_tests.cc)
add_executable(testsocket all_tests_socket.cc)

foreach(TEST IN ITEMS testapp testsocket)
    target_sources(${TEST} PRIVATE
            basic.cc
            callback_counter.cc
            callbacks.cc
            debug.cc
            deprecated.cc
            dump.cc
            encoding_key.cc
            error_conditions.cc
            exist.cc
            fetch_all_results.cc
            generate.cc
            haldenbrand.cc
            ketama.cc
            mem_functions.cc
            memcached_get.cc
            namespace.cc
            parser.cc
            pool.cc
            print.cc
            replication.cc
            server_add.cc
            setup_and_teardowns.cc
            stat.cc
            touch.cc
            virtual_buckets.cc
            )
    target_link_libraries(${TEST} PRIVATE
            libclient_utilities
            libhashkit
            libmemcached
            libmemcachedutil
            libtest
            Threads::Threads
            ${LIBUUID}
            )
    target_include_directories(${TEST} PRIVATE ../..)
    add_test(${TEST} ${TEST})
endforeach()
