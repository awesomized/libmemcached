include_directories(..)

add_library(utilities STATIC utilities.cc)
link_libraries(utilities memcachedinternal)

add_executable(memcapable   memcapable.cc ../libmemcached/byteorder.cc)
add_executable(memcat       memcat.cc)
add_executable(memcp        memcp.cc)
add_executable(memdump      memdump.cc)
add_executable(memerror     memerror.cc)
add_executable(memexist     memexist.cc)
add_executable(memflush     memflush.cc)
add_executable(memparse     memparse.cc)
add_executable(memping      memping.cc)
add_executable(memrm        memrm.cc)
add_executable(memslap      memslap.cc generator.cc execute.cc)
add_executable(memstat      memstat.cc)
add_executable(memtouch     memtouch.cc)

target_link_libraries(memcapable ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(memping memcachedutil)
target_link_libraries(memslap ${CMAKE_THREAD_LIBS_INIT})

find_library(LIBEVENT event)

if(LIBEVENT)
    add_executable(memaslap
            memaslap.c generator.cc execute.cc ms_conn.c ms_setting.c ms_sigsegv.c ms_stats.c ms_task.c ms_thread.c)
    target_link_libraries(memaslap event)
endif()
