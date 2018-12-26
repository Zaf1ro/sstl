#ifndef SSTL_ALGOBASE_H
#define SSTL_ALGOBASE_H


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

} // sstl


#endif // SSTL_ALGOBASE_H
