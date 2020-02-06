
install(FILES ax_libmemcached.m4
        DESTINATION ${CMAKE_INSTALL_DATADIR}/aclocal
        )

configure_file(example.cnf.in example.cnf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/example.cnf
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        )

configure_file(libmemcached.pc.in libmemcached.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libmemcached.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )

if(BUILD_TESTING)
    set(LIBMEMCACHED_WITH_SASL_PWDB "MEMCACHED_SASL_PWDB=${CMAKE_CURRENT_BINARY_DIR}/memcached.pwdb" PARENT_SCOPE)
    set(LIBMEMCACHED_WITH_SASL_CONF "SASL_CONF_PATH=${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)

    cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)
    configure_file(memcached.pwdb.in memcached.pwdb @ONLY)
    configure_file(memcached.conf.in memcached.conf @ONLY)
endif()
