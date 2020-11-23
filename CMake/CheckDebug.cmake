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

macro(check_sanitizer VAR NAME LIB)
    message(STATUS "Checking for sanitizer: ${NAME} (-l${LIB})")
    if(${NAME} IN_LIST ${VAR} OR ${LIB} IN_LIST ${VAR})
        safe_string(${LIB} ID)
        cmake_push_check_state(RESET)
        set(CMAKE_REQUIRED_LIBRARIES ${LIB})
        check_cxx_compiler_flag(-fsanitize=${NAME} HAVE_${ID})
        cmake_pop_check_state()
        if(HAVE_${ID})
            add_compile_definitions(HAVE_${ID})
            add_compile_options(-fsanitize=${NAME})
            link_libraries(-fsanitize=${NAME})
            check_flag(-fsanitize-recover=${NAME} IGNORE)
            message(STATUS "  OK: sanitizer ${NAME}")
        else()
            message(STATUS "  NO: not supported")
        endif()
    else()
        message(STATUS "  NO: not requested")
    endif()
endmacro()

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
                check_sanitizer(ENABLE_SANITIZERS address asan)
                check_sanitizer(ENABLE_SANITIZERS undefined ubsan)
                check_sanitizer(ENABLE_SANITIZERS thread tsan)
                check_sanitizer(ENABLE_SANITIZERS leak lsan)
            endif()
        endif()
    else()
        add_definitions(-DDEBUG=0)
    endif()
endfunction()
