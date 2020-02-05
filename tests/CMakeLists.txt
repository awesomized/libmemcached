
add_subdirectory(libmemcached-1.0)

add_executable(cycle cycle.cc)
target_link_libraries(cycle PRIVATE libtest Threads::Threads)
target_include_directories(cycle PRIVATE ..)
add_test(cycle cycle)

add_executable(parser parser.cc)
target_link_libraries(parser PRIVATE libtest libmemcached)
target_include_directories(parser PRIVATE ..)
add_test(parser parser)

add_executable(failure failure.cc)
add_executable(testudp mem_udp.cc)

foreach(TEST IN ITEMS failure testudp)
    target_sources(${TEST} PRIVATE
            libmemcached-1.0/callback_counter.cc
            libmemcached-1.0/fetch_all_results.cc
            libmemcached-1.0/generate.cc
            libmemcached-1.0/print.cc
            )
    target_link_libraries(${TEST} PRIVATE
            libclient_utilities
            libmemcached
            libmemcachedutil
            libtest
            )
    target_include_directories(${TEST} PRIVATE ..)
    add_test(${TEST} ${TEST})
endforeach()

add_executable(testhashkit hashkit_functions.cc)
target_link_libraries(testhashkit PRIVATE libtest libhashkit)
target_include_directories(testhashkit PRIVATE ..)
add_test(testhashkit testhashkit)

add_executable(hash_plus hash_plus.cc)
target_link_libraries(hash_plus PRIVATE libtest libhashkit)
target_include_directories(hash_plus PRIVATE ..)
add_test(testhashplus hash_plus)

foreach(CLIENT IN LISTS CLIENTS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT}.cc)
        add_executable(test${CLIENT} ${CLIENT}.cc)
        target_link_libraries(test${CLIENT} PRIVATE
                libmemcached
                libmemcachedutil
                libtest
                )
        target_include_directories(test${CLIENT} PRIVATE ..)
        add_test(test${CLIENT} test${CLIENT})
    endif()
endforeach()
