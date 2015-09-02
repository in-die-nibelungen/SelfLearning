#ifndef _TYPES_H_
#define _TYPES_H_

#include <sys/types.h>

typedef int status_t;

typedef unsigned int size_t;

// Will be depricated
typedef char s8;
typedef unsigned char u8;

typedef short int s16;
typedef unsigned short int u16;

typedef long int s32;
typedef unsigned long int u32;

typedef long long int s64;
typedef unsigned long long int u64;

typedef float  f32;
typedef double f64;
//

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 1
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

#define PTR_CAST(t, v) reinterpret_cast<t>(v)

#endif // #ifndef _TYPES_H_
