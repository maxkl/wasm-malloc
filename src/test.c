
#include "print.h"
#include "mm.h"

__attribute__((visibility("default")))
void *test_malloc(size_t size) {
    return malloc(size);
}

__attribute__((visibility("default")))
void test_free(void *ptr) {
    free(ptr);
}

__attribute__((visibility("default")))
void *test_calloc(size_t nmemb, size_t size) {
    return calloc(nmemb, size);
}

__attribute__((visibility("default")))
void *test_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}
