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

    // out-of-memory handler
    static void *oom_malloc(size_t n);
    static void *oom_realloc(void *p, size_t n);

    // no need of instance
    __malloc_alloc_template() = default;

public:
    static void *allocate(size_t n);
    static void deallocate(void *p);
    static void *reallocate(void *p, size_t new_size);

    // set self-defined malloc handler
    static auto set_malloc_handler(void(*f)()) -> void(*)() {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

} // sstl


#endif // SSTL_ALLOC_L1_H
