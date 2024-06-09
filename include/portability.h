#ifndef _REGEX_PORTABILITY_H_
#define _REGEX_PORTABILITY_H_

#include <string.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)

char* strdup(const char* source) {
    int len = strlen(source);
    char* dest = malloc(sizeof(char) * (len + 1));

    if (dest == NULL) {
        return NULL;
    }

    strncpy(dest, source, len);
    dest[len] = '\0'; // Nul-terminator
    return dest;
}

#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#error "Unknown compiler"
#endif

#endif // _REGEX_PORTABILITY_H_
