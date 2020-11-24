if(HAVE_MEMASLAP)
    add_executable(memaslap memaslap.c
            memaslap/ms_conn.c
            memaslap/ms_setting.c
            memaslap/ms_sigsegv.c
            memaslap/ms_stats.c
            memaslap/ms_task.c
            memaslap/ms_thread.c)
    target_include_directories(memaslap PRIVATE
            memaslap
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_BINARY_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_BINARY_DIR}/src
            ${CMAKE_BINARY_DIR})
    target_link_libraries(memaslap PUBLIC libmemcached Threads::Threads ${LIBEVENT} m)
    set_property(TARGET memaslap PROPERTY C_STANDARD 11)
    if(CMAKE_INSTALL_RPATH)
        set_target_properties(${CLIENT} PROPERTIES
                INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS memaslap
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
