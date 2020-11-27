# sets HAVE_ATOMICS, checks for :
# - C++11 std::atomic:  HAVE_CXX_STDATOMIC
# - C11 stdatomic:      HAVE_C_STDATOMIC
# - builtin __atomic:   HAVE_BUILTIN_ATOMIC
# - builtin __sync:     HAVE_BUILTIN_SYNC
# - atomic_add_nv:      HAVE_ATOMIC_ADD_NV

configure_define(HAVE_ATOMICS)

check_cxx_source("
        #include <atomic>
        int main() {
            std::atomic<int> i(0);
            return atomic_load(&i);
        }"
        HAVE_CXX_STDATOMIC)

check_c_source("
        #include <stdatomic.h>
        int main() {
            atomic_int i;
            atomic_init(&i, 0);
            return atomic_load(&i);
        }"
        HAVE_C_STDATOMIC)


configure_define(HAVE_BUILTIN_ATOMIC)
configure_define_literal(BUILTIN_ATOMIC_PREFIX)
foreach(BUILTIN_ATOMIC_PREFIX IN ITEMS __c11 _ "")
    check_c_source("
            int main() {
                long l = -1;
                return ${BUILTIN_ATOMIC_PREFIX}_atomic_add_fetch(&l,1,__ATOMIC_RELAXED);
            }"
            HAVE_BUILTIN_ATOMIC${BUILTIN_ATOMIC_PREFIX})
    if (HAVE_BUILTIN_ATOMIC${BUILTIN_ATOMIC_PREFIX})
        set(HAVE_BUILTIN_ATOMIC 1)
        break()
    endif()
endforeach()

check_c_source("
        int main() {
            long l = -1;
            return __sync_add_and_fetch(&l,1);
        }"
        HAVE_BUILTIN_SYNC)

check_c_source("
        #include <atomic.h>
        int main() {
            volatile uint_t i = 0;
            return atomic_add_int_nv(&i, 1) == 1 ? 0 : -1;
        }"
        HAVE_ATOMIC_ADD_NV)


if (        (HAVE_CXX_STDATOMIC)
        OR  (HAVE_C_STDATOMIC)
        OR  (HAVE_BUILTIN_ATOMIC)
        OR  (HAVE_BUILTIN_SYNC)
        OR  (HAVE_ATOMIC_ADD_NV))
    set(HAVE_ATOMICS 1 CACHE INTERNAL "HAVE_ATOMICS")
endif()
