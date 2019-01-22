#ifndef SSTL_ALGOBASE_H
#define SSTL_ALGOBASE_H

#include "sstl_iterator.h"
#include "sstl_traits.h"
#include "sstl_config.h"
#include <iostream>
#include <string>


namespace sstl {

/**
 * @brief   Compares the elements in the range [first1,last1)
 *          with those in the range beginning at first2
 * @param   first1: iterator to the beginning of the first container
 * @param   last1: iterator to the end of the first container
 * @param   first2: iterator to the beginning of the second container
 */
template <class InputIter1, class InputIter2>
inline bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
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
template <class InputIter1, class InputIter2, class BinaryPredicate>
inline bool euqal(InputIter1 first1, InputIter1 last1, InputIter2 first2, BinaryPredicate binary_pred) {
    for( ; first1 != last1; ++first1, ++first2) {
        if(!binary_pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief   Assign 'value' to all the elements in the range [first,last)
 * @param   first: iterator to the beginning of container
 * @param   last: iterator to the end of container
 * @param   value: value to assign to the elements
 */
template <class InputIter, class T>
inline void fill(InputIter first, InputIter last, const T& value) {
    for( ; first != last; ++first) {
        *first = value;
    }
}

/**
 * @brief   Assigns 'value' to the first n elements of
 *          the container pointed by first
 * @param   first: iterator to the beginning of container
 * @param   n: number of elements to fill
 * @param   value: value to assigned to the elements
 */
template <class InputIter, class Size, class T>
inline void fill_n(InputIter first, Size n, const T& value) {
    for( ; n > 0; ++first, --n) {
        *first = value;
    }
}

/**
 * @brief   Exchange values of objects pointed to by two iterators
 * @param   iter1, iter2: iterator to the object to swap
 */
template <class ForwardIterator>
inline void iter_swap(ForwardIterator iter1, ForwardIterator iter2) {
    typename __iterator_traits<ForwardIterator>::value_type tmp = *iter1;
    *iter1 = *iter2;
    *iter2 = tmp;
}

/**
 * @brief   Exchange the values of a and b
 * @param   a, b: values to swap
 */
template <class T>
inline void swap(const T& a, const T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

/**
 * @brief   Return the largest of a and b
 * @param   a, b: value to compare
 */
template <class T>
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

/**
 * @brief   Same as max(a, b)
 * @param   comp: Binary function that accepts two values of type T,
 *          and returns a value convertible to bool
 */
template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

/**
 * @brief   Return the smallest of a and b
 * @param   a, b: values to compare
 */
template <class T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

/**
 * @brief   Same as min(a, b)
 * @param   comp: Binary function that accepts two values of type T,
 *          and returns a value convertible to bool
 */
template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

template <class InputIter, class OutputIter, class Distance> inline OutputIter
__copy(InputIter first, InputIter last, OutputIter result, input_iterator_tag, Distance*)
{
    for( ; first != last; ++first, ++result) {
        *result = *first;
    }
    return result;
}

template <class RandomAccessIter, class OutputIter, class Distance> inline OutputIter
__copy(RandomAccessIter first, RandomAccessIter last, OutputIter result, random_access_iterator_tag, Distance*)
{
    for(Distance n = last - first; n > 0; --n, ++result, ++first) {
        *result = *first;
    }
    return result;
}

template <class T> inline T*
__copy_trivial(const T* first, const T* last, T* result)
{
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template <class InputIter, class OutputIter> inline OutputIter
__copy_dispatch(InputIter first, InputIter last, OutputIter result, __true_type)
{
    return __copy(first, last, result, __ITERATOR_CATEGORY(first), __DISTANCE_TYPE(first));
}

template <class InputIter, class OutputIter> inline OutputIter
__copy_dispatch(InputIter first, InputIter last, OutputIter result, __false_type)
{
    return __copy(first, last, result, __ITERATOR_CATEGORY(first), __DISTANCE_TYPE(first));
}

/* partial template specialization for POD pointer */
template <class T> inline T*
__copy_dispatch(T* first, T* last, T* result, __true_type)
{
    return __copy_trivial(first, last, result);
}

/* has trivial assignment operator or not */
template <class InputIter, class OutputIter, class T> inline OutputIter
__copy_aux(InputIter first, InputIter last, OutputIter result, T*)
{
    typedef typename __type_traits<T>::has_trivial_assignment_operator Trivial;
    return __copy_dispatch(first, last, result, Trivial());
}

/**
 * @brief   Copies the elements in the range [first,last)
 *          into the range beginning at result
 */
template <class InputIter, class OutputIter> inline OutputIter 
copy(InputIter first, InputIter last, OutputIter result)
{
    return __copy_aux(first, last, result, __VALUE_TYPE(first));
}

/**
 * @brief   Copies the elements from the range [first, last) to another range ending at d_last.
 *          The elements are copied in reverse order
 */
template <class BidirectionalIter1, class BidirectionalIter2> inline BidirectionalIter2
copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
    typedef typename __type_traits<BidirectionalIter1>::has_trivial_assignment_operator Trivial;
    return __copy_backward_dispatch(first, last, result, Trivial());
}

template <class BidirectionalIter1, class BidirectionalIter2, class BoolType> inline BidirectionalIter2
__copy_backward_dispatch(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result, BoolType)
{
    return __copy_backward(first, last, result, __ITERATOR_CATEGORY(first), __DISTANCE_TYPE(first));
}

template <class T> inline T*
__copy_backward_dispatch(T* first, T* last, T* result, __true_type)
{
    const ptrdiff_t n = last - first;
    memmove(result - n, first, sizeof(T) * n);
    return result - n;
}

template <class BidirectionalIter1, class BidirectionalIter2, class Distance> inline BidirectionalIter2
__copy_backward(BidirectionalIter1 first,
                BidirectionalIter1 last,
                BidirectionalIter2 result,
                bidirectional_iterator_tag,
                Distance*)
{
    while ( first != last ) {
        *--result = *--last;
    }
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2, class Distance> inline BidirectionalIter2
__copy_backward(BidirectionalIter1 first,
                BidirectionalIter1 last,
                BidirectionalIter2 result,
                random_access_iterator_tag,
                Distance*)
{
    for(Distance n = last - first; n > 0; --n) {
        *--result = *--last;
    }
    return result;
}

} // sstl


#endif // SSTL_ALGOBASE_H
