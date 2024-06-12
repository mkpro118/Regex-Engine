#include "portability.h"

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
