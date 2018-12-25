#ifndef SSTL_ALLOC_L1_H
#define SSTL_ALLOC_L1_H

#define TEMPLATE_HEAD template<bool threads, int inst>
#define ALLOC_L1 __malloc_alloc_template<threads, inst>


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
* @brief   simple allocator which is a simple wrapper of
*          malloc, free and realloc function
*/
TEMPLATE_HEAD class __malloc_alloc_template {
private:
    // malloc handler
    static void (*__malloc_alloc_oom_handler)();

    /**
     * @brief   request memory when out-of-memory
     * @param   n: size of memory(bytes) to request
     * @return  pointer to memory block
     */
    static void* oom_malloc(size_t n) {
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
    static void* oom_realloc(void* p, size_t n) {
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

    // no need of instance
    __malloc_alloc_template() = default;

public:
    /**
     * @brief   wrapper of malloc
     * @param   n: size of memory block
     * @return  pointer to memory block
     */
    static void* allocate(size_t n) {
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
    static void deallocate(void* p) {
        free(p);
    }

    /**
     * @brief   wrapper of realloc function
     * @param   p: pointer to memory block which will be changed
     * @param   new_size: new size of memory block
     */
    static void* reallocate(void* p, size_t new_size) {
        void* result = realloc(p, new_size);
        if(result == nullptr) { // out-of-memory
            result = oom_realloc(p, new_size);
        }
        return result;
    }

    /**
     * @brief   set self-defined out-of-memory handler
     * @param   pointer to out-of-memory handler function
     */
    static auto set_malloc_handler(void(*f)()) -> void(*)() {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

} // sstl


#endif // SSTL_ALLOC_L1_H
