
add_subdirectory(libhashkit-1)
add_subdirectory(libmemcached-1)
add_subdirectory(libmemcachedutil-1)
add_subdirectory(libmemcachedprotocol-0)

# v1.0 compatibility
if(NOT WIN32)
    file(COPY libhashkit-1.0 DESTINATION .)
    file(COPY libmemcached-1.0 DESTINATION .)
    file(COPY libmemcachedutil-1.0 DESTINATION .)
    file(COPY libmemcachedprotocol-0.0 DESTINATION .)
    install(CODE "
        file(INSTALL
                ${CMAKE_CURRENT_BINARY_DIR}/libhashkit-1.0
                ${CMAKE_CURRENT_BINARY_DIR}/libmemcached-1.0
                ${CMAKE_CURRENT_BINARY_DIR}/libmemcachedutil-1.0
                ${CMAKE_CURRENT_BINARY_DIR}/libmemcachedprotocol-0.0
            DESTINATION \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
    ")
endif()
