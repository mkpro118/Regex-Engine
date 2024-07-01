#include <string.h>

#include "testlib/asserts.h"
#include "testlib/tests.h"

#include "list.h"

// Define a test list for integers
CREATE_LIST_FOR(int, IntList)

IntList list;

// Compare function for integers
int compare_int(const int* a, const int* b) {
    return *a - *b;
}

// Test function to create a list
int test_create_list() {
    TEST_BEGIN;

    IntList* list = IntList_create(5);
    assert_is_not_null(list);
    assert_equals_int(list->capacity, 5);
    assert_equals_int(list->size, 0);
    assert_is_not_null(list->list);

    IntList_free(list, NULL);
    free(list);

    TEST_END;
}

// Test function to initialize a list
int test_init_list() {
    TEST_BEGIN;

    assert_equals_int(IntList_init(&list, 5), 0);
    assert_equals_int(list.capacity, 5);
    assert_equals_int(list.size, 0);
    assert_is_not_null(list.list);

    // Test initializing with capacity 0
    assert_equals_int(IntList_init(&list, 0), -1);

    // Test initializing NULL list
    assert_equals_int(IntList_init(NULL, 5), -1);

    IntList_free(&list, NULL);

    TEST_END;
}

// Test function to add elements to the list
int test_add_to_list() {
    TEST_BEGIN;

    int ret = IntList_init(&list, 2);
    assert_equals_int(ret, 0);

    int value1 = 10, value2 = 20, value3 = 30;

    assert_equals_int(IntList_add(&list, &value1), 1);
    assert_equals_int(IntList_add(&list, &value2), 2);
    assert_equals_int(list.size, 2);
    assert_equals_int(list.capacity, 2);

    // This should trigger reallocation
    assert_equals_int(IntList_add(&list, &value3), 3);
    assert_equals_int(list.size, 3);
    assert_equals_int(list.capacity, 3);

    assert_equals_int(list.list[0], 10);
    assert_equals_int(list.list[1], 20);
    assert_equals_int(list.list[2], 30);

    IntList_free(&list, NULL);

    TEST_END;
}

// Test function for find operation
int test_find_in_list() {
    TEST_BEGIN;

    int ret = IntList_init(&list, 3);
    assert_equals_int(ret, 0);

    int value1 = 10, value2 = 20, value3 = 30, value4 = 40;

    IntList_add(&list, &value1);
    IntList_add(&list, &value2);
    IntList_add(&list, &value3);

    int* found = IntList_find(&list, &value2, compare_int);
    assert_is_not_null(found);
    assert_equals_int(*found, 20);

    found = IntList_find(&list, &value4, compare_int);
    assert_is_null(found);

    IntList_free(&list, NULL);

    TEST_END;
}

// Test function for remove operation
int test_remove_from_list() {
    TEST_BEGIN;

    int ret = IntList_init(&list, 3);
    assert_equals_int(ret, 0);

    int value1 = 10, value2 = 20, value3 = 30, value4 = 40;

    IntList_add(&list, &value1);
    IntList_add(&list, &value2);
    IntList_add(&list, &value3);

    assert_equals_int(IntList_remove(&list, &value2, compare_int), 0);
    assert_equals_int(list.size, 2);
    assert_equals_int(list.list[0], 10);
    assert_equals_int(list.list[1], 30);

    assert_equals_int(IntList_remove(&list, &value4, compare_int), -1);
    assert_equals_int(list.size, 2);

    IntList_free(&list, NULL);

    TEST_END;
}

// Test function for size operation
int test_list_size() {
    TEST_BEGIN;

    int ret = IntList_init(&list, 3);
    assert_equals_int(ret, 0);

    int value1 = 10, value2 = 20;

    assert_equals_int(IntList_size(&list), 0);

    IntList_add(&list, &value1);
    assert_equals_int(IntList_size(&list), 1);

    IntList_add(&list, &value2);
    assert_equals_int(IntList_size(&list), 2);

    IntList_remove(&list, &value1, compare_int);
    assert_equals_int(IntList_size(&list), 1);

    IntList_free(&list, NULL);

    assert_equals_int(IntList_size(NULL), 0);

    TEST_END;
}

// Test function for edge cases
int test_edge_cases() {
    TEST_BEGIN;

    // Test creating list with 0 capacity
    int ret = IntList_init(&list, 0);
    assert_equals_int(ret, -1);

    // Test adding to NULL list
    int value = 10;
    assert_equals_int(IntList_add(NULL, &value), -1);

    // Test adding NULL value
    ret = IntList_init(&list, 1);
    assert_equals_int(ret, 0);

    assert_equals_int(IntList_add(&list, NULL), -1);

    // Test finding in NULL list
    assert_is_null(IntList_find(NULL, &value, compare_int));

    // Test finding with NULL compare function
    assert_is_null(IntList_find(&list, &value, NULL));

    // Test removing from NULL list
    assert_equals_int(IntList_remove(NULL, &value, compare_int), -1);

    // Test removing with NULL compare function
    assert_equals_int(IntList_remove(&list, &value, NULL), -1);

    IntList_free(&list, NULL);

    TEST_END;
}

// Define the tests array
Test tests[] = {
    {.name="test_create_list", .func=test_create_list},
    {.name="test_init_list", .func=test_init_list},
    {.name="test_add_to_list", .func=test_add_to_list},
    {.name="test_find_in_list", .func=test_find_in_list},
    {.name="test_remove_from_list", .func=test_remove_from_list},
    {.name="test_list_size", .func=test_list_size},
    {.name="test_edge_cases", .func=test_edge_cases},
    {NULL, NULL}  // Sentinel to mark the end of the array
};

// Main function
int main(int argc, char* argv[]) {
    return default_main(&argv[1], argc - 1);
}
