find_package(Backtrace)
if(Backtrace_FOUND)
    configure_set(HAVE_BACKTRACE 1)
    configure_define_header(Backtrace_HEADER)
    set(BACKTRACE BACKTRACE)
    add_library(BACKTRACE INTERFACE IMPORTED)
    set_target_properties(BACKTRACE PROPERTIES
            INTERFACE_LINK_LIBRARIES "${Backtrace_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${Backtrace_INCLUDE_DIR}")
endif()
