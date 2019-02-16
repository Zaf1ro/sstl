#ifndef SSTL_ALGO_H
#define SSTL_ALGO_H

#include "sstl_iterator.h"
#include "sstl_algobase.h"


namespace sstl {

/**
 * @brief   Search the range for two consecutive
 *          identical elements
 * @param   __first, __last: the range of elements to examine
 */
template<class _ForwardIter>
_ForwardIter
adjacent_find(_ForwardIter __first, _ForwardIter __last)
{
    if(__first == __last)
        return __last;

    for(_ForwardIter next = __first; next != __last;
        ++next, ++__first)
        if(*__first == *next)
            return __first;
    return __last;
}

/**
 * @brief   Return the number of elements in the range
 *          satisfying specific criteria
 * @param   __first, __last: the range of elements to examine
 * @param   __val: the value to search
 */
template <class _InputIter, class _Tp>
typename __iterator_traits<_InputIter>::difference_type
count(_InputIter __first, _InputIter __last,
      const _Tp& __val)
{
    typename __iterator_traits<_InputIter>::difference_type n = 0;
    for(; __first != __last; ++__first)
        if(*__first == __val)
            ++n;
    return n;
}

/**
 * @brief   Return the number of elements in the range
 *          satisfying specific criteria
 * @param   __first, __last: the range of elements to examine
 * @param   __pred: unary predicate which return true for
 *          required elements
 */
template <class _InputIter, class _UnaryPred>
typename __iterator_traits<_InputIter>::difference_type
count_if(_InputIter __first, _InputIter __last,
         _UnaryPred __pred)
{
    typename __iterator_traits<_InputIter>::difference_type n = 0;
    for(; __first != __last; ++__first)
        if(__pred(*__first))
            ++n;
    return n;
}

/**
 * @brief   Return the first element in the range
 *          that satisfies specific criteria
 * @param   __first, __last: the range of elements to examine
 * @param   __val: value to compare the elements to
 */
template <class _InputIter, class _Tp>
_InputIter
find(_InputIter __first, _InputIter __last,
     const _Tp& __val)
{
    while(__first != __last && *__first != __val)
        ++__first;
    return __first;
}

/**
 * @brief   Return the first element in the range
 *          that satisfies specific criteria
 * @param   __first, __last: the range of elements to examine
 * @param   __pred: unary predicate which returns true
 *          for the required elements
 */
template <class _InputIter, class _UnaryPred>
_InputIter
find_if(_InputIter __first, _InputIter __last,
        _UnaryPred __pred)
{
    while(__first != __last && !__pred(*__first))
        ++__first;
    return __first;
}

/**
 * @brief   Search for the first occurrence of the
 *          sequence of elements[__first2, last2) in
 *          the range[__first1, __last1)
 * @param   __first1, __last1: the rang of elements to examine
 * @param   __first2, __last2: the range of elements to search for
 */
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1
search(_ForwardIter1 __first1, _ForwardIter1 __last1,
       _ForwardIter2 __first2, _ForwardIter2 __last2)
{
    if(__first1 == __last1 || __first2 == __last2)
        return __first1;

    _ForwardIter2 next, p;
    next = __first2;
    ++next;

    _ForwardIter1 current = __first1;

    while(__first1 != __last1) {
        __first1 = find(__first1, __last1, *__first2);
        if(__first1 == __last1)
            return __last1;

        p = next;
        _ForwardIter1 cur = __first1;
        while(++cur == *p) {
            if(++p == __last2)
                return __first1;
            if(++cur == __last1)
                return __last1;
        }
        ++__first1;
    }

    return __first1;
}

template <class _ForwardIter1, class _ForwardIter2>
inline _ForwardIter1
__find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
           _ForwardIter2 __first2, _ForwardIter2 __last2,
           forward_iterator_tag, forward_iterator_tag)
{
    if(__first2 == __last2)
        return __last1;
    else {
        _ForwardIter1 result = __last1;
        while(true) {
            _ForwardIter1 new_result = search(__first1, 
                    __last1, __first2, __last2);
            if(new_result == __last1)
                return result;
            else {
                result = __first1 = new_result;
                ++__first1;
            }
        }
    }
}

