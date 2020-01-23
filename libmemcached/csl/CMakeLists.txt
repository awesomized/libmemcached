
find_package(FLEX)
find_package(BISON)

flex_target(CSL_SCANNER scanner.l ${CMAKE_CURRENT_SOURCE_DIR}/scanner.cc
        DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/scanner.h
        )
bison_target(CSL_PARSER parser.yy ${CMAKE_CURRENT_SOURCE_DIR}/parser.cc
        DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/parser.h
        )
add_flex_bison_dependency(CSL_SCANNER CSL_PARSER)

set(CSL_SOURCES
        csl/context.cc
        ${BISON_CSL_PARSER_OUTPUTS}
        ${FLEX_CSL_SCANNER_OUTPUTS}
    PARENT_SCOPE)
