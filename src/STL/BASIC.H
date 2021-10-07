// A module of Dlib v1.4.1.
// Written by Keith Pomakis during the summer of 1994.
// Released to the public domain on October 10, 1994.
// This version released on May 12, 1996.

#ifndef D_BASIC_DEFINED
#define D_BASIC_DEFINED

#include <string.h>
#include <iostream.h>
#include <stdlib.h>
#include "bool.h"

/****************************************************************************/

#ifndef NULL
#define NULL 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif


/****************************************************************************/

// Since the int type has only recently been added to the C++ standard,
// many compilers may not implement it yet.  Define the symbol
// D_SIMULATE_int to simulate the int type.

#ifdef D_SIMULATE_BOOL
    typedef bool int;
    const int false = 0;
    const int true = 1;
#endif

/****************************************************************************/

/*#ifndef min         // Hopefully this isn't already defined as a macro.

template <class T>
inline T
min(T arg1, T arg2)
{
    return (arg1 < arg2)? arg1 : arg2;
}

inline char *
min(char *arg1, char *arg2)
{
    return (strcmp(arg1, arg2) < 0)? arg1 : arg2;
}

#endif*/

/****************************************************************************/

/*#ifndef max         // Hopefully this isn't already defined as a macro.

template <class T>
inline T
max(T arg1, T arg2)
{
    return (arg2 < arg1)? arg1 : arg2;
}

inline char *
max(char *arg1, char *arg2)
{
    return (strcmp(arg2, arg1) < 0)? arg1 : arg2;
}

#endif*/

/****************************************************************************/

template <class T>
inline void
swap(T &arg1, T &arg2)
{
    T tmp = arg1;
    arg1 = arg2;
    arg2 = tmp;
}

/****************************************************************************/

inline void
check_mem(void *ptr)
{
    // Declare string as static so that it isn't defined per instantiation
    // in case the compiler actually does decide to inline this code.
    static const char *const mem_err = "Error allocating memory.\n";
    if (ptr == NULL) {
        cerr << mem_err;
        exit(EXIT_FAILURE);
    }
}

/****************************************************************************/

#endif /* D_BASIC_DEFINED */
