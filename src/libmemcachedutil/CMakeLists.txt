set(libmemcachedutil_sources
        ../libmemcached/backtrace.cc
        common.h
        flush.cc
        pid.cc
        ping.cc
        version.cc
        )
add_library(libmemcachedutil SHARED)
add_library(memcachedutil ALIAS libmemcachedutil)
if(CMAKE_USE_PTHREADS_INIT)
    target_sources(libmemcachedutil PRIVATE pool.cc)
endif()
set_target_properties(libmemcachedutil PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME memcachedutil
        LIBRARY_OUTPUT_NAME_DEBUG memcachedutil-dbg
        SOVERSION ${LIBMEMCACHEDUTIL_SO_SOVERSION}
        VERSION ${LIBMEMCACHEDUTIL_SO_VERSION})
target_compile_definitions(libmemcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)
if(CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
	set_target_properties(libmemcachedutil PROPERTIES
		LINK_FLAGS "-Wl,-undefined,dynamic_lookup"
		)
endif()
if(MSVC OR MINGW)
    target_link_libraries(libmemcachedutil PUBLIC libmemcached)
endif()
target_link_libraries(libmemcachedutil PUBLIC Threads::Threads)
if(HAVE_LIBSASL)
    target_link_libraries(libmemcachedutil PUBLIC ${LIBSASL})
endif()
target_include_directories(libmemcachedutil PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedutil PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS libmemcachedutil EXPORT libmemcachedutil-targets
        RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ${TARGET_NAMELINK_COMPONENT}
        )
export(EXPORT libmemcachedutil-targets NAMESPACE libmemcached::)
install(EXPORT libmemcachedutil-targets NAMESPACE libmemcached::
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

target_sources(libmemcachedutil PRIVATE
    ${libmemcachedutil_sources}
    ${libmemcachedutil_includes})
