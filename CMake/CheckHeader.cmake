include(CheckIncludeFileCXX)

include(SafeString)

# check for header.h and set HAVE_HEADER_H
function(check_header HEADER_PATH)
    safe_string(${HEADER_PATH} HEADER_CONST)
    check_include_file_cxx(${HEADER_PATH} HAVE_${HEADER_CONST})
endfunction(check_header)

