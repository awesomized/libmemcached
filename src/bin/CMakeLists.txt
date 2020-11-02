add_subdirectory(common)

foreach(CLIENT IN LISTS CLIENTS)
    add_executable(${CLIENT} ${CLIENT}.cc)
    set_target_properties(${CLIENT} PROPERTIES CXX_STANDARD ${CXX_STANDARD})
    target_include_directories(${CLIENT} PRIVATE ..)
    target_link_libraries(${CLIENT} PRIVATE libclient_common)
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

if(HAVE_MEMASLAP)
    add_executable(memaslap memaslap.c
            memaslap/ms_conn.c
            memaslap/ms_setting.c
            memaslap/ms_sigsegv.c
            memaslap/ms_stats.c
            memaslap/ms_task.c
            memaslap/ms_thread.c)
    target_include_directories(memaslap PRIVATE memaslap)
    target_link_libraries(memaslap PRIVATE libclient_common Threads::Threads ${LIBEVENT})
    if(CMAKE_INSTALL_RPATH)
        set_target_properties(${CLIENT} PROPERTIES
                INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS memaslap
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
