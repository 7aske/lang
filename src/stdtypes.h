//
// Created by nik on 1/27/22.
//
/**
 * stdtypes.h header contains all standardized numeric types that should be
 * used across the program source code. This is a replacement and extension of
 * the standard stdint.h header.
 */

#ifndef LANG_STDTYPES_H
#define LANG_STDTYPES_H

// @Incomplete handle 32bit?

// Readable booleans.
#define true  1
#define false 0

// @formatter:off

// Standard unsigned types.
typedef unsigned char          bool;
typedef unsigned char          u8;
typedef unsigned short int     u16;
typedef unsigned long int      u32;
typedef unsigned long long int u64;

// Standard signed types.
typedef signed char          s8;
typedef signed short int     s16;
typedef signed long int      s32;
typedef signed long long int s64;

// Standard floating point types.
typedef float       float32;
typedef double      float64;
typedef long double float128;

#include <limits.h>
// Maximum representable number for each of the integer types.
#define S8_MAX  (0xff / 2)
#define S16_MAX (0xffff / 2)
#define S32_MAX (0xffffffff / 2)
#define S64_MAX (0xffffffffffffffff / 2)

#define U8_MAX  (0xff)
#define U16_MAX (0xffff)
#define U32_MAX (0xffffffff)
#define U64_MAX (0xffffffffffffffff)

// Minimum representable number for each of the integer types.
#define S8_MIN  (-0xff / 2 - 1)
#define S16_MIN (-0xffff / 2 - 1)
#define S32_MIN (-0xffffffff / 2 - 1)
#define S64_MIN (-0xffffffffffffffff / 2 - 1)

#define U8_MIN  (0x0)
#define U16_MIN (0x0)
#define U32_MIN (0x0)
#define U64_MIN (0x0)

//@formatter:on

#endif //LANG_STDTYPES_H
