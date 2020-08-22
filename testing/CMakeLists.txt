
add_executable(catch_main
        main.cpp

        lib/random_.cpp

        lib/Cluster.cpp
        lib/Poll.cpp
        lib/Server.cpp
        lib/WaitForConn.cpp
        lib/WaitForExec.cpp

        server.cpp
        )

set_target_properties(catch_main PROPERTIES
        CXX_STANDARD 17)
