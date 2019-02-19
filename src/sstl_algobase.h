#ifndef SSTL_ALGOBASE_H
#define SSTL_ALGOBASE_H

#include "sstl_iterator.h"
#include "sstl_traits.h"
#include "sstl_config.h"
#include <iostream>
#include <string>


namespace sstl {

/**
 * @brief   _Compares the elements in the range [first1,last1)
 *          with those in the range beginning at first2
 * @param   __first1: iterator to the beginning of the first container
 * @param   __last1: iterator to the end of the first container
 * @param   __first2: iterator to the beginning of the second container
 */
template <class _InputIter1, class _InputIter2>
inline bool
equal(_InputIter1 __first1, _InputIter1 __last1,
      _InputIter2 __first2)
{
    for( ; __first1 != __last1; ++__first1, ++__first2) {
        if( *__first1 != *__first2 ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief   Same as equal(first1, last1, first2)
 * @param   __bp: _Compare two parameters and represent
 *          the truth or falsehood on some condition
 */
template <class _InputIter1, class _InputIter2,
          class _BinaryPred>
inline bool
euqal(_InputIter1 __first1, _InputIter1 __last1,
      _InputIter2 __first2, _BinaryPred __bp) {
    for( ; __first1 != __last1; ++__first1, ++__first2)
        if(!__bp(*__first1, *__first2))
            return false;
    return true;
}

/**
 * @brief   Assign 'value' to all the elements in the range [first,last)
 * @param   __first: iterator to the beginning of container
 * @param   __last: iterator to the end of container
 * @param   __val: value to assign to the elements
 */
template <class _InputIter, class _Tp>
inline void
fill(_InputIter __first, _InputIter __last,
     const _Tp& __val)
{
    for( ; __first != __last; ++__first)
        *__first = __val;
}

/**
 * @brief   Assigns 'value' to the first n elements of
 *          the container pointed by first
 * @param   __first: iterator to the beginning of container
 * @param   __n: number of elements to fill
 * @param   __val: value to assigned to the elements
 */
template <class _InputIter, class _Size, class _Tp>
inline void
fill_n(_InputIter __first, _Size __n, const _Tp& __val) 
{
    for( ; __n > 0; ++__first, --__n)
        *__first = __val;
}

/**
 * @brief   Exchange values of objects pointed to by two iterators
 * @param   __it1, __it2: iterator to the object to swap
 */
template <class _ForwardIter>
inline void 
iter_swap(_ForwardIter __it1, _ForwardIter __it2) {
    typename __iterator_traits<_ForwardIter>::value_type tmp = *__it1;
    *__it1 = *__it2;
    *__it2 = tmp;
}

/**
 * @brief   Exchange the values of a and b
 * @param   __a, __b: values to swap
 */
template <class _Tp>
inline void swap(const _Tp& __a, const _Tp& __b) {
    _Tp tmp = __a;
    __a = __b;
    __b = tmp;
}

/**
 * @brief   Return the largest of a and b
 * @param   __a, __b: value to compare
 */
template <class _Tp>
inline const _Tp& max(const _Tp& __a, const _Tp& __b) 
{ return __a < __b ? __b : __a; }

/**
 * @brief   Same as max(a, b)
 * @param   __comp: Binary function that accepts two values
 *          of type T, and returns a value convertible to bool
 */
template <class _Tp, class _Compare>
inline const _Tp& 
max(const _Tp& __a, const _Tp& __b, _Compare __comp) 
{ return comp(__a, __b) ? __b : __a; }

/**
 * @brief   Return the smallest of a and b
 * @param   __a, __b: values to compare
 */
template <class _Tp>
inline const _Tp&
min(const _Tp& __a, const _Tp& __b)
{ return __b < __a ? __b : __a; }

/**
 * @brief   Same as min(a, b)
 * @param   __comp: Binary function that accepts two values
 *          of type T, and returns a value convertible to bool
 */
template <class _Tp, class _Compare>
inline const _Tp& 
min(const _Tp& __a, const _Tp& __b, _Compare __comp) {
    return comp(__b, __a) ? __b : __a;
}

template <class _InputIter, class _OutputIter, class _Distance>
inline _OutputIter
__copy(_InputIter __first, _InputIter __last,
       _OutputIter __result, input_iterator_tag,
       _Distance*)
{
    for( ; __first != __last; ++__first, ++__result)
        *__result = *__first;
    return __result;
}

template <class _RandomAccessIter, class _OutputIter, class _Distance> inline _OutputIter
__copy(_RandomAccessIter __first, _RandomAccessIter __last,
       _OutputIter __result, random_access_iterator_tag,
       _Distance*)
{
    for(_Distance n = __last - __first; n > 0;
        --n, ++__result, ++__first)
        *__result = *__first;
    return __result;
}

template <class _Tp>
inline _Tp*
__copy_trivial(const _Tp* __first, const _Tp* __last,
               _Tp* __result)
{
    memmove(__result, __first, sizeof(_Tp) * (__last - __first));
    return __result + (__last - __first);
}

template <class _InputIter, class _OutputIter>
inline _OutputIter
__copy_dispatch(_InputIter __first, _InputIter __last,
                _OutputIter __result, __true_type)
{
    return __copy(__first, __last, __result,
            __ITERATOR_CATEGORY(__first),
            __DISTANCE_TYPE(__first));
}

template <class _InputIter, class _OutputIter>
inline _OutputIter
__copy_dispatch(_InputIter __first, _InputIter __last,
                _OutputIter __result, __false_type)
{
    return __copy(__first, __last, __result,
            __ITERATOR_CATEGORY(__first),
            __DISTANCE_TYPE(__first));
}

/* partial template specialization for POD pointer */
template <class _Tp>
inline _Tp*
__copy_dispatch(_Tp* __first, _Tp* __last,
                _Tp* __result, __true_type)
{
    return __copy_trivial(__first, __last, __result);
}

/* has trivial assignment operator or not */
template <class _InputIter, class _OutputIter, class _Tp>
inline _OutputIter
__copy_aux(_InputIter __first, _InputIter __last,
           _OutputIter __result, _Tp*)
{
    typedef typename __type_traits<_Tp>::has_trivial_assignment_operator _Trivial;
    return __copy_dispatch(__first, __last, __result, _Trivial());
}

/**
 * @brief   Copies the elements in the range [first,last)
 *          into the range beginning at result
 */
template <class _InputIter, class _OutputIter> 
inline _OutputIter 
copy(_InputIter __first, _InputIter __last, 
     _OutputIter __result)
{
    return __copy_aux(__first, __last, __result, 
            __VALUE_TYPE(__first));
}

template <class _BidirectionalIter1,
        class _BidirectionalIter2,
        class _Distance>
inline _BidirectionalIter2
__copy_backward(_BidirectionalIter1 __first,
                _BidirectionalIter1 __last,
                _BidirectionalIter2 __result,
                bidirectional_iterator_tag,
                _Distance*)
{
    while ( __first != __last )
        *--__result = *--__last;
    return __result;
}

template <class _BidirectionalIter1,
        class _BidirectionalIter2,
        class _Distance>
inline _BidirectionalIter2
__copy_backward(_BidirectionalIter1 __first,
                _BidirectionalIter1 __last,
                _BidirectionalIter2 __result,
                random_access_iterator_tag,
                _Distance*)
{
    for(_Distance n = __last - __first; n > 0; --n)
        *--__result = *--__last;
    return __result;
}

template <class _BidirectionalIter1,
        class _BidirectionalIter2,
        class _BoolType>
inline _BidirectionalIter2
__copy_backward_dispatch(_BidirectionalIter1 __first,
                         _BidirectionalIter1 __last,
                         _BidirectionalIter2 __result,
                         _BoolType)
{
    return __copy_backward(__first, __last, __result,
                           __ITERATOR_CATEGORY(__first),
                           __DISTANCE_TYPE(__first));
}

template <class _Tp>
inline _Tp*
__copy_backward_dispatch(_Tp* __first, _Tp* __last,
                         _Tp* __result, __true_type)
{
    const ptrdiff_t n = __last - __first;
    memmove(__result - n, __first, sizeof(_Tp) * n);
    return __result - n;
}

/**
 * @brief   Copies the elements from the range [first, last) to another range ending at d_last.
 *          The elements are copied in reverse order
 */
template <class _BidirectionalIter1,
          class _BidirectionalIter2>
inline _BidirectionalIter2
copy_backward(_BidirectionalIter1 __first,
              _BidirectionalIter1 __last,
              _BidirectionalIter2 __result)
{
    typedef typename __type_traits<_BidirectionalIter1>::has_trivial_assignment_operator _Trivial;
    return __copy_backward_dispatch(__first, __last,
                                    __result, _Trivial());
}

} // sstl


#endif // SSTL_ALGOBASE_H
