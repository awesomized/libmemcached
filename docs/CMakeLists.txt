find_package(Sphinx)

if(NOT SPHINX_EXECUTABLE)

    message(WARNING "The sphinx-build command is required to build documentation.")

else()

    if(NOT DEFINED SPHINX_OPTIONS)
        set(SPHINX_OPTIONS)
    endif()

    if(NOT DEFINED SPHINX_THEME)
        set(SPHINX_THEME sphinx_rtd_theme)
        set(SPHINX_THEME_OPTIONS "'collapse_navigation':False, 'navigation_depth':2, 'titles_only':False, 'includehidden':False")
    endif()

    set(SPHINX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

    # configured documentation tools and intermediate build results
    set(SPHINX_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    # Sphinx cache with pickled ReST documents
    set(SPHINX_CACHE_DIR "${SPHINX_BUILD_DIR}/_doctrees")

    # HTML output directory
    set(SPHINX_HTML_DIR "${SPHINX_BUILD_DIR}/html")

    # MAN output directory
    set(SPHINX_MAN_DIR "${SPHINX_BUILD_DIR}/man")

    configure_file("conf.py.in" "conf.py" @ONLY)
    add_subdirectory(source)

    file(GLOB_RECURSE DOCS_SOURCE
            CONFIGURE_DEPENDS
            "*.rst")

    if(BUILD_DOCS_HTML)
        add_custom_command(
                OUTPUT "${SPHINX_HTML_DIR}"
                COMMAND
                    ${SPHINX_EXECUTABLE}
                    -q -b html
                    -c "${SPHINX_BUILD_DIR}"
                    -d "${SPHINX_CACHE_DIR}"
                    ${SPHINX_OPTIONS}
                    "${SPHINX_SOURCE_DIR}"
                    "${SPHINX_HTML_DIR}"
                COMMAND
                    ${CMAKE_COMMAND} -E touch "${SPHINX_HTML_DIR}"
                BYPRODUCTS ${SPHINX_HTML_DIR}
                DEPENDS "${DOCS_SOURCE}"
                )
        add_custom_target(html ALL DEPENDS "${SPHINX_HTML_DIR}")

        install(DIRECTORY ${SPHINX_HTML_DIR}/
                COMPONENT doc
                DESTINATION ${CMAKE_INSTALL_DOCDIR}/html)
    endif()

    if(BUILD_DOCS_MAN)
        add_custom_command(
                OUTPUT ${SPHINX_MAN_DIR}
                COMMAND
                    ${SPHINX_EXECUTABLE}
                    -q -b man
                    -c "${SPHINX_BUILD_DIR}"
                    -d "${SPHINX_CACHE_DIR}"
                    ${SPHINX_OPTIONS}
                    "${SPHINX_SOURCE_DIR}"
                    "${SPHINX_MAN_DIR}"
                COMMAND
                    ${CMAKE_COMMAND} -E touch "${SPHINX_MAN_DIR}"
                BYPRODUCTS ${SPHINX_MAN_DIR}
                DEPENDS "${DOCS_SOURCE}"
                )
        add_custom_target(man ALL DEPENDS "${SPHINX_MAN_DIR}")

        set(MAN_EXT "")
        if(BUILD_DOCS_MANGZ)
            find_program(PIGZ pigz)
            if(PIGZ)
                set(GZIP ${PIGZ})
            else()
                find_package(UnixCommands)
            endif()
            if(GZIP)
                set(MAN_EXT ".gz")
                add_custom_target(man_gz ALL
                        COMMAND ${GZIP} -kf
                            ${SPHINX_MAN_DIR}/*.1
                            ${SPHINX_MAN_DIR}/*.3
                        DEPENDS ${SPHINX_MAN_DIR}
                        ) # some shells do not support braced glob patterns
            endif()
        endif()

        install(DIRECTORY ${SPHINX_MAN_DIR}/
                COMPONENT doc
                DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
                FILES_MATCHING PATTERN *.1${MAN_EXT}
                )
        install(DIRECTORY ${SPHINX_MAN_DIR}/
                COMPONENT doc
                DESTINATION ${CMAKE_INSTALL_MANDIR}/man3
                FILES_MATCHING PATTERN *.3${MAN_EXT}
                )
    endif()

endif()
