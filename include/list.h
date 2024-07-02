#ifndef REGEX_LIST_H
#define REGEX_LIST_H

#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define CREATE_LIST_TYPE_FOR(type, list_name) typedef struct list_name {\
    size_t capacity;\
    size_t size;\
    type* list;\
} list_name;\
typedef void (*list_name##_free_cb)(type*);\
typedef int (*list_name##_compare_cb)(const type*, const type*);\
int list_name##_init(list_name* list, size_t capacity);\
list_name* list_name##_create(size_t capacity);\
int list_name##_add(list_name* list, type* value);\
void list_name##_free(list_name* list, list_name##_free_cb cb);\
type* list_name##_find(list_name* list, const type* value, list_name##_compare_cb compare);\
int list_name##_remove(list_name* list, const type* value, list_name##_compare_cb compare);\
size_t list_name##_size(const list_name* list);

#define CREATE_LIST_IMPL_FOR(type, list_name)\
int list_name##_init(list_name* list, size_t capacity) {\
    if (list == NULL || capacity == 0) {\
        return -1;\
    }\
\
    type* arr = malloc(sizeof(type) * capacity);\
    if (arr == NULL) {\
        /* Failed to allocate memory. */\
        return -1;\
    }\
\
    list->capacity = capacity;\
    list->size = 0;\
    list->list = arr;\
\
    return 0;\
}\
\
list_name* list_name##_create(size_t capacity) {\
    if (capacity == 0) {\
        return NULL;\
    }\
\
    list_name* list = malloc(sizeof(list_name));\
    if (list == NULL) {\
        /* Failed to allocate memory. */\
        return NULL;\
    }\
\
    if (list_name##_init(list, capacity) != 0) {\
        free(list);\
        return NULL;\
    }\
\
    return list;\
}\
\
int list_name##_add(list_name* list, type* value) {\
    if (list == NULL || value == NULL) {\
        return -1;\
    }\
    \
    if (list->size > list->capacity) {\
        /* This is invalid state*/ \
        return -1;\
    }\
    if (list->size == list->capacity) {\
        size_t temp_cap = list->capacity + ((list->capacity + 1) / 2);\
        type* temp_ptr = realloc(list->list, sizeof(type) * temp_cap);\
        if (temp_ptr == NULL) {\
            /* Failed to allocate memory. The original list is intact. */\
            return -1;\
        }\
        list->list = temp_ptr;\
        list->capacity = temp_cap;\
    }\
    list->list[list->size++] = *value;\
    return list->size;\
}\
\
void list_name##_free(list_name* list, list_name##_free_cb cb) {\
    if (list == NULL) {\
        return;\
    }\
\
    if (cb != NULL) {\
        for (type* item = list->list; item < &list->list[list->size]; item++) {\
            cb(item);\
        }\
    }\
\
    free(list->list);\
}\
\
type* list_name##_find(list_name* list, const type* value, list_name##_compare_cb compare) {\
    if (list == NULL || value == NULL || compare == NULL) {\
        return NULL;\
    }\
\
    for (size_t i = 0; i < list->size; i++) {\
        if (compare((const type*) &list->list[i], value) == 0) {\
            return &list->list[i];\
        }\
    }\
\
    return NULL;\
}\
\
int list_name##_remove(list_name* list, const type* value, list_name##_compare_cb compare) {\
    if (list == NULL || value == NULL || compare == NULL) {\
        return -1;\
    }\
\
    for (size_t i = 0; i < list->size; i++) {\
        if (compare((const type*) &list->list[i], value) == 0) {\
            memmove(&list->list[i], &list->list[i + 1], (list->size - i - 1) * sizeof(type));\
            list->size--;\
            return 0;\
        }\
    }\
\
    return -1;\
}\
\
size_t list_name##_size(const list_name* list) {\
    return list ? list->size : 0;\
}

#endif // REGEX_LIST_H
