#include "tests.h"
#include "../util/bool.h"
#include "../util/lists.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../memory/heap.h"

void test_assert(int condition, const char *message) {
    if (!condition) {
        printf("Test failed: %s\n", message);
        while (1) {}
    }
}

// Function to print a test result
void print_test_result(uint8_t condition, const char *test_name) {
    if (condition) {
        printf("%s: PASSED\n", test_name);
    } else {
        printf("%s: FAILED\n", test_name);
    }
}

// Function to create and initialize a linked list for testing
struct linked_list *create_and_initialize_list() {
    struct linked_list *list = linked_list_create();

    linked_list_add(list, "Item 1");
    linked_list_add(list, "Item 2");
    linked_list_add(list, "Item 3");

    return list;
}

// Test case for linked_list_create and linked_list_destroy
void test_linked_list_create_and_destroy() {
    struct linked_list *list = linked_list_create();
    linked_list_destroy(list);
    print_test_result(true, "test_linked_list_create_and_destroy");
}

// Test case for linked_list_add and linked_list_size
void test_linked_list_add_and_size() {
    struct linked_list *list = create_and_initialize_list();
    int size = linked_list_size(list);
    linked_list_destroy(list);
    print_test_result(size == 3, "test_linked_list_add_and_size");
}

// Test case for linked_list_get, linked_list_get_first, and linked_list_get_last
void test_linked_list_get() {
    struct linked_list *list = create_and_initialize_list();
    void *item1 = linked_list_get(list, 0);
    void *firstItem = linked_list_get_first(list);
    void *lastItem = linked_list_get_last(list);
    linked_list_destroy(list);
    print_test_result(item1 != NULL && firstItem != NULL && lastItem != NULL, "test_linked_list_get");
}

// Test case for linked_list_remove_at with invalid index
void test_linked_list_remove_at_invalid_index() {
    struct linked_list *list = create_and_initialize_list();
    linked_list_remove_at(list, 10); // Removing an element at an invalid index
    int size = linked_list_size(list);
    linked_list_destroy(list);
    print_test_result(size == 3, "test_linked_list_remove_at_invalid_index");
}

// Test case for linked_list_remove with non-existent data
void test_linked_list_remove_non_existent_data() {
    struct linked_list *list = create_and_initialize_list();
    char *nonExistentData = "Non-existent";
    linked_list_remove(list, nonExistentData); // Removing non-existent data
    int size = linked_list_size(list);
    linked_list_destroy(list);
    print_test_result(size == 3, "test_linked_list_remove_non_existent_data");
}

// Test case for linked_list_remove_all
void test_linked_list_remove_all() {
    struct linked_list *list = create_and_initialize_list();
    linked_list_remove_all(list);
    int size = linked_list_size(list);
    linked_list_destroy(list);
    print_test_result(size == 0, "test_linked_list_remove_all");
}

void test_circular_buffer_create_and_destroy() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    CircularBufferFree(cBuf);
    print_test_result(true, "test_circular_buffer_create_and_destroy");
}

void test_circular_buffer_reset() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    CircularBufferReset(cBuf);
    print_test_result(true, "test_circular_buffer_reset");
}

void test_circular_buffer_get_capacity() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    size_t capacity = CircularBufferGetCapacity(cBuf);
    CircularBufferFree(cBuf);
    print_test_result(capacity == 10, "test_circular_buffer_get_capacity");
}

void test_circular_buffer_get_size() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    size_t size = CircularBufferGetSize(cBuf);
    CircularBufferFree(cBuf);
    print_test_result(size == 10, "test_circular_buffer_get_size");
}

void test_circular_buffer_get_data_size() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    size_t data_size = CircularBufferGetDataSize(cBuf);
    CircularBufferFree(cBuf);
    print_test_result(data_size == 0, "test_circular_buffer_get_data_size");
}

void test_circular_buffer_push() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    char *data = "Hello";
    CircularBufferPush(cBuf, data, 5);
    size_t data_size = CircularBufferGetDataSize(cBuf);
    CircularBufferFree(cBuf);
    print_test_result(data_size == 5, "test_circular_buffer_push");
}

void test_circular_buffer_pop() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    char *data = "Hello";
    CircularBufferPush(cBuf, data, 5);
    char *dataOut = malloc(5);
    CircularBufferPop(cBuf, 5, dataOut);
    CircularBufferFree(cBuf);
    print_test_result(strcmp(data, dataOut) == 0, "test_circular_buffer_pop");
}

void test_circular_buffer_read() {
    CircularBuffer cBuf = CircularBufferCreate(10);
    char *data = "Hello";
    CircularBufferPush(cBuf, data, 5);
    char *dataOut = malloc(5);
    CircularBufferRead(cBuf, 5, dataOut);
    CircularBufferFree(cBuf);
    print_test_result(strcmp(data, dataOut) == 0, "test_circular_buffer_read");
}

void test_circular_buffer_push_when_full() {
    //Fill the buffer and then try to push more data
    CircularBuffer cBuf = CircularBufferCreate(4);
    char *data = "Hello";
    CircularBufferPush(cBuf, data, 5);
    CircularBufferPush(cBuf, data, 5);
    CircularBufferPush(cBuf, data, 5);
    CircularBufferPush(cBuf, data, 5);
    CircularBufferPush(cBuf, data, 5);
    size_t data_size = CircularBufferGetDataSize(cBuf);
    CircularBufferFree(cBuf);
    print_test_result(data_size == 4, "test_circular_buffer_push_when_full");
}

void test_circular_buffer_pop_when_empty() {
    //Pop data from an empty buffer
    CircularBuffer cBuf = CircularBufferCreate(4);
    char *dataOut = malloc(5);
    int result = CircularBufferPop(cBuf, 5, dataOut);
    CircularBufferFree(cBuf);
    print_test_result(result == 0, "test_circular_buffer_pop_when_empty");
}

void listest() {
    test_linked_list_create_and_destroy();
    test_linked_list_add_and_size();
    test_linked_list_get();
    test_linked_list_remove_at_invalid_index();
    test_linked_list_remove_non_existent_data();
    test_linked_list_remove_all();

    test_circular_buffer_create_and_destroy();
    test_circular_buffer_reset();
    test_circular_buffer_get_capacity();
    test_circular_buffer_get_size();
    test_circular_buffer_get_data_size();
    test_circular_buffer_push();
    test_circular_buffer_pop();
    test_circular_buffer_read();
    test_circular_buffer_push_when_full();
    test_circular_buffer_pop_when_empty();
    
    return 0;
}

