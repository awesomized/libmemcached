configure_define(HAVE_VISIBILITY)

check_flag(-fvisibility=hidden HAVE_VISIBILITY_FLAG)
if(NOT HAVE_VISIBILITY_FLAG)
    check_flag(-Wl,-fvisibility=hidden HAVE_VISIBILITY_LINKER_FLAG)
endif()
check_c_source("
        __attribute__ ((visibility (\"default\")))
        int main(int argc, char **argv) {
            return *argv[argc-1];
        }"
        HAVE_VISIBILITY_ATTR
)

if(HAVE_VISIBILITY_ATTR AND (HAVE_VISIBILITY_FLAG OR HAVE_VISIBILITY_LINKER_FLAG))
    if(HAVE_VISIBILITY_LINKER_FLAG)
        string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,-fvisibility=hidden")
    else()
        add_compile_options("-fvisibility=hidden")
    endif()
    set(HAVE_VISIBILITY 1 CACHE INTERNAL "-fvisibility and __attribute__((visibility(...)))")
endif()
