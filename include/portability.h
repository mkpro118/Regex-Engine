#ifndef _REGEX_PORTABILITY_H_
#define _REGEX_PORTABILITY_H_

#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    char* strdup(const char* source);

#elif __unix__ // all unices not caught above
    #include <unistd.h>
#elif defined(_POSIX_VERSION)
    #include <unistd.h>
#else
#error "Unknown compiler"
#endif

#endif // _REGEX_PORTABILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


int n_processors_online();
