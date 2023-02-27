add_subdirectory(common)

foreach(CLIENT IN LISTS CLIENTS)
    add_executable(${CLIENT} mem${CLIENT}.cc)
    set_property(TARGET ${CLIENT} OUTPUT_NAME ${CLIENT_PREFIX}${CLIENT} PROPERTY CXX_STANDARD ${CXX_STANDARD})
    target_include_directories(${CLIENT} PRIVATE ..)
    target_link_libraries(${CLIENT} PRIVATE libclient_common)
    if(CMAKE_INSTALL_RPATH)
        set_target_properties(${CLIENT} PROPERTIES
                INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS ${CLIENT} RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

# extra sources

target_sources(capable PRIVATE ../libmemcached/byteorder.cc)

# extra libs

target_link_libraries(ping PRIVATE libmemcachedutil)
target_link_libraries(slap PRIVATE Threads::Threads)
