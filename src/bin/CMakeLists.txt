
add_library(libclient_utilities STATIC utilities.cc generator.cc execute.cc)
add_library(client_utilities ALIAS libclient_utilities)
target_link_libraries(libclient_utilities libmemcachedinternal)

foreach(CLIENT IN LISTS CLIENTS)
    add_executable(${CLIENT} ${CLIENT}.cc)
    target_include_directories(${CLIENT} PRIVATE ..)
    target_link_libraries(${CLIENT} libclient_utilities)
    install(TARGETS ${CLIENT}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

# extra sources

target_sources(memcapable PRIVATE ../libmemcached/byteorder.cc)

# extra libs

target_link_libraries(memcapable Threads::Threads)
target_link_libraries(memping libmemcachedutil)
target_link_libraries(memslap Threads::Threads)

# memaslap is special

if(ENABLE_MEMASLAP)
    if(LIBEVENT AND HAVE_C_STDATOMIC)
        add_executable(memaslap memaslap.c
                ms_conn.c ms_setting.c ms_sigsegv.c ms_stats.c ms_task.c ms_thread.c)
        target_include_directories(memaslap PRIVATE ${LIBEVENT_INCLUDEDIR})
        target_link_libraries(memaslap libclient_utilities ${LIBEVENT_LIBRARIES} Threads::Threads)
        install(TARGETS memaslap
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()
