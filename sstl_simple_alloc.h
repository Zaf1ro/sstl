#ifndef SSTL_SIMPLE_ALLOC_H
#define SSTL_SIMPLE_ALLOC_H

#include <cstddef>


namespace sstl {

/**
 * @brief   wrapper of memory allocator
 */
template <class T, class Alloc>
class simple_alloc {
private:
    simple_alloc() = default;

public:
    static T* allocate() {
        return (T*) Alloc::allocate(sizeof(T));
    }

    static T* allocate(size_t n) {
        return (T*) Alloc::allocate(n);
    }

    static void deallocate(T* p, size_t n) {
        if( n != 0 ) {
            Alloc::deallocate(p, n);
        }
    }

    static void deallocate(T* p) {
        Alloc::deallocate(p, sizeof(T));
    }
};

} // sstd


#endif // SSTL_SIMPLE_ALLOC_H
