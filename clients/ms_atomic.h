/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */

#ifndef CLIENTS_MS_ATOMIC_H
#define CLIENTS_MS_ATOMIC_H

#if defined(__SUNPRO_C)
# define _KERNEL
# include <atomic.h>
# if SIZEOF_SIZE_T == 8
#  define  atomic_add_size(X, Y) atomic_add_64((X), (Y))
#  define  atomic_add_size_nv(X, Y) atomic_add_64((X), (Y))
#  define  atomic_dec_size(X, Y) atomic_add_64((X), (Y))
#  define  atomic_dec_size_nv(X, Y) atomic_add_64((X), (Y))
# else
#  define  atomic_add_size(X, Y) atomic_add_32((X), (Y))
#  define  atomic_add_size_nv(X, Y) atomic_add_32((X), (Y))
#  define  atomic_dec_size(X, Y) atomic_add_32((X), (Y))
#  define  atomic_dec_size_nv(X, Y) atomic_add_32((X), (Y))
# endif
# undef _KERNEL
#elif HAVE_GCC_ATOMIC_BUILTINS
# define atomic_add_8(X, Y)  __sync_fetch_and_add((X), (Y))
# define atomic_add_16(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_add_32(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_add_size(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_dec_8(X)  __sync_fetch_and_sub((X), 1)
# define atomic_dec_16(X) __sync_fetch_and_sub((X), 1)
# define atomic_dec_32(X) __sync_fetch_and_sub((X), 1)
# define atomic_dec_size(X) __sync_fetch_and_sub((X), 1)
/* The same as above, but these return the new value instead of void */
# define atomic_add_8_nv(X, Y)  __sync_fetch_and_add((X), (Y))
# define atomic_add_16_nv(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_add_32_nv(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_add_size_nv(X, Y) __sync_fetch_and_add((X), (Y))
# define atomic_dec_8_nv(X)  __sync_fetch_and_sub((X), 1)
# define atomic_dec_16_nv(X) __sync_fetch_and_sub((X), 1)
# define atomic_dec_32_nv(X) __sync_fetch_and_sub((X), 1)
# define atomic_dec_size_nv(X) __sync_fetch_and_sub((X), 1)
#elif HAVE_C_STDATOMIC
# include <stdatomic.h>
# define atomic_add_8(X, Y) atomic_fetch_add(X, Y)
# define atomic_add_16(X, Y) atomic_fetch_add(X, Y)
# define atomic_add_32(X, Y) atomic_fetch_add(X, Y)
# define atomic_add_size(X, Y) atomic_fetch_add(X, Y)
# define atomic_dec_8(X) atomic_fetch_sub(X, 1)
# define atomic_dec_16(X) atomic_fetch_sub(X, 1)
# define atomic_dec_32(X) atomic_fetch_sub(X, 1)
# define atomic_dec_size(X) atomic_fetch_sub(X, 1)
/* The same as above, but these return the new value instead of void */
# define ATOMIC_ADD_FETCH_DECL(T) \
static inline T atomic_add_fetch_##T(volatile T *ptr, T add) { \
	T des, cur = atomic_load(ptr); \
	do { \
		des = cur + add; \
	} while(!atomic_compare_exchange_weak(ptr, &cur, des)); \
	return des; \
}
# define ATOMIC_SUB_FETCH_DECL(T) \
T atomic_sub_fetch_##T(volatile T *ptr) { \
	T des, cur = atomic_load(ptr); \
	do { \
		des = cur - 1; \
	} while(!atomic_compare_exchange_weak(ptr, &cur, des)); \
	return des; \
}
ATOMIC_ADD_FETCH_DECL(uint8_t)
# define atomic_add_8_nv(X, Y) atomic_add_fetch_uint8_t(X, Y)
ATOMIC_ADD_FETCH_DECL(uint16_t)
# define atomic_add_16_nv(X, Y) atomic_add_fetch_uint16_t(X, Y)
ATOMIC_ADD_FETCH_DECL(uint32_t)
# define atomic_add_32_nv(X, Y) atomic_add_fetch_uint32_t(X, Y)
ATOMIC_ADD_FETCH_DECL(size_t)
# define atomic_add_size_nv(X, Y) atomic_add_fetch_size_t(X, Y)
# define atomic_dec_8_nv(X) atomic_sub_fetch<uint8_t>(X, Y)
# define atomic_dec_16_nv(X) atomic_sub_fetch<uint16_t>(X, Y)
# define atomic_dec_32_nv(X) atomic_sub_fetch<uint32_t>(X, Y)
# define atomic_dec_size_nv(X) atomic_sub_fetch<size_t>(X, Y)
#else
#warning "Atomic operators not found so memslap will not work correctly"
# define atomic_add_8(X, Y) 0
# define atomic_add_16(X, Y) 0
# define atomic_add_32(X, Y) 0
# define atomic_add_size(X, Y) 0
# define atomic_dec_8(X) 0
# define atomic_dec_16(X) 0
# define atomic_dec_32(X) 0
# define atomic_dec_size(X) 0
/* The same as above, but these return the new value instead of void */
# define atomic_add_8_nv(X, Y) 0
# define atomic_add_16_nv(X, Y) 0
# define atomic_add_32_nv(X, Y) 0
# define atomic_add_size_nv(X, Y) 0
# define atomic_dec_8_nv(X) 0
# define atomic_dec_16_nv(X) 0
# define atomic_dec_32_nv(X) 0
# define atomic_dec_size_nv(X) 0
#endif /* defined(__SUNPRO_C) */

#endif /* CLIENTS_MS_ATOMIC_H */
