if(ENABLE_MEMASLAP AND CMAKE_USE_PTHREADS_INIT)

    include(CheckAtomics)

    check_type(cpu_set_t sched.h)
    check_symbol(getline stdio.h)
    check_symbol(_SC_NPROCESSORS_ONLN unistd.h)

    check_dependency(LIBEVENT event)

    if(HAVE_LIBEVENT AND HAVE_ATOMICS)
        add_executable(aslap
                ms_main.c
                ms_conn.c
                ms_setting.c
                ms_sigsegv.c
                ms_stats.c
                ms_task.c
                ms_thread.c)
        target_include_directories(aslap PRIVATE
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_BINARY_DIR}/include
                ${CMAKE_SOURCE_DIR}/src
                ${CMAKE_BINARY_DIR}/src
                ${CMAKE_BINARY_DIR})
        target_link_libraries(aslap PRIVATE libmemcached Threads::Threads ${LIBEVENT} m)
        set_property(TARGET aslap PROPERTY C_STANDARD 11 OUTPUT_NAME ${CLIENT_PREFIX}aslap)
        if(CMAKE_INSTALL_RPATH)
            set_target_properties(aslap PROPERTIES
                    INSTALL_RPATH ${CMAKE_INSTALL_RPATH}/../${CMAKE_INSTALL_LIBDIR})
        endif()
        install(TARGETS aslap
                RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()

endif()
