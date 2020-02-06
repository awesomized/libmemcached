
configure_file(configure.h.in configure.h @ONLY)

install(DIRECTORY ../libhashkit-1.0
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING REGEX "\\.h(pp)?$"
        )
