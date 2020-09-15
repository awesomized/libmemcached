
# globals
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(PkgConfig)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# locals
include(InstallPublicHeaders)
include(SafeString)
include(CheckDebug)
include(CheckDecl)
include(CheckDependency)
include(CheckHeader)
include(CheckCompiles)
include(CheckType)
include(CheckStdatomic)

# configuration

## debug
check_debug()

## memaslap
if(ENABLE_MEMASLAP)
    check_stdatomic()
    check_dependency(LIBEVENT event event.h)
    check_decl(getline stdio.h)
endif()

## dtrace
if(ENABLE_DTRACE)
    find_package(DTrace)
    if(DTRACE_EXECUTABLE)
        set(HAVE_DTRACE 1)
    else()
        message(WARNING "The dtrace command is required to enable dtrace/systemtap support.")
    endif()
endif()

## uuid
if(BUILD_TESTING)
    check_dependency(LIBUUID uuid uuid/uuid.h)
endif()

## sasl
if(ENABLE_SASL)
    check_dependency(LIBSASL sasl2 sasl/sasl.h)
    if(LIBSASL)
        set(LIBMEMCACHED_WITH_SASL_SUPPORT 1)
    endif()
endif()

## hashes
if(ENABLE_HASH_FNV64)
    set(HAVE_FNV64_HASH 1)
endif()
if(ENABLE_HASH_MURMUR)
    set(HAVE_MURMUR_HASH 1)
endif()
if(ENABLE_HASH_HSIEH)
    set(HAVE_HSIEH_HASH 1)
endif()

# system checks

check_header(arpa/inet.h)
check_header(dlfcn.h)
check_header(errno.h)
check_header(execinfo.h)
check_header(fcntl.h)
check_header(io.h)
check_header(limits.h)
check_header(netdb.h)
check_header(poll.h)
check_header(stddef.h)
check_header(stdlib.h)
check_header(strings.h)
check_header(sys/socket.h)
check_header(sys/time.h)
check_header(sys/types.h)
check_header(sys/un.h)
check_header(sys/wait.h)
check_header(time.h)
check_header(umem.h)
check_header(unistd.h)
check_header(winsock2.h)
check_header(ws2tcpip.h)

check_decl(fcntl fcntl.h)
check_decl(htonll arpa/inet.h)
check_decl(MSG_DONTWAIT sys/socket.h)
check_decl(MSG_MORE sys/socket.h)
check_decl(MSG_NOSIGNAL sys/socket.h)
check_decl(rcvtimeo sys/socket.h)
check_decl(sndtimeo sys/socket.h)
check_decl(strerror string.h)
check_decl(strerror_r string.h)
check_compiles(HAVE_STRERROR_R_CHAR_P "char x, y = *strerror_r(0,&x,1);" string.h)
check_decl(abi::__cxa_demangle cxxabi.h)
set(HAVE_GCC_ABI_DEMANGLE ${HAVE_ABI____CXA_DEMANGLE})

check_type(in_port_t netinet/in.h)

check_header(cstdint)
check_header(cinttypes)
