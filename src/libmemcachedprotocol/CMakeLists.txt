
add_library(libmemcachedprotocol SHARED
        ascii_handler.c
        binary_handler.c
        cache.c
        common.h
        handler.c
        pedantic.c
        )
add_library(memcachedprotocol ALIAS libmemcachedprotocol)
set_target_properties(libmemcachedprotocol PROPERTIES LIBRARY_OUTPUT_NAME memcachedprotocol)
target_compile_definitions(libmemcachedprotocol PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcachedprotocol libmemcached Threads::Threads)

set_target_properties(libmemcachedprotocol PROPERTIES SOVERSION ${LIBMEMCACHEDPROTOCOL_SO_VERSION})
install(TARGETS libmemcachedprotocol EXPORT libmemcachedprotocol
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcachedprotocol)
install(EXPORT libmemcachedprotocol DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
