find_program(DTRACE_EXECUTABLE NAMES dtrace
        HINTS $ENV{DTRACE_DIR}
        PATH_SUFFIXES bin
        DOC "DTrace static probe generator"
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DTrace DEFAULT_MSG DTRACE_EXECUTABLE)

mark_as_advanced(DTRACE_EXECUTABLE)
