include(CheckCSourceRuns)
include(CheckCXXSourceRuns)

# check for C11 stdatomic and C++11 std::atomic, set HAVE_C_STDATOMIC and/or HAVE_CXX_STDATOMIC
function(check_stdatomic)
    check_cxx_source_runs(
            "#include <atomic>
            int main() {
                std::atomic<int> i(0);
                return atomic_fetch_add(&i,1);
            }"
            HAVE_CXX_STDATOMIC)
    check_c_source_runs(
            "#include <stdatomic.h>
            int main() {
                atomic_int i;
                atomic_init(&i, 0);
                return atomic_fetch_add(&i,1);
            }"
            HAVE_C_STDATOMIC)
endfunction(    )
