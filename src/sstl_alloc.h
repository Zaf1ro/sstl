#ifndef SSTL_ALLOC_L2_H
#define SSTL_ALLOC_L2_H

#include <cstdlib>

#define __SSTL_DEFAULT_NOBJS 5

#define __SSTL_ALLOC_TEMPLATE           template<bool threads, int inst>
#define __SSTL_ALLOC_L1(_Thread, _Inst) sstl::__malloc_alloc_template<_Thread, _Inst>
#define __SSTL_ALLOC_L2(_Thread, _Inst) sstl::__default_alloc_template<_Thread, _Inst>
#define __SSTL_ALLOC(_T, _Alloc)        sstl::simple_alloc<_T, _Alloc>
#define __SSTL_DEFAULT_ALLOC            __SSTL_ALLOC_L2(1, true)

#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC \
            std::cout << "out of memory" << std::endl; \
            exit(1)
#endif


namespace sstl {

/**
 * @brief   Simple allocator which is a simple wrapper of
 *          malloc, free and realloc function
 */
__SSTL_ALLOC_TEMPLATE class __malloc_alloc_template {
private:
    // malloc handler
    static void (*s_malloc_alloc_oom_handler)();

    /**
     * @brief   Request memory when out-of-memory
     * @param   __n: size of memory(bytes) to request
     * @return  Pointer to memory block
     */
    static void* _s_oom_malloc(size_t __n) {
        void (*my_alloc_handler)();
        void* result;

        for(; ; ) {
            my_alloc_handler = s_malloc_alloc_oom_handler;
            if(my_alloc_handler == nullptr) {
                __THROW_BAD_ALLOC;
            }
            (*my_alloc_handler)(); // call malloc handler to release memory

            result = malloc(__n); // allocate memory
            if(result) {
                return result;
            }
        }
    }

    /**
     * @brief   Change the size of memory block
     * @param   __p: pointer to the memory block which will be changed
     * @param   __n: new size of memory block
     * @return  Pointer to the new memory block
     */
    static void* _s_oom_realloc(void *__p, size_t __n) {
        void (*tmp_malloc_handler)();
        void* result;

        for(; ; ) {
            tmp_malloc_handler = s_malloc_alloc_oom_handler;
            if(tmp_malloc_handler == nullptr) {
                __THROW_BAD_ALLOC;
            }
            (*tmp_malloc_handler)();

            result = realloc(__p, __n);
            if(result)
                return result;
        }
    }

    // no need of instance
    __malloc_alloc_template() = default;

public:
    /**
     * @brief   Wrapper of malloc
     * @param   __n: size of memory block
     * @return  Pointer to memory block
     */
    static void* allocate(size_t __n) {
        void* result = malloc(__n);
        if(result == nullptr) { // out-of-memory
            result = __SSTL_ALLOC_L1(threads, inst)::_s_oom_malloc(__n);
        }
        return result;
    }

    /**
     * @brief   Wrapper of free function
     * @param   Pointer to memory block
     */
    static void deallocate(void *__p) {
        free(__p);
    }

    /**
     * @brief   Wrapper of realloc function
     * @param   __p: pointer to memory block which will be changed
     * @param   ___new_size: new size of memory block
     */
    static void* reallocate(void* __p, size_t __new_size) {
        void* result = realloc(__p, __new_size);
        if(result == nullptr) { // out-of-memory
            result = _s_oom_realloc(__p, __new_size);
        }
        return result;
    }

    /**
     * @brief   Set self-defined out-of-memory handler
     * @param   Pointer to out-of-memory handler function
     */
    static auto set_malloc_handler(void(*__f)()) -> void(*)() {
        void (*old)() = s_malloc_alloc_oom_handler;
        s_malloc_alloc_oom_handler = __f;
        return old;
    }
};

__SSTL_ALLOC_TEMPLATE
void (* __malloc_alloc_template<threads, inst>::s_malloc_alloc_oom_handler)() = nullptr;


/**
 * @brief   Allocator with independent memory pool
 */
__SSTL_ALLOC_TEMPLATE class __default_alloc_template {
private:
    union obj {
        union obj *free_list_link; // next pointer
        char client_data[1]; // pointer to memory block
    };

    enum {
        __ALIGN = 8, // minimum size of memory block
        __MAX_BYTES = 128, // maximum size of memory block
        __NFREELISTS = __MAX_BYTES / __ALIGN // number of memory block in freelist
    };

private:
    static obj *m_free_list[__NFREELISTS];
    static char *m_start_free;    // start of memory pool
    static char *m_end_free;      // end of memory pool
    static size_t m_heap_size;    // total bytes of memory in allocator

    /**
     * @brief   Convert bytes to the times of __ALIGN
     */
    static inline size_t _s_round_up(size_t __bytes) {
        return (__bytes + __ALIGN - 1) & ~(__ALIGN - 1);
    }

    /**
     * @brief   Convert bytes to the index of freelist
     */
    static inline size_t _s_freelist_index(size_t __bytes) {
        return ((__bytes + __ALIGN - 1) / __ALIGN - 1);
    }

    /**
     * @brief   Allocate number of __nbjs memory block
     *          and the size of memory block is __size
     * @param   __size: size of memory block
     * @param   __nobjs: number of memory block
     * @return  Pointer to the start of memory block
     */
    static char* _s_chunk_alloc(size_t __size, int __nobjs) {
        char *result;
        size_t total_bytes = __size * __nobjs; // total size of bytes
        size_t bytes_left = m_end_free - m_start_free; // remaining memory in memory pool

        if (bytes_left >= total_bytes) { // memory pool has enough memory to give
            result = m_start_free;
            m_start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= __size) { // not enough memory for blocks, but enough for one block
            __nobjs = (int) (bytes_left / __size); // re-calculate the need of size of memory block
            total_bytes = __size * __nobjs;
            result = m_start_free;
            m_start_free += total_bytes;
            return result;
        }
        else { // no memory left in memory pool
            if (bytes_left > 0) { // put the rest of memory into free_list
                obj **my_free_list = m_free_list + _s_freelist_index(bytes_left);
                ((obj*) m_start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*) m_start_free;
            }

            size_t bytes_to_get = 2 * total_bytes + _s_round_up(m_heap_size >> 4); // 2 times of need
            m_start_free = (char*) malloc(bytes_to_get);

            if (m_start_free == nullptr) { // if malloc failed, get memory from another memory block chain
                obj **my_free_list, *p;
                for (size_t i = __size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = m_free_list + _s_freelist_index(i);
                    p = *my_free_list;
                    if (p != nullptr) {
                        *my_free_list = p->free_list_link;
                        m_start_free = (char*) p;
                        m_end_free = m_start_free + i;
                        return _s_chunk_alloc(__size, __nobjs);
                    }
                }
                m_end_free = nullptr;
                // call allocator level 1, may out-of-memory
                m_start_free = (char *)__SSTL_ALLOC_L1(threads, inst)::allocate(bytes_to_get);
            }

            m_heap_size += bytes_to_get;
            m_end_free = m_start_free + bytes_to_get;
            return _s_chunk_alloc(__size, __nobjs); // recursion
        }
    }

    /**
     * @brief   Require memory block
     * @param   __n: size of memory block
     * @return  Pointer to the first memory block
     */
    static void *_s_refill(size_t __n) {
    #ifdef NOBJS
        #if NOBJS <= 0
            NOBJS = __SSTL_DEFAULT_NOBJS
        #endif
    #else
        #define NOBJS __SSTL_DEFAULT_NOBJS
    #endif
        char *chunk = _s_chunk_alloc(__n, NOBJS);

    #if NOBJS > 1
        // NOBJS == 1, no need to put into freelist
        // NOBJS > 1, put the rest of memory block into freelist
        obj *current_obj, *next_obj;
        obj **my_free_list = m_free_list + _s_freelist_index(__n);
        *my_free_list = next_obj = (obj*) (chunk + __n);

        // link all memory block
        for (int i = 1; ; i++) {
            current_obj = next_obj;
            next_obj = (obj*) ((char*) next_obj + __n);
            if (i == NOBJS - 1) {
                current_obj->free_list_link = nullptr;
                break;
            } else {
                current_obj->free_list_link = next_obj;
            }
        }
    #endif
        return chunk;
    }

public:
    /**
     * @brief   allocate certain size of memory
     * @param   __n: size of memory you want
     * @return  pointer to the start of memory
     */
    static void *allocate(size_t __n) {
        // call allocator level 1
        if (__n > (size_t) __MAX_BYTES) {
            return __SSTL_ALLOC_L1(threads, inst)::allocate(__n);
        }

        // pick up suitable memory block from freelist
        obj **my_free_list = m_free_list + _s_freelist_index(__n);
        obj *result = *my_free_list;

        if (result == nullptr) { // no memory available
            void *r = _s_refill(_s_round_up(__n));
            return r;
        }

        // remove target memory block from freelist
        *my_free_list = result->free_list_link;
        return result;
    }

    /**
     * @brief   recycle memory to freelist or release memory
     */
    static void deallocate(void *p, size_t __n) {
        obj *q = (obj*) p;
        obj **my_free_list;

        // free memory
        if (__n > (size_t) __MAX_BYTES) {
            __SSTL_ALLOC_L1(threads, inst)::deallocate(p);
            return;
        }

        // put memory into freelist
        my_free_list = m_free_list + _s_freelist_index(__n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};

// initialize static class member
__SSTL_ALLOC_TEMPLATE char *__SSTL_ALLOC_L2(threads, inst)::m_start_free = nullptr;
__SSTL_ALLOC_TEMPLATE char *__SSTL_ALLOC_L2(threads, inst)::m_end_free = nullptr;
__SSTL_ALLOC_TEMPLATE size_t __SSTL_ALLOC_L2(threads, inst)::m_heap_size = 0;
__SSTL_ALLOC_TEMPLATE typename __SSTL_ALLOC_L2(threads, inst)::obj*
__SSTL_ALLOC_L2(threads, inst)::m_free_list[__NFREELISTS] = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr,
};


/**
 * @brief   wrapper of memory allocator
 */
template <class T, class Alloc>
class simple_alloc {
public:
    simple_alloc() = default;

    static T *allocate() {
        return (T*) Alloc::allocate(sizeof(T));
    }

    static T *allocate(size_t __n) {
        return (T*) Alloc::allocate(sizeof(T) * __n);
    }

    static void deallocate(T *__p, size_t __n) {
        if( __n != 0 ) {
            Alloc::deallocate(__p, __n);
        }
    }

    static void deallocate(T *__p) {
        Alloc::deallocate(__p, sizeof(T));
    }
};

} // sstl


#endif // SSTL_ALLOC_L2_H