template <class _BidirectionalIter1,
          class _BidirectionalIter2>
inline _BidirectionalIter1
__find_end(_BidirectionalIter1 __first1,
           _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2,
           _BidirectionalIter2 __last2,
           bidirectional_iterator_tag,
           bidirectional_iterator_tag)
{
    typedef reverse_iterator<_BidirectionalIter1> reviter1;
    typedef reverse_iterator<_BidirectionalIter2> reviter2;
    reviter1 rlast1(__first1);
    reviter2 rlast2(__first2);

    reviter1 search_res = search(reviter1(__last1),
            rlast1, reviter2(__last2), rlast2);
    if(search_res == rlast1)
        return __last1;
    _BidirectionalIter1 res = search_res.base();
    __ADVANCE(res, -__DISTANCE(__first2, __last2));
    return res;
}

/**
 * @brief   Search for the last occurrence of the
 *          sequence [__first2, __last2) int the range
 *          [__first1, __last1]
 * @param   __first1, __last1: the range of elements to examine
 * @param   __first2, __last2: the range of elements to search for
 */
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
         _ForwardIter2 __first2, _ForwardIter1 __last2)
{
    typedef typename __iterator_traits<_ForwardIter1>::iterator_category category1;
    typedef typename __iterator_traits<_ForwardIter2>::iterator_category category2;
    return __find_end(__first1, __last1,
                      __first2, __last2,
                      category1(), category2());
}

/**
 * @brief   Searches the range [__first1, __last1)
 *          for any of the elements in the range
 *          [__first2, __last2)
 */
template <class _InputIter, class _ForwardIter>
_InputIter
find_first_of(_InputIter __first1, _InputIter __last1,
              _ForwardIter __first2, _ForwardIter __last2)
{
    for(; __first1 != __last1; ++__first1)
        for(_ForwardIter iter = __first2;
            iter != __last2; ++iter)
            if(*__first1 == *iter)
                return __first1;
    return __last1;
}

/**
 * @brief   Apply the given function object to the
 *          result of dereferencing every iterator in
 *          the range [__first, __last)
 * @param   __first, __last: the range to apply function
 * @param   __f: function object ot applied to the result
 *          of dereferencing every iterator in the range
 */
template <class _InputIter, class _Function>
_Function
for_each(_InputIter __first, _InputIter __last,
         _Function __f)
{
    for(; __first != __last; ++__first)
        __f(*__first);
    return __f;
}

/**
 * @brief   Assign each element in range [__first, __last)
 *          a value generated by the give function object
 * @param   __first, __last: the range of elements to generate
 * @param   __g: generator function object that will be called
 */
template <class _ForwardIter, class _Generator>
void
generate(_ForwardIter __first, _ForwardIter __last,
         _Generator __g)
{
    for(; __first != __last; ++__first)
        *__first = __g();
}

/**
 * @brief   Assign values generated by given function
 * @param   __first: the beginning of the range of elements
 * @param   __n: number of elements to generate
 * @param   __g: generator function object that will be called
 */
template <class _OutputIter, class _Size,
          class _Generator>
_OutputIter
generate_n(_OutputIter __first, _Size __n,
           _Generator __g)
{
    for(; __n > 0; --__n, ++__first)
        *__first = __g();
    return __first;
}

/**
 * @brief   Return true if the sorted range
 *          [__first1, __last1) is a subsequence
 *          of the sorted range [__first2, __last2)
 * @param   __first1, __last1: the sorted range of
 *          elements to examine
 * @param   __first2, __last2: the sorted range of
 *          elements to search for
 */
template <class _InputIter1, class _InputIter2>
bool
includes(_InputIter1 __first1, _InputIter1 __last1,
         _InputIter2 __first2, _InputIter2 __last2)
{
    while(__first1 != __last1 && __first2 != __last2)
        if(*__first2 < *__first1)
            return false;
        else if(*__first1 < *__first2)
            ++__first1;
        else
            ++__first1, ++__first2;
    return __first2 == __last2;
}

template <class _InputIter1, class _InputIter2,
          class _Compare>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2,
              _Compare __comp)
{
    while(__first1 != __last1 && __first2 != __last2)
        if(__comp(*__first2, *__first1))
            return false;
        else if(__comp(*__first1, *__first2))
            ++__first1;
        else
            ++__first1, ++__first2;
    return __first2 == __last2;
}

