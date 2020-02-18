
add_subdirectory(bin)
add_subdirectory(libhashkit)
add_subdirectory(libmemcached)
add_subdirectory(libmemcachedutil)

if(BUILD_TESTING)
    add_subdirectory(libtest)
endif()
