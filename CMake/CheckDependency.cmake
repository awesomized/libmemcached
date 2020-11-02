find_package(PkgConfig)

include(SafeString)
include(CheckHeader)

function(check_dependency_basic NAME LIB HEADER)
	set(FOUND 1)
	message("-- Checking for library '${LIB}' with header '${HEADER}' ...")
	find_library(LIB${LIB} NAMES ${LIB})
	if(NOT LIB${LIB})
		set(FOUND 0)
	endif()
	if(HEADER)
		safe_string(${HEADER} HEADER_CONST)
		check_header(${HEADER})
		if(NOT HAVE_${HEADER_CONST})
			set(FOUND 0)
		else()
			find_path(${HEADER_CONST}_ROOT ${HEADER})
			if(NOT ${HEADER_CONST}_ROOT)
				set(FOUND 0)
			endif()
		endif()
	endif()
	if(FOUND)
		message("--   Found library '${LIB${LIB}}'")
		message("--   Found include '${${HEADER_CONST}_ROOT}'")
        add_library(${NAME} INTERFACE IMPORTED)
        set_target_properties(${NAME} PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${${HEADER_CONST}_ROOT}
                INTERFACE_LINK_LIBRARIES ${LIB${LIB}})
	else()
		message("--   No library '${LIB}' found")
	endif()
	set(${NAME}_FOUND ${FOUND} PARENT_SCOPE)
endfunction()

function(check_dependency NAME LIB HEADER)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(${NAME} lib${LIB}${ARGN} IMPORTED_TARGET)
        if(NOT ${NAME}_FOUND)
            pkg_check_modules(${NAME} ${LIB}${ARGN})
        endif()
        if(${NAME}_FOUND)
            add_library(${NAME} ALIAS PkgConfig::${NAME})
        endif()
    endif()
    if(NOT ${NAME}_FOUND)
        check_dependency_basic(${NAME} ${LIB} ${HEADER})
    endif()

    set(HAVE_${NAME} ${${NAME}_FOUND} PARENT_SCOPE)
    if(${NAME}_FOUND)
        safe_string(${HEADER} HEADER_CONST)
        find_path(HAVE_${HEADER_CONST} ${HEADER} PATHS ${${NAME}_INCLUDEDIR})
    endif()
endfunction()
