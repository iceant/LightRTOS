#ifndef INCLUDED_OS_COMPILER_GCC_H
#define INCLUDED_OS_COMPILER_GCC_H

#ifndef __ALIGNED
#define __ALIGNED(n) __attribute__((aligned(n)))
#endif /**/

#ifndef __PACKED
#define __PACKED __attribute__((packed))
#endif /**/

#ifndef __UNUSED
#define __UNUSED __attribute__((unused))
#endif /**/

#ifndef __USED
#define __USED __attribute__((used))
#endif /**/

#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif /**/

#ifndef   __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif /**/

#ifndef   __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif /**/

#ifndef __PACKED
#define __PACKED __attribute__((packed, aligned(1)))
#endif /**/

#ifndef   __PACKED_STRUCT
#define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
#endif /**/

#ifndef   __PACKED_UNION
#define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
#endif /**/

#ifndef   __RESTRICT
#define __RESTRICT __restrict
#endif /**/

////////////////////////////////////////////////////////////////////////////////
////



#endif /*INCLUDED_OS_COMPILER_GCC_H*/
