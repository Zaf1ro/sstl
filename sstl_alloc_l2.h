//
// Created by Jason Duan on 12/18/18.
//

#ifndef SSTL_ALLOC_L2_H
#define SSTL_ALLOC_L2_H

#include <cstdlib>

#define TEMPLATE_HEAD template<bool threads, int inst>


namespace sstl {

union obj {
    union obj* free_list_link; // next pointer
    char client_data[1]; // pointer to memory block
};

enum {
    __ALIGN = 8, // minimum size of memory block
    __MAX_BYTES = 128, // maximum size of memory block
    __NFREELISTS = __MAX_BYTES / __ALIGN // number of memory block in freelist
};

/**
 * @brief
 */
TEMPLATE_HEAD class __default_alloc_template {
private:
    static obj* free_list[__NFREELISTS];
    static char* start_free;    // start of memory pool
    static char* end_free;      // end of memory pool
    static size_t heap_size;    // total bytes of memory in allocator

    /**
     * @brief   convert bytes to the times of __ALIGN
     */
    static inline size_t ROUND_UP(size_t bytes) {
        return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);
    }

    /**
     * @brief   convert bytes to the index of freelist
     */
    static inline size_t FREELIST_INDEX(size_t bytes) {
        return (bytes + __ALIGN - 1) / __ALIGN - 1;
    }

    static char* chunk_alloc(size_t size, int nobjs);
    static void* refill(size_t n);

public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
};
} // sstl


#endif // SSTL_ALLOC_L2_H
