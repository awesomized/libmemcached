
add_library(libclient_utilities STATIC utilities.cc generator.cc execute.cc)
add_library(client_utilities ALIAS libclient_utilities)
target_link_libraries(libclient_utilities PRIVATE libmemcachedinternal)
target_include_directories(libclient_utilities PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})


foreach(CLIENT IN LISTS CLIENTS)
    add_executable(${CLIENT} ${CLIENT}.cc)
    target_include_directories(${CLIENT} PRIVATE ..)
    target_link_libraries(${CLIENT} PRIVATE libclient_utilities)
    if(CMAKE_INSTALL_RPATH)
        set_target_properties(${CLIENT} PROPERTIES
                INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS ${CLIENT}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

# extra sources

target_sources(memcapable PRIVATE ../libmemcached/byteorder.cc)

# extra libs

target_link_libraries(memcapable PRIVATE Threads::Threads)
target_link_libraries(memping PRIVATE libmemcachedutil)
target_link_libraries(memslap PRIVATE Threads::Threads)

# memaslap is special

if(ENABLE_MEMASLAP)
    if(LIBEVENT AND HAVE_C_STDATOMIC)
        add_executable(memaslap memaslap.c
                ms_conn.c ms_setting.c ms_sigsegv.c ms_stats.c ms_task.c ms_thread.c)
        target_include_directories(memaslap PRIVATE ${LIBEVENT_INCLUDEDIR})
        target_link_libraries(memaslap PRIVATE libclient_utilities ${LIBEVENT_LIBRARIES} Threads::Threads)
        if(CMAKE_INSTALL_RPATH)
            set_target_properties(${CLIENT} PROPERTIES
                    INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
        endif()
        install(TARGETS memaslap
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()
