configure_define(HAVE_CPP17_PARALLELISM)

#
# see CheckTbb.cmake on change
#

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
        HAVE_CPP17_PARALLELISM
        )

if(HAVE_CPP17_PARALLELISM)
    add_library(cpp17::parallelism INTERFACE IMPORTED)
    # noting to be done
else()
    include(CheckTbb)
    set(HAVE_CPP17_PARALLELISM "${HAVE_TBB}")
    if(HAVE_TBB)
        add_library(cpp17::parallelism INTERFACE IMPORTED)
        target_link_libraries(cpp17::parallelism INTERFACE "${LIBTBB}")
    endif()
endif()
