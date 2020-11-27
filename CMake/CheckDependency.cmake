find_package(PkgConfig)

function(check_dependency NAME LIB)
    make_have_identifier(${NAME} HAVE)
    configure_define(${HAVE})

    if(PKG_CONFIG_FOUND)
        pkg_check_modules(${NAME} lib${LIB}${ARGN} IMPORTED_TARGET)
        if(NOT ${NAME}_FOUND)
            pkg_check_modules(${NAME} ${LIB}${ARGN} IMPORTED_TARGET)
        endif()
        if(${NAME}_FOUND)
            set(${NAME} PkgConfig::${NAME} CACHE INTERNAL "${NAME} import target")
            set(${HAVE} 1 CACHE INTERNAL "${HAVE}")
            return()
        endif()
    endif()

    message(STATUS "Checking for library '${LIB}' ...")
    find_library(${NAME}_FOUND NAMES ${LIB} ${ARGN})
    if(${NAME}_FOUND)
        mark_as_advanced(${NAME}_FOUND)
        message(STATUS "  Found '${${NAME}_FOUND}'")

        set(${NAME}_INCLUDES "")
        foreach(PATH IN_LIST CMAKE_PREFIX_PATHS)
            if(LIB${LIB} MATCHES "^${PATH}")
                set(${NAME}_INCLUDES "${PATH}/include")
                break()
            endif()
        endforeach()

        add_library(Imported::${NAME} INTERFACE IMPORTED)
        set_target_properties(Imported::${NAME} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${${NAME}_INCLUDES}"
                INTERFACE_LINK_LIBRARIES ${${NAME}_FOUND})

        set(${NAME} Imported::${NAME} CACHE INTERNAL "${NAME} import target")
        set(${HAVE} 1 CACHE INTERNAL "${HAVE}")
        return()
    endif()

    message(STATUS "  Not found")
endfunction()
