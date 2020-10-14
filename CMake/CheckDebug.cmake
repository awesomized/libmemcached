include(CMakePushCheckState)
include(CheckCXXCompilerFlag)

function(check_flag FLAG DEFAULT)
    unset(FLAG_CONSTANT)
    string(MAKE_C_IDENTIFIER CXX${FLAG} FLAG_CONSTANT)
    check_cxx_compiler_flag(${FLAG} ${FLAG_CONSTANT})
    if(${FLAG_CONSTANT})
        add_compile_options(${FLAG})
    elseif(DEFAULT)
        add_compile_options(${DEFAULT})
    endif()
endfunction()

function(check_debug)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DDEBUG=1)
        if(CMAKE_CXX_FLAGS MATCHES --coverage)
            message("-- Coverage build detected!")
            message("--   Skipping debug and sanitizer flag checks.")
        else()
            check_flag(-Og -O0)
            check_flag(-ggdb -g)
            foreach(FLAG IN ITEMS
                    -fno-inline
                    -fno-omit-frame-pointer
                    -fno-eliminate-unused-debug-types
                    -funsafe-loop-optimizations

                    -Wall
                    -Wextra

                    -Wdouble-promotion
                    -Wduplicated-cond
                    -Wduplicated-branches
                    -Wformat=2
                    -Wlogical-op
                    -Wnull-dereference
                    -Wrestrict
                    -Wshadow
                    -Wunknown-pragmas
                    -Wunsafe-loop-optimizations
                    )
                check_flag(${FLAG} IGNORE)
            endforeach()

            if(ENABLE_SANITIZERS)
                if(address IN_LIST ENABLE_SANITIZERS OR asan IN_LIST ENABLE_SANITIZERS)
                    cmake_push_check_state(RESET)
                    set(CMAKE_REQUIRED_LIBRARIES asan)
                    check_cxx_compiler_flag(-fsanitize=address HAVE_ASAN)
                    cmake_pop_check_state()
                    if(HAVE_ASAN)
                        add_compile_definitions(HAVE_ASAN)
                        add_compile_options(-fsanitize=address)
                        link_libraries(-fsanitize=address)
                        check_flag(-fsanitize-recover=address IGNORE)
                    endif()
                endif()

                if(undefined IN_LIST ENABLE_SANITIZERS OR ubsan IN_LIST ENABLE_SANITIZERS)
                    cmake_push_check_state(RESET)
                    set(CMAKE_REQUIRED_LIBRARIES ubsan)
                    check_cxx_compiler_flag(-fsanitize=undefined HAVE_UBSAN)
                    cmake_pop_check_state()
                    if(HAVE_UBSAN)
                        add_compile_definitions(HAVE_UBSAN)
                        add_compile_options(-fsanitize=undefined)
                        link_libraries(-fsanitize=undefined)
                        check_flag(-fsanitize-recover=undefined IGNORE)
                    endif()
                endif()
            endif()
        endif()
    else()
        add_definitions(-DDEBUG=0)
    endif()
endfunction()
