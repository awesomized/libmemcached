find_program(MEMCACHED_EXECUTABLE NAMES memcached
        HINTS $ENV{MEMCACHED_DIR}
        PATH_SUFFIXES bin
        DOC "memcached(), Memcached daemon"
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Memcached DEFAULT_MSG MEMCACHED_EXECUTABLE)

mark_as_advanced(MEMCACHED_EXECUTABLE)
