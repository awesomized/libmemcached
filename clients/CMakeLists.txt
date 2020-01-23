
set(CLIENTS
        memcapable
        memcat
        memcp
        memdump
        memerror
        memexist
        memflush
        memparse
        memping
        memrm
        memslap
        memstat
        memtouch)

add_library(libclient_utilities STATIC utilities.cc)
add_library(client_utilities ALIAS libclient_utilities)
target_include_directories(libclient_utilities PRIVATE ..)
target_link_libraries(libclient_utilities libmemcachedinternal)

add_library(libclient_slap STATIC generator.cc execute.cc)
add_library(client_slap ALIAS libclient_slap)
target_include_directories(libclient_slap PRIVATE ..)
target_link_libraries(libclient_slap libmemcachedinternal)

foreach(CLIENT IN LISTS CLIENTS)
    add_executable(${CLIENT} ${CLIENT}.cc)
    target_include_directories(${CLIENT} PRIVATE ..)
    target_link_libraries(${CLIENT} libclient_utilities)
endforeach()

# extra sources

target_sources(memcapable PRIVATE ../libmemcached/byteorder.cc)

# extra libs

target_link_libraries(memcapable ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(memping libmemcachedutil)
target_link_libraries(memslap libclient_slap ${CMAKE_THREAD_LIBS_INIT})

# memaslap is special

find_library(LIBEVENT event)
check_stdatomic()

if(LIBEVENT AND HAVE_C_STDATOMIC)
    check_decl(getline stdio.h)
    add_executable(memaslap memaslap.c
            ms_conn.c ms_setting.c ms_sigsegv.c ms_stats.c ms_task.c ms_thread.c)
    target_include_directories(memaslap PRIVATE ..)
    target_link_libraries(memaslap libclient_slap libclient_utilities ${LIBEVENT} ${CMAKE_THREAD_LIBS_INIT})
endif()
