#include "alloc_l1.h"
#include <cstddef>


namespace sstl {

/**
 * @brief   request memory when out-of-memory
 * @param   n: size of memory(bytes) to request
 * @return  pointer to memory block
 */
TEMPLATE_HEAD void* ALLOC_L1::oom_malloc(size_t n) {
    void (*my_alloc_handler)();
    void* result;

    for(;;) {
        my_alloc_handler = __malloc_alloc_oom_handler;
        if(my_alloc_handler == nullptr) {
            __THROW_BAD_ALLOC;
        }
        (*my_alloc_handler)(); // call malloc handler to release memory

        result = malloc(n); // allocate memory
        if(result) {
            return result;
        }
    }
}

/**
 * @brief   change the size of memory block
 * @param   p: pointer to the memory block which will be changed
 * @param   n: new size of memory block
 * @return  pointer to the new memory block
 */
TEMPLATE_HEAD void* ALLOC_L1::oom_realloc(void* p, size_t n) {
    void (*my_malloc_handler)();
    void* result;

    for(;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if(my_malloc_handler == nullptr) {
            __THROW_BAD_ALLOC;
        }
        (*my_malloc_handler)();

        result = realloc(p, n);
        if(result) {
            return result;
        }
    }
}

/**
 * @brief   wrapper of malloc
 * @param   n: size of memory block
 * @return  pointer to memory block
 */
TEMPLATE_HEAD void* ALLOC_L1::allocate(size_t n) {
    void* result = malloc(n);
    if(result == nullptr) { // out-of-memory
        result = ALLOC_L1::oom_malloc(n);
    }
    return result;
}

/**
 * @brief   wrapper of free function
 * @param   pointer to memory block
 */
TEMPLATE_HEAD void ALLOC_L1::deallocate(void* p) {
    free(p);
}

/**
 * @brief   wrapper of realloc function
 * @param   p: pointer to memory block which will be changed
 * @param   new_size: new size of memory block
 */
TEMPLATE_HEAD void* ALLOC_L1::reallocate(void* p, size_t new_size) {
    void* result = realloc(p, new_size);
    if(result == nullptr) { // out-of-memory
        result = oom_realloc(p, new_size);
    }
    return result;
}

} // sstl


