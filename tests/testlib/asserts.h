#ifndef TEST_LIB_ASSERTS_H
#define TEST_LIB_ASSERTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* A zero exit code refers to success on all assertions.
   Any other value refers to a failure on at least one assertion. */
extern int ASSERTS_EXIT_CODE;

#ifdef FAIL_FAST
  #define ASSERTION_FAILED(...) do {\
      fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      ASSERTS_EXIT_CODE = 1;\
      return ASSERTS_EXIT_CODE;\
  } while(0)

#else

  #define ASSERTION_FAILED(...) do {\
      fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      ASSERTS_EXIT_CODE = 1;\
  } while(0)

#endif // FAIL_FAST

/* If they refer to the same memory then they are the same thing */
#define mem_equals(a, b, type) if ((type) (a) == ((type) (b))) break;


/* Assert a value is NULL */
#define assert_is_null(a) do {\
    if ((a) != NULL) {\
        ASSERTION_FAILED("Pointer " #a " is not NULL, found %p\n", (void*) (a));\
    }\
} while (0)


/* Assert a value is not NULL */
#define assert_is_not_null(a) do {\
    if ((a) == NULL) {\
        ASSERTION_FAILED("Pointer " #a " is NULL\n");\
    }\
} while (0)

/* Compare integers, and display a message about the failure */
#define assert_equals_int(a, b) do {\
    if ((a) != (b)) {\
        ASSERTION_FAILED("%ld != %ld\n", (long int)(a), (long int)(b));\
    }\
} while(0)

/* Compare pointers, and display a message about the failure */
#define assert_equals_ptr(a, b, type) do {\
    if (((type) (a)) != ((type) (b))) {\
        ASSERTION_FAILED("%p != %p\n", (void*) (a), (void*) (b));\
    }\
} while(0)

/* Compare pointers, and display a message about the failure */
#define assert_equals_funcptr(a, b, type) do {\
    if (((type) (a)) != ((type) (b))) {\
        ASSERTION_FAILED(#a " != " #b"\n");\
    }\
} while(0)

/* Compare strings, and display a message about the failure */
#define assert_equals_str(a, b) do {\
    mem_equals((a), (b), char*);\
    if (strcmp((char*) (a), (char*) (b))) {\
        ASSERTION_FAILED("\"%s\" != \"%s\"\n", (a), (b));\
    }\
} while(0)


// Comparator function for ints
static inline int compar_int(const void* a, const void* b) {
    return *((const int*) a) - *((const int*) b);
}

// Type safe wrapper over strcmp, comparator for strings
static inline int compar_str(const void* a, const void* b) {
    return strcmp((const char*) a, (const char*) b);
}

#define _ensure_not_null_(x, y) {if ((x) == NULL) {\
    ASSERTION_FAILED("Cannot assert because " #x " is NULL.\n");\
    break;\
} else if ((y) == NULL) {\
    ASSERTION_FAILED("Cannot assert because " #y " is NULL.\n");\
    break;\
}}


/* Compare arrays of integers (ordered), and display a message about the failure */
#define assert_equals_int_array(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _ensure_not_null_((a), (b));\
    for (int i = 0; i < (size); i++) {\
        if ((a)[i] != (b)[i]) {\
            ASSERTION_FAILED("Mismatch at index %d, %d != %d\n", i, (a)[i], (b)[i]);\
        }\
    }\
    \
} while(0)

/* Compare arrays of strings (ordered), and display a message about the failure */
#define assert_equals_str_array(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _ensure_not_null_((a), (b));\
    for (int i = 0; i < (size); i++) {\
        if (strcmp((a)[i], (b)[i]) != 0) {\
            ASSERTION_FAILED("Mismatch at index %d, %d != %d\n", i, (a)[i], (b)[i]);\
        }\
    }\
    \
} while(0)

#define _test_arr_sort_(x, y, sz, type, comparator) \
/* Allocate memory to copy array, we do not modify original array */\
type* _arr1 = malloc(sizeof(type) * (sz));\
if (_arr1 == NULL) { /* Verify malloc worked */ \
    fprintf(stderr, "Allocator failed. strerror: %s\n", strerror(errno));\
    exit(EXIT_FAILURE);\
}\
type* _arr2 = malloc(sizeof(type) * (sz));\
if (_arr2 == NULL) { /* Verify malloc worked */ \
    free(_arr1);\
    fprintf(stderr, "Allocator failed. strerror: %s\n", strerror(errno));\
    exit(EXIT_FAILURE);\
}\
\
/* Copy array to sort, we do not modify original array */\
memcpy(_arr1, x, sizeof(type) * (sz));\
memcpy(_arr2, y, sizeof(type) * (sz));\
qsort(_arr1, (sz), sizeof(type), comparator);\
qsort(_arr2, (sz), sizeof(type), comparator)

#ifdef FAIL_FAST
/* Compare arrays of integers (unordered), and display a message about the failure */
#define assert_equals_int_array_unordered(a, b, size) do {\
    _ensure_not_null_((a), (b));\
    mem_equals((a), (b), int*);\
    _test_arr_sort_((a), (b), (size), int, compar_int);\
    for (int i = 0; i < (size); i++) {\
        if ((a)[i] != (b)[i]) {\
            free(_arr1);\
            free(_arr2);\
            ASSERTION_FAILED("Mismatched elements (index %d after sorting asc.), %d in " #a " not found in " #b ".\n", i, (a)[i]);\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)

/* Compare arrays of strings (unordered), and display a message about the failure */
#define assert_equals_str_array_unordered(a, b, size) do {\
    _ensure_not_null_((a), (b));\
    mem_equals((a), (b), int*);\
    _test_arr_sort_((a), (b), (size), char*, compar_str);\
    for (int i = 0; i < (size); i++) {\
        if (strcmp((a)[i], (b)[i]) != 0) {\
            free(_arr1);\
            free(_arr2);\
            ASSERTION_FAILED("Mismatched elements (index %d after sorting with strcmp), %s in " #a " not found in " #b ".\n", i, (a)[i]);\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)

#else // not FAIL_FAST

/* Compare arrays of integers (unordered), and display a message about the failure */
#define assert_equals_int_array_unordered(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _ensure_not_null_((a),(b));\
    _test_arr_sort_((a), (b), (size), int, compar_int);\
    for (int i = 0; i < (size); i++) {\
        if (_arr1[i] != _arr2[i]) {\
            ASSERTION_FAILED("Mismatched elements (index %d after sorting asc.), %d in " #a " not found in " #b ".\n", i, _arr1[i]);\
            break;\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)

/* Compare arrays of strings (unordered), and display a message about the failure */
#define assert_equals_str_array_unordered(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _ensure_not_null_((a),(b));\
    _test_arr_sort_((a), (b), (size), char*, compar_str);\
    for (int i = 0; i < (size); i++) {\
        if (strcmp(_arr1[i], _arr2[i]) != 0) {\
            ASSERTION_FAILED("Mismatched elements (index %d after sorting with strcmp), %s in " #a " not found in " #b ".\n", i, _arr1[i]);\
            break;\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)

#endif // FAIL_FAST

#endif // TEST_LIB_ASSERTS_H
