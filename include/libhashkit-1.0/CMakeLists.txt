
configure_file(configure.h.in configure.h @ONLY)

install_public_headers(
        libhashkit-1.0

        algorithm.h
        basic_string.h
        behavior.h
        digest.h
        function.h
        has.h
        hashkit.h
        hashkit.hpp
        str_algorithm.h
        strerror.h
        string.h
        types.h
        visibility.h
        )
