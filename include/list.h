#ifndef REGEX_LIST_H
#define REGEX_LIST_H

#include <stdlib.h>
#include <sys/types.h>

#define CREATE_LIST_FOR(type, list_name) typedef struct list_name {\
    size_t capacity;\
    size_t size;\
    type* list;\
} list_name;\
typedef void (*list_name##_free_cb(type*));\
\
\
list_name* list_name##_create(size_t capacity) {\
    if (capacity <= 0) {\
        return NULL;\
    }\
\
    list_name* list = malloc(sizeof(list_name));\
    if (list == NULL) {\
        /* Failed to allocate memory. */\
        return NULL;\
    }\
\
    type* arr = malloc(sizeof(type) * capacity);\
    if (arr == NULL) {\
        /* Failed to allocate memory. */\
        free(list);\
        return NULL;\
    }\
\
    *list = (list_name) {\
        .capacity = capacity,\
        .size = 0,\
        .list = arr,\
    };\
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
        size_t temp_cap = list->capacity + list->capacity / 2;\
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
}

#endif // REGEX_LIST_H
