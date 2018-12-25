#ifndef SSTL_UNINITIALIZED_H
#define SSTL_UNINITIALIZED_H

#include "sstl_iterator.h"
#include "sstl_construct.h"
#include <algorithm>


namespace sstl {

/**
 * @brief   Copies elements from the range [first, last) to an uninitialized memory area
 */
template <class InputIterator, class OutputIterator>
inline OutputIterator uninitialized_copy(InputIterator first, InputIterator last, OutputIterator result) {
    return __uninitialized_copy(first, last, result, value_type<InputIterator>());
}

template <class InputIterator, class OutputIterator, class T>
inline OutputIterator __uninitialized_copy(InputIterator first, InputIterator last, OutputIterator result, T) {
    return __uninitialized_copy_aux(first, last, result, __type_traits<T>::is_POD_type());
}

template<class InputIterator, class OutputIterator>
inline OutputIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, OutputIterator result, __true_type) {
    return std::copy(first, last, result);
}

template<class InputIterator, class OutputIterator>
inline OutputIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, OutputIterator result, __false_type) {
    for(; first != last; first++, result++) {
        construct(&*result, *first);
    }
    return result;
}

/**
 * @brief   Copies the given value to an uninitialized memory area
 */
template <class InputIterator, class T>
inline void uninitialized_fill(InputIterator first, InputIterator last, const T& x) {
    __uninitialized_fill(first, last, x, value_type<first>());
}

template <class InputIterator, class T, class T2>
inline void __uninitialized_fill(InputIterator first, InputIterator last, const T& x, T2) {
    return __uninitialized_fill_aux(first, last, x, __type_traits<T2>::is_POD_type);
}

template <class InputIterator, class T>
inline void __uninitialized_fill_aux(InputIterator first, InputIterator last, const T& x, __true_type) {
    std::fill(first, last, x);
}

template <class InputIterator, class T>
inline void __uninitialized_fill_aux(InputIterator first, InputIterator last, const T& x, __false_type) {
    for( ; first != last; ++first) {
        construct(&*first, x);
    }
}

/**
 * @brief   Copies the given value value to the first count elements
 *          in an uninitialized memory area
 */
template <class InputIterator, class Distance, class T>
inline InputIterator uninitialized_fill_n(InputIterator first, Distance n, const T& x) {
    return __uninitialized_fill_n(first, n, x, value_type<first>());
}

template <class InputIterator, class Distance, class T, class T2>
inline InputIterator __uninitialized_fill_n(InputIterator first, Distance n, const T& x, T2) {
    return __uninitialized_fill_n_aux(first, n, x, __type_traits<T2>::is_POD_type());
}

template <class InputIterator, class Distance, class T>
inline InputIterator __uninitialized_fill_n_aux(InputIterator first, Distance n, const T& x, __true_type) {
    for( ; n > 0; ++first, --n) {
        *first = x;
    }
    return first;
}

template <class InputIterator, class Distance, class T>
inline InputIterator __uninitialized_fill_n_aux(InputIterator first, Distance n, const T& x, __false_type) {
    for( ; n > 0; ++first, --n) {
        construct(&*first, x);
    }
    return first;
}

} // sstl


#endif // SSTL_UNINITIALIZED_H
