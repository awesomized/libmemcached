
# replace any non-alnum characters with underscore and uppercase the result
function(safe_string STRING OUTPUT)
    string(REGEX REPLACE "[^0-9a-zA-Z_]" "_" HEADER_SAFE ${STRING})
    string(TOUPPER "${HEADER_SAFE}" HEADER_DEFN)
    set(${OUTPUT} ${HEADER_DEFN} PARENT_SCOPE)
endfunction(safe_string)

