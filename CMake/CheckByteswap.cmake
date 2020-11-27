# defines HAVE_BYTESWAP
# optionally defines BYTESWAP_HEADER
# optionally defines BYTESWAP_32
#
# checks whether the following compiles:
# __builtin_bswap32(): defines HAVE_BUILTIN_BSWAP32
#
# else checks:
# byteswap.h: defines HAVE_BYTESWAP_H
# bswap_32() in byteswap.h: defines HAVE_BSWAP_32
#
# else checks:
# sys/endian.h: defines HAVE_SYS_ENDIAN_H
# bswap32() in sys/endian.h: defines HAVE_BSWAP32
#

include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)
configure_define(WORDS_BIGENDIAN)

configure_define(HAVE_BYTESWAP)
configure_define_header(BYTESWAP_HEADER)
configure_define_literal(BYTESWAP_32)

check_c_source("
        #include <stdint.h>
        int main() {
            uint32_t a = 1, b = __builtin_bswap32(a);
            return b;
        }"
        HAVE_BUILTIN_BSWAP32
)

if(HAVE_BUILTIN_BSWAP32)
    configure_undef(BYTESWAP_HEADER)
    set(BYTESWAP_32 __builtin_bswap32 CACHE INTERNAL "BYTESWAP_32")
    set(HAVE_BYTESWAP 1 CACHE INTERNAL "HAVE_BYTESWAP")
    return()
endif()

check_include(byteswap.h)
check_symbol(bswap_32 byteswap.h)

if(HAVE_BSWAP_32)
    if(HAVE_BYTESWAP_H)
        set(BYTESWAP_HEADER byteswap.h CACHE INTERNAL "BYTESWAP_HEADER")
    endif()
    set(BYTESWAP_32 bswap_32 CACHE INTERNAL "BYTESWAP_32")
    set(HAVE_BYTESWAP 1 CACHE INSTERNAL "HAVE_BYTESWAP")
    return()
endif()

check_include(sys/endian.h)
check_symbol(bswap32 sys/endian.h)

if(HAVE_BSWAP32)
    if(HAVE_SYS_ENDIAN_H)
        set(BYTESWAP_HEADER sys/endian.h CACHE INTERNAL "BYTESWAP_HEADER")
    endif()
    set(BYTESWAP_32 bswap32 CACHE INTERNAL "BYTESWAP_32")
    set(HAVE_BYTESWAP 1 CACHE INTERNAL "HAVE_BYTESWAP")
    return()
endif()

configure_undef(BYTESWAP_HEADER)
configure_undef(BYTESWAP_32)
set(HAVE_BYTESWAP 0)
