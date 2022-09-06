#ifndef FLAG_H_

#define FLAG_H_

/* Includes for the header file */

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef FLAG_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef FLAG_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

DLLIMPORT typedef struct Flag Flag;
LANGUAGE DLLIMPORT CALLING char* flag_to_string(Flag* flag);

struct Flag {
    /* Es el nombre de la {Flag} */
    char* name;
    /* Es el valor de la {Flag} */
    char* value;
    /* Indica si es o no una {Flag} requerida */
    int required;
};

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
