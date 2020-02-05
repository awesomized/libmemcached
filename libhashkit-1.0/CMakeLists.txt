
configure_file(configure.h.in configure.h @ONLY)

install(DIRECTORY ../libhashkit-1.0
        TYPE INCLUDE
        FILES_MATCHING REGEX "\\.h(pp)?$"
        )
