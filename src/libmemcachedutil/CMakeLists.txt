
add_library(libmemcachedutil SHARED
        ../libmemcached/backtrace.cc
        common.h
        flush.cc
        pid.cc
        ping.cc
        version.cc
        )
if(CMAKE_USE_PTHREADS_INIT)
    target_sources(libmemcachedutil PRIVATE pool.cc)
endif()
add_library(memcachedutil ALIAS libmemcachedutil)
set_target_properties(libmemcachedutil PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME memcachedutil
        LIBRARY_OUTPUT_NAME_DEBUG memcachedutil-dbg
        SOVERSION ${LIBMEMCACHEDUTIL_SO_VERSION})
target_compile_definitions(libmemcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)
if(CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
	set_target_properties(libmemcachedutil PROPERTIES
		LINK_FLAGS "-Wl,-undefined,dynamic_lookup"
		)
endif()
if(MSVC)
        set_target_properties(libmemcachedutil PROPERTIES
                LINK_FLAGS "/FORCE:UNRESOLVED"
                )
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
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcachedutil-targets NAMESPACE libmemcached::)
install(EXPORT libmemcachedutil-targets NAMESPACE libmemcached:: DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
