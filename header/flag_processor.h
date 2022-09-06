#ifndef FLAG_PROCESSOR_H_

#define FLAG_PROCESSOR_H_

/* Includes for the header file */
#include "flag.h"

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef FLAG_PROCESSOR_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef FLAG_PROCESSOR_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#define alloc(type, size) (type) calloc(size, sizeof(type))
#define for_each_array_idx(index, item, array) \
    int index = 0;                             \
    while ((item = array[index]) && item != 0 && ++index)
#define for_each_array(item, array) for_each_array_idx(index, item, array)
#define NULL_FLAG (Flag*)67
#endif

LANGUAGE DLLIMPORT CALLING void printFlagsArray(Flag** flags, int printNull);
LANGUAGE DLLIMPORT CALLING Flag** validateFlags(char** args, char* requiredFlags[][20], char* optionalFlags[][20], int allowUnknownFlags);

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
