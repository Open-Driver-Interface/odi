#ifndef _ODI_TYPES_H
#define _ODI_TYPES_H

#include <stddef.h>
#include <stdarg.h>

typedef va_list odi_va_list;

#define odi_va_start va_start
#define odi_va_end va_end
#define odi_va_arg va_arg

#ifndef u64
#define u64 unsigned long long
#endif

#ifndef u32
#define u32 unsigned int
#endif

#ifndef u16
#define u16 unsigned short
#endif

#ifndef u8
#define u8 unsigned char
#endif

#ifndef s64
#define s64 long long
#endif

#ifndef s32
#define s32 int
#endif

#ifndef s16
#define s16 short
#endif

#ifndef s8
#define s8 char
#endif

#ifndef f32
#define f32 float
#endif

#ifndef f64
#define f64 double
#endif

#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef NULLPTR
#define NULLPTR 0
#endif

#endif