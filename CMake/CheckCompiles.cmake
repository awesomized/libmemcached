include(CheckCXXSourceCompiles)

function(check_compiles RESULT_VAR TEST)

    unset(SOURCE)
    foreach(HEADER IN LISTS ARGN)
        string(APPEND SOURCE "#include <${HEADER}>\n")
    endforeach()
    string(APPEND SOURCE "
            int main() {
                ${TEST}
                return 0;
            }
    ")
    check_cxx_source_compiles("${SOURCE}" ${RESULT_VAR})
endfunction()
