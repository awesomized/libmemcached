if(CMAKE_HOST_SYSTEM_NAME MATCHES "BSD")
    find_program(PKGCONF pkgconf)
    if(PKGCONF)
        set(PKG_CONFIG_EXECUTABLE ${PKGCONF})
    endif()
endif()

find_package(PkgConfig)
