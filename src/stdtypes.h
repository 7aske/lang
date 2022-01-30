//
// Created by nik on 1/27/22.
//

#ifndef LANG_STDTYPES_H
#define LANG_STDTYPES_H

// @Incomplete handle 32bit?

#define true  1
#define false 0

//@formatter:off
typedef unsigned char          bool;
typedef unsigned char          u8;
typedef unsigned short int     u16;
typedef unsigned long int      u32;
typedef unsigned long long int u64;

typedef signed char          s8;
typedef signed short int     s16;
typedef signed long int      s32;
typedef signed long long int s64;

typedef float       float32;
typedef double      float64;
typedef long double float128;

#define S8_MAX  (0xff - 1)
#define U8_MAX  (0xff)
#define S16_MAX (0xffff - 1)
#define U16_MAX (0xffff)
#define S32_MAX (0xffffffff - 1)
#define U32_MAX (0xffffffff)
#define S64_MAX (0xffffffffffffffff - 1)
#define U64_MAX (0xffffffffffffffff)

#define S8_MIN  (0xff - 1)
#define U8_MIN  (0xff)
#define S16_MIN (0xffff - 1)
#define U16_MIN (0xffff)
#define S32_MIN (0xffffffff - 1)
#define U32_MIN (0xffffffff)
#define S64_MIN (0xffffffffffffffff - 1)
#define U64_MIN (0xffffffffffffffff)

//@formatter:on



#endif //LANG_STDTYPES_H
