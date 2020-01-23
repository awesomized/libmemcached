if(HAVE_HSIEH_HASH)
    set(HSIEH_CC hsieh.cc)
else()
    set(HSIEH_CC nohsieh.cc)
endif()

add_library(libhashkit SHARED
        aes.cc
        algorithm.cc
        behavior.cc
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
        nohsieh.cc
        one_at_a_time.cc
        rijndael.cc
        str_algorithm.cc
        strerror.cc
        string.cc
        )
add_library(hashkit ALIAS libhashkit)
set_target_properties(libhashkit PROPERTIES LIBRARY_OUTPUT_NAME hashkit)
target_include_directories(libhashkit PRIVATE ..)
target_compile_options(libhashkit PRIVATE -DBUILDING_HASHKIT)

set(AUTOHEADER_FILE mem_config.h)
configure_file(hashkitcon.h.in hashkitcon.h @ONLY)
