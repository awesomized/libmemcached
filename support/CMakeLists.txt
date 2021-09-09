
install(FILES ax_libmemcached.m4
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_DATADIR}/aclocal
        )

configure_file(example.cnf.in example.cnf @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/example.cnf
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
        )

pkgconfig_import(REQUIRES)
pkgconfig_import(REQUIRES_PRIVATE)
configure_file(libmemcached.pc.in libmemcached.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libmemcached.pc
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )

if(BUILD_TESTING)
    set(LIBMEMCACHED_WITH_SASL_PWDB "${CMAKE_CURRENT_BINARY_DIR}/memcached.pwdb" PARENT_SCOPE)
    set(LIBMEMCACHED_WITH_SASL_CONF "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)

    cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)
    configure_file(memcached.pwdb.in memcached.pwdb @ONLY)
    configure_file(memcached.conf.in memcached.conf @ONLY)
endif()

configure_package_config_file(libmemcached-config.cmake.in
        libmemcached-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
write_basic_package_version_file(libhashkit-version.cmake
        VERSION ${LIBHASHKIT_VERSION}
        COMPATIBILITY SameMajorVersion)
write_basic_package_version_file(libmemcached-version.cmake
        VERSION ${LIBMEMCACHED_VERSION}
        COMPATIBILITY SameMajorVersion)
write_basic_package_version_file(libmemcachedprotocol-version.cmake
        VERSION ${LIBMEMCACHEDPROTOCOL_VERSION}
        COMPATIBILITY SameMajorVersion)
write_basic_package_version_file(libmemcachedutil-version.cmake
        VERSION ${LIBMEMCACHEDUTIL_VERSION}
        COMPATIBILITY SameMajorVersion)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libmemcached-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libhashkit-version.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libmemcached-version.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libmemcachedprotocol-version.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/libmemcachedutil-version.cmake
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/)
