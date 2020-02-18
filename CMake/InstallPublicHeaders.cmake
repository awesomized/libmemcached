function(install_public_headers DIRECTORY)
    foreach(HEADER IN LISTS ARGN)
        file(READ ${HEADER} HEADER_IN)
        string(REGEX REPLACE "include *\"([^\"]+)\"" "include <\\1>" HEADER_OUT "${HEADER_IN}")
        file(GENERATE
                OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${HEADER}
                CONTENT "${HEADER_OUT}"
                )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${HEADER}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DIRECTORY}
                )
    endforeach()
endfunction()
