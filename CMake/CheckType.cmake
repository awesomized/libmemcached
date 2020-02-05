include(CheckTypeSize)

include(SafeString)

# check for type (possibly in header.h) and set HAVE_TYPE
function(check_type TYPE HEADER)
    safe_string(${TYPE} TYPE_CONST)
    SET(CMAKE_EXTRA_INCLUDE_FILES ${HEADER})
    check_type_size(${TYPE} ${TYPE_CONST} LANGUAGE CXX)
    SET(CMAKE_EXTRA_INCLUDE_FILES)
endfunction(check_type)
