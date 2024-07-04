#include "regex.h"

// Create a heap allocated and initialized regex buffer.
Regex* regex_create(char* pattern) {
    Regex* buf = malloc(sizeof(Regex));

    if (regex_init(buf, pattern) < 0) {
        free(buf);
        return NULL;
    }

    return buf;
}

// Initialize a given regex buffer.
int regex_init(Regex* regex_buf, char* pattern);

// Compile a given regex pattern.
int regex_compile(Regex* regex_buf, char* pattern);

// Test whether the given string matches the given regex.
// This function will compile the regex if it is not already compiled.
bool regex_match(Regex* regex_buf, char* string);

// Release the memory used by the given regex structure
void regex_free(Regex* regex_buf);
