if(HAVE_HSIEH_HASH)
    set(HSIEH_CC hsieh.cc)
else()
    set(HSIEH_CC nohsieh.cc)
endif()

add_library(libhashkit SHARED
        aes.cc
        algorithm.cc
        behavior.cc
        common.h
        crc32.cc
        digest.cc
        encrypt.cc
        fnv_32.cc
        fnv_64.cc
        function.cc
        has.cc
        hashkit.cc
        ${HSIEH_CC}
        jenkins.cc
        ketama.cc
        md5.cc
        murmur.cc
        murmur3.cc
        murmur3_api.cc
        one_at_a_time.cc
        rijndael.cc
        str_algorithm.cc
        strerror.cc
        string.cc
        )
add_library(hashkit ALIAS libhashkit)
set_target_properties(libhashkit PROPERTIES LIBRARY_OUTPUT_NAME hashkit)
target_compile_options(libhashkit PRIVATE -DBUILDING_HASHKIT)
target_include_directories(libhashkit PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libhashkit PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
configure_file(hashkitcon.h.in hashkitcon.h @ONLY)

set_target_properties(libhashkit PROPERTIES SOVERSION ${LIBHASHKIT_SO_VERSION})
install(TARGETS libhashkit EXPORT libhashkit
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libhashkit)
install(EXPORT libhashkit DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)

install_public_headers(
        libhashkit

        hashkit.h
        )
