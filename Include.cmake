
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

function(set_test_target TARGET2)
    foreach(TEST IN LISTS TESTS)
        add_custom_command(TARGET ${TARGET2} POST_BUILD COMMAND ${TEST})
    endforeach()
endfunction()

include(CheckTypeSize)
include(CheckCSourceRuns)
include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)

function(safe_string STRING OUTPUT)
    string(REGEX REPLACE "[^0-9a-zA-Z_]" "_" HEADER_SAFE ${STRING})
    string(TOUPPER "${HEADER_SAFE}" HEADER_DEFN)
    set(${OUTPUT} ${HEADER_DEFN} PARENT_SCOPE)
endfunction(safe_string)

function(define_cpp DEF)
    safe_string(${DEF} CNST)
    add_compile_definitions(HAVE_${CNST}=1)
endfunction(define_cpp)

function(check_header HEADER_PATH)
    safe_string(${HEADER_PATH} HEADER_CONST)
    check_include_file_cxx(${HEADER_PATH} HAVE_${HEADER_CONST})
endfunction(check_header)

function(check_decl DECL HEADER)
    safe_string(${DECL} DECL_CONST)
    check_cxx_symbol_exists(${DECL} ${HEADER} HAVE_${DECL_CONST})
endfunction(check_decl)

function(check_type TYPE HEADER)
    safe_string(${TYPE} TYPE_CONST)
    SET(CMAKE_EXTRA_INCLUDE_FILES ${HEADER})
    check_type_size(${TYPE} ${TYPE_CONST} LANGUAGE CXX)
    SET(CMAKE_EXTRA_INCLUDE_FILES)
endfunction(check_type)

function(check_debug)
    if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
        add_compile_definitions(DEBUG=1)
        add_compile_options(-Wall -Wextra)
    endif()
endfunction(check_debug)

function(check_stdatomic)
    check_cxx_source_runs(
            "#include <atomic>
            int main() {
                std::atomic<int> i(0);
                return atomic_fetch_add(&i,1);
            }"
            HAVE_CXX_STDATOMIC)
    check_c_source_runs(
            "#include <stdatomic.h>
            int main() {
                atomic_int i;
                atomic_init(&i, 0);
                return atomic_fetch_add(&i,1);
            }"
            HAVE_C_STDATOMIC)
endfunction(check_atomic_builtins)
