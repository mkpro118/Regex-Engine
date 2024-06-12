#ifndef _TEST_LIB_ASSERTS_H_
#define _TEST_LIB_ASSERTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* A zero exit code refers to success on all assertions.
   Any other value refers to a failure on at least one assertion. */
extern int ASSERTS_EXIT_CODE;

#ifdef FAIL_FAST

  #define ASSERTION_FAILED(fmt, a, b) do {\
      fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
      fprintf(stderr, fmt, (a), (b));\
      ASSERTS_EXIT_CODE = 1;\
      return;\
  } while(0)

#else

  #define ASSERTION_FAILED(fmt, a, b) do {\
      fprintf(stderr, "Assertion Failed! %s:%d | ", __func__, __LINE__);\
      fprintf(stderr, fmt, (a), (b));\
      ASSERTS_EXIT_CODE = 1;\
  } while(0)

#endif // FAIL_FAST

/* If they refer to the same memory then they are the same thing */
#define mem_equals(a, b, type) if ((type) (a) == ((type) (b))) break;

/* Compare integers, and display a message about the failure */
#define assert_equals_int(a, b) do {\
    if ((a) != (b)) {\
        ASSERTION_FAILED("%d != %d\n", (a), (b));\
    }\
} while(0)

/* Compare strings, and display a message about the failure */
#define assert_equals_str(a, b) do {\
    mem_equals((a), (b), char*);\
    if (strcmp((char*) (a), (char*) (b))) {\
        ASSERTION_FAILED("\"%s\" != \"%s\"\n", (a), (b));\
    }\
} while(0)


static int compar_int(const void* a, const void* b) {
    return *((const int*) a) - *((const int*) b);
}

static int compar_str(const void* a, const void* b) {
    return strcmp((const char*) a, (const char*) b);
}


/* Compare arrays of integers (ordered), and display a message about the failure */
#define assert_equals_int_array(a, b, size) do {\
    mem_equals((a), (b), int*);\
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
    for (int i = 0; i < (size); i++) {\
        if (strcmp((a)[i], (b)[i]) != 0) {\
            ASSERTION_FAILED("Mismatch at index %d, %d != %d\n", i, (a)[i], (b)[i]);\
        }\
    }\
    \
} while(0)

#define _test_arr_sort_(x, y, type, comparator) \
/* Allocate memory to copy array, we do not modify original array */\
type* _arr1 = malloc(sizeof(type) * (size));\
if (_arr1 == NULL) { /* Verify malloc worked */ \
    fprintf(stderr, "Allocator failed. strerror: %s\n", strerror(errno));\
    exit(EXIT_FAILURE);\
}\
type* _arr2 = malloc(sizeof(type) * (size));\
if (_arr2 == NULL) { /* Verify malloc worked */ \
    free(_arr1);\
    fprintf(stderr, "Allocator failed. strerror: %s\n", strerror(errno));\
    exit(EXIT_FAILURE);\
}\
\
/* Copy array to sort, we do not modify original array */\
memcpy(_arr1, x, sizeof(type) * (size));\
memcpy(_arr2, y, sizeof(type) * (size));\
qsort(_arr1, (size), sizeof(type), comparator);\
qsort(_arr2, (size), sizeof(type), comparator)

/* Compare arrays of integers (unordered), and display a message about the failure */
#define assert_equals_int_array_unordered(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _test_arr_sort_((a), (b), int, compar_int);\
    for (int i = 0; i < (size); i++) {\
        if ((a)[i] != (b)[i]) {\
            free(_arr1);\
            free(_arr2);\
            ASSERTION_FAILED("Mismatched elements, %d in " #a " not found in " #b ".\n", (a)[i]);\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)

/* Compare arrays of strings (unordered), and display a message about the failure */
#define assert_equals_str_array_unordered(a, b, size) do {\
    mem_equals((a), (b), int*);\
    _test_arr_sort_((a), (b), char*, compar_str);\
    for (int i = 0; i < (size); i++) {\
        if (strcmp((a)[i], (b)[i]) != 0) {\
            free(_arr1);\
            free(_arr2);\
            ASSERTION_FAILED("Mismatched elements, %s in " #a " not found in " #b ".\n",(a)[i]);\
        }\
    }\
    free(_arr1);\
    free(_arr2);\
} while(0)


#endif // _TEST_LIB_ASSERTS_H_
