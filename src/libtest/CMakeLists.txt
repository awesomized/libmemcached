
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp_chroot)
file(WRITE ${CMAKE_BINARY_DIR}/libtool
"#!/bin/bash
shift
exec $@
")
if(UNIX)
    if(EXISTS ${CMAKE_BINARY_DIR}/libtool)
        execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/libtool)
    endif()
endif()

add_library(libtest STATIC
        alarm.cc
        binaries.cc
        client.cc
        cmdline.cc
        collection.cc
        comparison.cc
        core.cc
        cpu.cc
        dns.cc
        dream.cc
        drizzled.cc
        exception.cc
        exception/fatal.cc
        formatter.cc
        framework.cc
        gearmand.cc
        has.cc
        http.cc
        is_local.cc
        killpid.cc
        libtool.cc
        main.cc
        memcached.cc
        port.cc
        result.cc
        runner.cc
        server.cc
        server_container.cc
        signal.cc
        socket.cc
        strerror.cc
        timer.cc
        tmpfile.cc
        vchar.cc
        )
set_target_properties(libtest PROPERTIES LIBRARY_OUTPUT_NAME test)
target_compile_definitions(libtest PRIVATE
        BUILDING_LIBTEST=1
        LIBTEST_TEMP=\"${CMAKE_BINARY_DIR}/tmp_chroot\"

        DRIZZLED_BINARY=\"drizzled\"
        GEARMAND_BINARY=\"gearmand\"
        MEMCACHED_BINARY=\"${MEMCACHED_BINARY}\"
        HAVE_MEMCACHED_BINARY=1
        )
target_link_libraries(libtest PRIVATE Threads::Threads ${CMAKE_DL_LIBS})

add_executable(wait wait.cc dream.cc)

add_executable(core_count core_count.cc cpu.cc)

add_executable(backtrace backtrace_test.cc ../libmemcached/backtrace.cc)
target_link_libraries(backtrace PRIVATE ${CMAKE_DL_LIBS})

configure_file(yatlcon.h.in yatlcon.h @ONLY)
configure_file(version.h.in version.h @ONLY)
