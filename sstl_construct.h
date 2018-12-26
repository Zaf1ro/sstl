#ifndef SSTL_CONSTRUCT_H
#define SSTL_CONSTRUCT_H

#include <new>
#include "sstl_iterator.h"

namespace sstl {

/**
 * @brief   Constructs an object of type T pointed to p
 * @param   p: pointer to allocated uninitialized storage
 * @param   value: copy constructor argument
 */
template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new(p) T1(value); // call T1::T1(value)
}

/**
 * @brief   Calls the destructor of the object pointed by p
 * @param   p: pointer to the object that will be destroyed
 */
template <class T>
inline void destroy(T* p) {
    p->~T();
}

/**
 * @brief   Calls the destructor of objects in the range [first, last)
 * @param   first: an iterator to the beginning of given container
 * @param   last: an iterator to the end of given container
 */
template <class Iterator>
inline void destroy(Iterator first, Iterator last) {
    __destroy(first, last, value_type<first>);
}

template <class Iterator, class T>
inline void __destroy(Iterator first, Iterator last, T) {
    __destroy_aux(first, last, __type_traits<T>::has_trivial_destructor());
}

template <class Iterator>
inline void __destroy_aux(Iterator first, Iterator last, __true_type) {}

template <class Iterator>
inline void __destroy_aux(Iterator first, Iterator last, __false_type) {
    for( ; first != last; first++) {
        destroy(&*first);
    }
}

} // sstl

#endif // SSTL_CONSTRUCT_H
