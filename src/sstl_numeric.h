#ifndef SSTL_NUMERIC_H
#define SSTL_NUMERIC_H

#include "sstl_iterator.h"
#include "sstl_functional.h"


namespace sstl {

/**
 * @brief   Return the result of accumulating all the
 *          values in the range [first,last) to __init
 * @param   __first, __last: Input iterators to the
 *          initial and final positions in a sequence.
 * @param   __init: Initial value for the accumulator
 */
template <class _InputIter, class _Tp>
_Tp
accumulate(_InputIter __first, _InputIter __last,
           _Tp __init)
{
    for(; __first != __last; ++__first)
        __init = __init + *__first;
    return __init;
}

/**
 * @brief   Assign to every element in the range
 *          starting at __result the difference between
 *          the range [first,last)
 * @param   __first, __last: Input iterators to the initial
 *          and final positions in a sequence
 * @param   __result: Output iterator to the initial position
 *          where the differences are stored
 */
template <class _InputIter, class _OutputIter>
_OutputIter
adjacent_difference(_InputIter __first,
                    _InputIter __last,
                    _OutputIter __result)
{
    if(__first == __last)
        return __result;
    *__result = *__first;

    typedef typename __iterator_traits<_InputIter>::value_type _Tp;
    _Tp prev = *__first;
    while(++__first != __last) {
        _Tp cur = *__first;
        *++__result = cur - prev;
        prev = cur;
    }
    return ++__result;
}

/**
 * @brief   Compute inner product or perform ordered
 *          map/reduce operation on the range
 * @param   __first1, __last1: the first range of elements
 * @param   __first2: the beginning of the second range of elements
 * @param   __init: initial value of the sum of products
 */
template <class _InputIter1, class _InputIter2, class _Tp>
_Tp
inner_product(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _Tp __init)
{
    for(; __first1 != __last1; ++__first1, ++__first2)
        __init = __init + (*__first1 * __first2);
    return __init;
}

/**
 * @brief   Compute the partial sums of the elements in
 *          the subrange of the range
 * @param   __first, __last: the range of elements to sum
 * @param   __result:
 */
template <class _InputIter, class _OutputIter>
_OutputIter
partial_sum(_InputIter __first, _InputIter __last,
            _OutputIter __result)
{
    if(__first == __last)
        return __result;
    *__result = *__first;
    typedef typename __iterator_traits<_InputIter>::value_type _Tp;
    _Tp sum = *__first;
    while(++__first != __last) {
        sum = sum + *__first;
        *++__result = sum;
    }
    return __result;
}

/**
 * @brief   Compute the value of __base raised to
 *          the power __exp
 * @param   __base: base as a value of floating-point
 *          or integral type
 * @param   __exp: exponent as a value of floating-point
 *          or integral type
 */
template <class _Tp, class _Integer>
inline _Tp
power(_Tp __base, _Integer __exp)
{
    if(__exp == 0) {
        return identity_element(multiplies<_Tp>());
    } else {
        while((__exp & 1) == 0) {
            __exp >>= 1;
            __base = sstl::multiplies<_Tp>(__base, __base);
        }
        _Tp result = __base;
        __exp >>= 1;
        while(__exp != 0) {
            __base = multiplies<_Tp>(__base, __base);
            if((__exp & 1) != 0)
                result = multiplies<_Tp>(result, __base);
            __exp >>= 1;
        }
        return result;
    }
}

/**
 * @brief   Fill the range with sequentially increasing
 *          values starting with value
 * @param   __first, __last: the range of elements to
 *          fill with sequentially increasing values
 */
template <class _ForwardIter, class _Tp>
void
iota(_ForwardIter __first, _ForwardIter __last,
     _Tp __val)
{
    while(__first != __last)
        *__first++ = __val++;
}

}

#endif // SSTL_NUMERIC_H
