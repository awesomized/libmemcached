
configure_define(HAVE_TBB)

# TBBConfig only sets TBB_FOUND to FALSE
check_dependency(LIBTBB tbb)

if(HAVE_LIBTBB)

    cmake_push_check_state()
    get_property(LIBTBB_INCLUDEDIR TARGET ${LIBTBB} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    get_property(LIBTBB_LIBRARIES TARGET ${LIBTBB} PROPERTY INTERFACE_LINK_LIBRARIES)
    set(CMAKE_REQUIRED_INCLUDES "${LIBTBB_INCLUDEDIR}")
    set(CMAKE_REQUIRED_LIBRARIES "${LIBTBB_LIBRARIES}")
    set(CMAKE_REQUIRED_FLAGS -std=c++17)

    check_cxx_include(execution -std=c++17)
    if(HAVE_EXECUTION)
        check_cxx_source_compiles("
                #include <vector>
                #include <algorithm>
                #include <execution>
                int main() {
                    std::vector<char> a = {1,2,3};
                    std::all_of(std::execution::par, a.begin(), a.end(), [](char i) {
                        return i>0;
                    });
                }
                "
                HAVE_TBB
                )

    endif()
    cmake_pop_check_state()
endif()
