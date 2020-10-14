include(CheckHeader)
include(CheckDecl)
include(CheckCompiles)

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
function(check_byteswap)

    check_compiles(HAVE_BUILTIN_BSWAP32 "uint32_t a, b = __builtin_bswap32(a);" stdint.h)

    if(HAVE_BUILTIN_BSWAP32)
        unset(BYTESWAP_HEADER PARENT_SCOPE)
        set(BYTESWAP_32 __builtin_bswap32 PARENT_SCOPE)
        set(HAVE_BYTESWAP 1 PARENT_SCOPE)
        return()
    endif()

    check_header(byteswap.h)
    check_decl(bswap_32 byteswap.h)

    if(HAVE_BSWAP_32)
        if(HAVE_BYTESWAP_H)
            set(BYTESWAP_HEADER byteswap.h PARENT_SCOPE)
        endif()
        set(BYTESWAP_32 bswap_32 PARENT_SCOPE)
        set(HAVE_BYTESWAP 1 PARENT_SCOPE)
        return()
    endif()

    check_header(sys/endian.h)
    check_decl(bswap32 sys/endian.h)

    if(HAVE_BSWAP32)
        if(HAVE_SYS_ENDIAN_H)
            set(BYTESWAP_HEADER sys/endian.h PARENT_SCOPE)
        endif()
        set(BYTESWAP_32 bswap32 PARENT_SCOPE)
        set(HAVE_BYTESWAP 1 PARENT_SCOPT)
        return()
    endif()

    unset(BYTESWAP_HEADER PARENT_SCOPE)
    unset(BYTESWAP_32 PARENT_SCOPE)
    set(HAVE_BYTESWAP 0 PARENT_SCOPE)
endfunction()
