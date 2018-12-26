#ifndef SSTL_ALGOBASE_H
#define SSTL_ALGOBASE_H

#include "sstl_iterator.h"


namespace sstl {

/**
 * @brief   Compares the elements in the range [first1,last1)
 *          with those in the range beginning at first2
 * @param   first1: iterator to the beginning of the first container
 * @param   last1: iterator to the end of the first container
 * @param   first2: iterator to the beginning of the second container
 */
template <class InputIterator1, class InputIterator2>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    for( ; first1 != last1; ++first1, ++first2) {
        if( *first1 != *first2 ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief   Same as equal(first1, last1, first2)
 * @param   binary_pred: Compare two parameters and represent
 *          the truth or falsehood on some condition
 */
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool euqal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate binary_pred) {
    for( ; first1 != last1; ++first1, ++first2) {
        if(!binary_pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

template <class InputIterator, class T>
inline void fill(InputIterator first, InputIterator last, const T& value) {
    for( ; first != last; ++first) {
        *first = value;
    }
}

template <class InputIterator, class Size, class T>
inline void fill_n(InputIterator first, Size n, const T& value) {
    for( ; n > 0; ++first, --n) {
        *first = value;
    }
}

template <class InputIterator>
inline void iter_swap(InputIterator iter1, InputIterator iter2) {
    typename iterator_traits<InputIterator>::value_type tmp = *iter1;
    *iter1 = *iter2;
    *iter2 = tmp;
}

template <class T>
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

template <class T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

} // sstl


#endif // SSTL_ALGOBASE_H
