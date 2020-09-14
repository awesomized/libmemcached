
add_library(libmemcachedprotocol SHARED
        ascii_handler.c
        binary_handler.c
        cache.c
        common.h
        handler.c
        pedantic.c
        )
add_library(memcachedprotocol ALIAS libmemcachedprotocol)
set_target_properties(libmemcachedprotocol PROPERTIES
        LIBRARY_OUTPUT_NAME memcachedprotocol
        INSTALL_RPATH $ORIGIN
        SOVERSION ${LIBMEMCACHEDPROTOCOL_SO_VERSION}
        VERSION v${LIBMEMCACHEDPROTOCOL_VERSION})
target_compile_definitions(libmemcachedprotocol PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcachedprotocol PUBLIC libmemcached Threads::Threads)
target_include_directories(libmemcachedprotocol PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedprotocol PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS libmemcachedprotocol EXPORT libmemcachedprotocol
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcachedprotocol)
install(EXPORT libmemcachedprotocol DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
