if(HSIEH)
    set(HSIEH_CC hsieh.cc)
else()
    set(HSIEH_CC nohsieh.cc)
endif()

set(AUTOHEADER_FILE mem_config.h)
configure_file(hashkitcon.h.in hashkitcon.h @ONLY)

include_directories(..)

add_library(hashkit
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
target_compile_options(hashkit PRIVATE -DBUILDING_HASHKIT)
