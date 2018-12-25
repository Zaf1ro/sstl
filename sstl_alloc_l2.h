//
// Created by Jason Duan on 12/18/18.
//

#ifndef SSTL_ALLOC_L2_H
#define SSTL_ALLOC_L2_H

#include <cstdlib>
#include "sstl_alloc_l1.h"

#define TEMPLATE_HEAD template<bool threads, int inst>
#define ALLOC_L2 __default_alloc_template<threads, inst>

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

    /**
     * @brief   allocate number of 'nbjs' memory block
     *          and the size of memory block is 'size'
     * @param   size: size of memory block
     * @param   nobjs: number of memory block
     * @return  pointer to the start of memory block
     */
    static char* chunk_alloc(size_t size, int nobjs) {
        char *result;
        size_t total_bytes = size * nobjs; // total size of bytes
        size_t bytes_left = end_free - start_free; // remaining memory in memory pool

        if (bytes_left >= total_bytes) { // memory pool has enough memory to give
            result = start_free;
            start_free += total_bytes;
            return result;
        } else if (bytes_left >= size) { // not enough memory for blocks, but enough for one block
            nobjs = (int) (bytes_left / size); // re-calculate the need of size of memory block
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        } else { // no memory left in memory pool
            if (bytes_left > 0) { // put the rest of memory into free_list
                obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj*) start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*) start_free;
            }

            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4); // 2 times of need
            start_free = (char*) malloc(bytes_to_get);

            if (start_free == nullptr) { // if malloc failed, get memory from another memory block chain
                obj **my_free_list, *p;
                for (size_t i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != nullptr) {
                        *my_free_list = p->free_list_link;
                        start_free = (char*) p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                    }
                }
                end_free = nullptr;
                // call allocator level 1, may out-of-memory
                start_free = (char *)ALLOC_L1::allocate(bytes_to_get);
            }

            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs); // recursion
        }
    }

    /**
     * @brief   require memory block
     * @param   n: size of memory block
     * @return  pointer to the first memory block
     */
    static void* refill(size_t n) {
    #ifndef NOBJS
        #define NOBJS 20
    #endif
        char *chunk = chunk_alloc(n, NOBJS);

    #if NOBJS > 1
        // NOBJS == 1, no need to put into freelist
        // NOBJS > 1, put the rest of memory block into freelist
        obj *current_obj, *next_obj;
        obj **my_free_list = free_list + FREELIST_INDEX(n);
        *my_free_list = next_obj = (obj*) (chunk + n);

        // link all memory block
        for (int i = 1; ;i++) {
            current_obj = next_obj;
            next_obj = (obj*) ((char*) next_obj + n);
            if (i == NOBJS - 1) {
                current_obj->free_list_link = nullptr;
                break;
            } else {
                current_obj->free_list_link = next_obj;
            }
        }
    #endif
        return (void*) chunk;
    }

public:
    /**
     * @brief   allocate certain size of memory
     * @param   n: size of memory you want
     * @return  pointer to the start of memory
     */
    void* allocate(size_t n) {
        obj **my_free_list;
        obj *result;

        // call allocator level 1
        if (n > (size_t) __MAX_BYTES) {
            return ALLOC_L1::allocate(n);
        }

        // pick up suitable memory block from freelist
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;

        if (result == nullptr) { // no memory available
            void *r = refill(ROUND_UP(n));
            return r;
        }

        // remove target memory block from freelist
        *my_free_list = result->free_list_link;
        return result;
    }

    /**
     * @brief   recycle memory to freelist or release memory
     */
    static void deallocate(void *p, size_t n) {
        obj *q = (obj *) p;
        obj **my_free_list;

        // free memory
        if (n > (size_t) __MAX_BYTES) {
            ALLOC_L1::deallocate(p);
            return;
        }

        // put memory into freelist
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};

// initialize static class member
TEMPLATE_HEAD char* ALLOC_L2::start_free = nullptr;
TEMPLATE_HEAD char* ALLOC_L2::end_free = nullptr;
TEMPLATE_HEAD size_t ALLOC_L2::heap_size = 0;
TEMPLATE_HEAD obj* ALLOC_L2::free_list[__NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
};

} // sstl


#endif // SSTL_ALLOC_L2_H
