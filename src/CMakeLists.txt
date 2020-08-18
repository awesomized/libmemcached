
add_subdirectory(bin)
add_subdirectory(libhashkit)
add_subdirectory(libmemcached)
add_subdirectory(libmemcachedutil)
add_subdirectory(libmemcachedprotocol)

if(BUILD_TESTING)
    add_subdirectory(libtest)
endif()
