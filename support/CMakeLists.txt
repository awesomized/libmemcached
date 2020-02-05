
install(FILES ax_libmemcached.m4
        DESTINATION ${CMAKE_INSTALL_DATADIR}/aclocal
        )

configure_file(example.cnf.in example.cnf @ONLY)
install(FILES example.cnf
        DESTINATION ${CMAKE_INSTALL_DATADIR}
        )

configure_file(libmemcached.pc.in libmemcached.pc @ONLY)
install(FILES libmemcached.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
