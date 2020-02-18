
configure_file(configure.h.in configure.h @ONLY)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING REGEX "\\.h(pp)?$"
        PATTERN t EXCLUDE
        )
