#ifndef CLIENTS_MS_ATOMIC_H
#define CLIENTS_MS_ATOMIC_H

#if defined(__SUNC)

#define _KERNEL
#include <atomic.h>
#undef _KERNEL

inline int32_t __sync_add_and_fetch(volatile int32_t* ptr, int32_t val)
{
  (val == 1) ? atomic_inc_32((volatile uint32_t*)ptr) : atomic_add_32((volatile uint32_t*)ptr, val);
  return *ptr;
}


inline uint32_t __sync_sub_and_fetch(volatile uint32_t* ptr, uint32_t val)
{
  (val == 1) ? atomic_dec_32(ptr) : atomic_add_32(ptr, 0-(int32_t)val);
  return *ptr;
}

#endif /* defined(__SUNC) */
#endif /* CLIENTS_MS_ATOMIC_H */
