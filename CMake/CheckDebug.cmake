
function(set_flag FLAG DEFAULT)
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
        make_have_identifier(${LIB} HAVE)
        cmake_push_check_state(RESET)
        set(CMAKE_REQUIRED_LIBRARIES ${LIB})
        check_cxx_compiler_flag(-fsanitize=${NAME} ${HAVE})
        cmake_pop_check_state()
        if(${HAVE})
            add_compile_definitions(${HAVE})
            add_compile_options(-fsanitize=${NAME})
            link_libraries(-fsanitize=${NAME})
            set_flag(-fsanitize-recover=${NAME} IGNORE)
            message(STATUS "  OK: sanitizer ${NAME}")
        else()
            message(STATUS "  NO: not supported")
        endif()
    else()
        message(STATUS "  NO: not requested")
    endif()
endmacro()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    add_definitions(-DDEBUG=1)
    if(CMAKE_CXX_FLAGS MATCHES --coverage)
        message("-- Coverage build detected!")
        message("--   Skipping debug and sanitizer flag checks.")
    else()
        set_flag(-Og -O0)
        set_flag(-ggdb -g)
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
            set_flag(${FLAG} IGNORE)
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