/**
 * @brief   Constructs a sorted union beginning at
 *          __result consisting of the set of elements
 *          present in one or both sorted ranges
 *          [__first1, __last1) and [__first2, __last2)
 * @param   __first1, __last1: the first input sorted range
 * @param   __first2, __last2: the second input sorted range
 * @param   __result: iterator to the beginning of
 *          union of two sorted ranges
 */
template <class _InputIter1, class _InputIter2,
          class _OutputIter>
_OutputIter
set_union(_InputIter1 __first1, _InputIter1 __last1,
          _InputIter2 __first2, _InputIter2 __last2,
          _OutputIter __result)
{
    while(__first1 != __last1 && __first2 != __last2) {
        if(*__first1 < *__first2) {
            *__result = *__first1;
            ++__first1;
        } else if(*__first2 < *__first1) {
            *__result = *__first2;
            ++__first2;
        } else {
            *__result = *__first1;
            ++__first1;
            ++__first2;
        }
        ++__result;
    }
    sstl::copy(__first2, __last2,
            sstl::copy(__first1, __last1, __result));
}

/**
 * @brief   Constructs a sorted range beginning at
 *          __result consisting of elements that are
 *          found in both sorted ranges
 *          [__first1, __last1) and [__first2, __last2)
 * @param   __first1, __last1: the first input sorted range
 * @param   __first2, __last2: the second input sorted range
 * @param   __result: iterator to the beginning of
 *          intersection of two sorted ranges
 */
template <class _InputIter1, class _InputIter2,
          class _OutputIter>
_OutputIter
set_intersection(_InputIter1 __first1,
                 _InputIter1 __last1,
                 _InputIter2 __first2,
                 _InputIter2 __last2,
                 _OutputIter __result)
{
    while(__first1 != __last1 && __first2 != __last2) {
        if(*__first1 < *__first2) {
            ++__first1;
        } else if(*__first2 < *__first1) {
            ++__first2;
        } else {
            *__result = *__first1;
            ++__first1;
            ++__first2;
            ++__result;
        }
    }
    return __result;
}

/**
 * @brief   Copies the elements from the sorted range
 *          [__first1, __last1) which are not found in
 *          the sorted range [__first2, __last2) to
 *          the range beginning at __result
 * @param   __first1, __last1: the range of elements
 *          to examine
 * @param   __first2, __last2: the range of elements
 *          to search for
 */
template <class _InputIter1, class _InputIter2,
          class _OutputIter>
_OutputIter
set_difference(_InputIter1 __first1,
               _InputIter1 __last1,
               _InputIter2 __first2,
               _InputIter2 __last2,
               _OutputIter __result)
{
    while(__first1 != __last1 && __first2 != __last2) {
        if(*__first1 < *__first2) {
            *__result = *__first1;
            ++__first1;
            ++__result;
        } else if(*__first2 < *__first1) {
            ++__first2;
        } else {
            ++__first1;
            ++__first2;
        }
    }
    return sstl::copy(__first1, __last1, __result);
}

/**
 * @brief   the elements that are found in either of
 *          the ranges, but not in both of them are
 *          copied to the range beginning at __result
 * @param   __first1, __last1: the first sorted range
 *          of elements
 * @param   __first2, __last2: the second sorted range
 *          of elements
 */
template <class _InputIter1, class _InputIter2,
          class _OutputIter>
_OutputIter
set_symmetric_difference(_InputIter1 __first1,
                         _InputIter1 __last1,
                         _InputIter2 __first2,
                         _InputIter2 __last2,
                         _OutputIter __result)
{
    while(__first1 != __last1 && __first2 != __last2) {
        if(*__first1 < *__first2) {
            *__result = *__first1;
            ++__first1;
            ++__result;
        } else if(*__first2 < *__first1) {
            *__result = *__first2;
            ++__first2;
            ++__result;
        } else {
            ++__first1;
            ++__first2;
        };
    }
    return sstl::copy(__first2, __last2,
            sstl::copy(__first1, __last1, __result));
}

} // sstl


#endif //SSTL_ALGO_H
