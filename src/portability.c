#include "portability.h"

#ifdef WIN32_LEAN_AND_MEAN

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

#endif // WIN32_LEAN_AND_MEAN

int n_processors_online() {
    long nprocs = -1;
    #ifdef _WIN32
        #ifndef _SC_NPROCESSORS_ONLN
            SYSTEM_INFO info;
            GetSystemInfo(&info);

            #define sysconf(a) info.dwNumberOfProcessors
            #define _SC_NPROCESSORS_ONLN
        #endif
    #endif

    #ifdef _SC_NPROCESSORS_ONLN

        nprocs = sysconf(_SC_NPROCESSORS_ONLN);
        return nprocs < 0 ? -1 : nprocs;

    #else

    return nprocs;

    #endif
}
