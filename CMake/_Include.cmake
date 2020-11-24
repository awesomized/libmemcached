
# globals
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "BSD")
    find_program(PKGCONF pkgconf)
    if(PKGCONF)
        set(PKG_CONFIG_EXECUTABLE ${PKGCONF})
    endif()
endif()
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
include(CheckAtomics)
include(TestBigEndian)
include(CheckByteswap)

# configuration

## debug
check_debug()

## memaslap
if(ENABLE_MEMASLAP)
    check_atomics()
    check_dependency(LIBEVENT event event.h)
    check_decl(getline stdio.h)
    if(HAVE_LIBEVENT AND HAVE_C_STDATOMIC)
        set(HAVE_MEMASLAP 1)
    endif()
endif()

## dtrace
include(EnableDtrace)
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
    if(NOT MEMCACHED_BINARY)
        find_package(Memcached)
        set(MEMCACHED_BINARY ${MEMCACHED_EXECUTABLE})
    endif()
endif()

## sasl
if(ENABLE_SASL)
    check_dependency(LIBSASL sasl2 sasl/sasl.h)
    if(HAVE_LIBSASL)
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

test_big_endian(WORDS_BIGENDIAN)
check_byteswap()

check_header(alloca.h)
check_header(arpa/inet.h)
check_header(dlfcn.h)
check_header(errno.h)
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
check_decl(SO_RCVTIMEO sys/socket.h)
check_decl(SO_SNDTIMEO sys/socket.h)
check_decl(setenv stdlib.h)
check_decl(strerror string.h)
check_decl(strerror_r string.h)

check_compiles(HAVE_STRERROR_R_CHAR_P "char x, y = *strerror_r(0,&x,1);" string.h)

check_decl(abi::__cxa_demangle cxxabi.h)

find_package(Backtrace)
if(Backtrace_FOUND)
    set(HAVE_BACKTRACE 1)
    set(BACKTRACE BACKTRACE)
    add_library(BACKTRACE INTERFACE IMPORTED)
    set_target_properties(BACKTRACE PROPERTIES
            INTERFACE_LINK_LIBRARIES "${Backtrace_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${Backtrace_INCLUDE_DIR}")
endif()

check_type(in_port_t netinet/in.h)

check_header(cstdint)
check_header(cinttypes)
check_header(inttypes.h)
