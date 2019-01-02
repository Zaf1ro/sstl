#ifndef SSTL_CONSTRUCT_H
#define SSTL_CONSTRUCT_H

#include <new>
#include "sstl_iterator.h"
#include "sstl_traits.h"

namespace sstl {

template <class T1, class T2> inline void
__CONSTRUCT(T1* p, const T2& value) {
    __construct(p, value);
}

template <class T> inline void
__DESTROY(T* p) {
    __destroy(p);
}

template <class Iter> inline void
__DESTROY(Iter first, Iter last) {
    __destroy(first, last);
}


/**
 * @brief   Constructs an object of type T pointed to p
 * @param   p: pointer to allocated uninitialized storage
 * @param   value: copy constructor argument
 */
template <class T1, class T2>
inline void __construct(T1* p, const T2& value) {
    new(p) T1(value); // call T1::T1(value)
}

/**
 * @brief   Calls the destructor of the object pointed by p
 * @param   p: pointer to the object that will be destroyed
 */
template <class T>
inline void __destroy(T* p) {
    p->~T();
}

/**
 * @brief   Calls the destructor of objects in the range [first, last)
 * @param   first: an iterator to the beginning of given container
 * @param   last: an iterator to the end of given container
 */
template <class Iter> inline void
__destroy(Iter first, Iter last) {
    __destroy_aux(first, last, __VALUE_TYPE(first));
}

template <class Iter, class T> inline void
__destroy_aux(Iter first, Iter last, T) {
    typedef typename __type_traits<T>::has_trivial_destructor Trivial_Des;
    __destroy_t(first, last, Trivial_Des());
}

template <class Iter> inline void
__destroy_t(Iter first, Iter last, __true_type) {}

template <class Iter> inline void
__destroy_t(Iter first, Iter last, __false_type) {
    for( ; first != last; first++) {
        destroy(&*first);
    }
}

} // sstl

#endif // SSTL_CONSTRUCT_H
