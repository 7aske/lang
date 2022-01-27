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
//@formatter:on



#endif //LANG_STDTYPES_H
