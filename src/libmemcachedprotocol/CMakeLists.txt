check_include(umem.h)

set(libmemcachedprotocol_sources
        ascii_handler.c
        binary_handler.c
        cache.c
        common.h
        handler.c
        pedantic.c
        ../libmemcached/byteorder.cc
        )
add_library(libmemcachedprotocol SHARED)
add_library(memcachedprotocol ALIAS libmemcachedprotocol)
set_target_properties(libmemcachedprotocol PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME memcachedprotocol
        LIBRARY_OUTPUT_NAME_DEBUG memcachedprotocol-dbg
        SOVERSION ${LIBMEMCACHEDPROTOCOL_SO_SOVERSION}
        VERSION ${LIBMEMCACHEDPROTOCOL_SO_VERSION})
target_compile_definitions(libmemcachedprotocol PRIVATE -DBUILDING_LIBMEMCACHED)
if(CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
	set_target_properties(libmemcachedprotocol PROPERTIES
		LINK_FLAGS "-Wl,-undefined,dynamic_lookup"
		)
endif()
target_link_libraries(libmemcachedprotocol PRIVATE p9y)
target_link_libraries(libmemcachedprotocol PUBLIC Threads::Threads)
if(MSVC OR MINGW)
    target_link_libraries(libmemcachedprotocol PUBLIC wsock32 ws2_32)
endif()
target_include_directories(libmemcachedprotocol PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedprotocol PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS libmemcachedprotocol EXPORT libmemcachedprotocol-targets
        RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ${TARGET_NAMELINK_COMPONENT}
        )
export(EXPORT libmemcachedprotocol-targets NAMESPACE libmemcached::)
install(EXPORT libmemcachedprotocol-targets NAMESPACE libmemcached::
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

target_sources(libmemcachedprotocol PRIVATE
        ${libmemcachedprotocol_sources}
        ${libmemcachedprotocol_includes}
        )
