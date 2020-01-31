find_package(Sphinx)

if(SPHINX_EXECUTABLE)

    if(NOT DEFINED SPHINX_THEME)
        set(SPHINX_THEME sphinx_rtd_theme)
        set(SPHINX_THEME_OPTIONS "'collapse_navigation':False, 'navigation_depth':2, 'titles_only':False, 'includehidden':False")
    endif()

    if(NOT DEFINED SPHINX_THEME_DIR)
        set(SPHINX_THEME_DIR)
    endif()

    if(NOT DEFINED SPHINX_OPTIONS)
        set(SPHINX_OPTIONS)
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

    configure_file(
            "${SPHINX_SOURCE_DIR}/conf.py.in"
            "${SPHINX_BUILD_DIR}/conf.py"
            @ONLY)

    add_custom_target(html
            ${SPHINX_EXECUTABLE}
            -q -a -b html
            -c "${SPHINX_BUILD_DIR}"
            -d "${SPHINX_CACHE_DIR}"
            ${SPHINX_OPTIONS}
            "${SPHINX_SOURCE_DIR}"
            "${SPHINX_HTML_DIR}"
            BYPRODUCTS ${SPHINX_HTML_DIR}
            COMMENT "Build HTML documentation with Sphinx")
    add_custom_target(man
            ${SPHINX_EXECUTABLE}
            -q -a -b man
            -c "${SPHINX_BUILD_DIR}"
            -d "${SPHINX_CACHE_DIR}"
            ${SPHINX_OPTIONS}
            "${SPHINX_SOURCE_DIR}"
            "${SPHINX_MAN_DIR}"
            BYPRODUCTS ${SPHINX_MAN_DIR}
            COMMENT "Build manpage documentation with Sphinx")
endif()
