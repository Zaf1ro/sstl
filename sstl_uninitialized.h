#ifndef SSTL_UNINITIALIZED_H
#define SSTL_UNINITIALIZED_H

#include "sstl_iterator.h"
#include "sstl_construct.h"
#include "sstl_traits.h"
#include "sstl_algobase.h"


namespace sstl {

template <class InputIter, class OutputIter, class T> inline OutputIter
__uninitialized_copy(InputIter first, InputIter last, OutputIter result, T*)
{
    typedef typename __type_traits<T>::is_POD_type Is_POD;
    return __uninitialized_copy_aux(first, last, result, Is_POD());
}

template<class InputIter, class OutputIter> inline OutputIter
__uninitialized_copy_aux(InputIter first, InputIter last, OutputIter result, __true_type)
{
    return sstl::copy(first, last, result);
}

template<class InputIter, class OutputIter> inline OutputIter
__uninitialized_copy_aux(InputIter first, InputIter last, OutputIter result, __false_type)
{
    for(; first != last; ++first, ++result) {
        construct(&*result, *first);
    }
    return result;
}

/**
 * @brief   Copies elements from the range [first, last) to an uninitialized memory area
 */
template <class InputIter, class OutputIter> inline OutputIter
uninitialized_copy(InputIter first, InputIter last, OutputIter result)
{
    return __uninitialized_copy(first, last, result, __VALUE_TYPE(first));
}


template <class InputIter, class T> inline void
__uninitialized_fill_aux(InputIter first, InputIter last, const T& x, __false_type)
{
    for( ; first != last; ++first) {
        construct(&*first, x);
    }
}

template <class InputIter, class T, class T2> inline void
__uninitialized_fill(InputIter first, InputIter last, const T& x, T2*)
{
    typedef typename __type_traits<T2>::is_POD_type Is_POD;
    return __uninitialized_fill_aux(first, last, x, Is_POD());
}

template <class InputIter, class T> inline void
__uninitialized_fill_aux(InputIter first, InputIter last, const T& x, __true_type)
{
    std::fill(first, last, x);
}

/**
 * @brief   Copies the given value to an uninitialized memory area
 */
template <class InputIter, class T> inline void
uninitialized_fill(InputIter first, InputIter last, const T& x)
{
    __uninitialized_fill(first, last, x, __VALUE_TYPE(first));
}


template <class InputIter, class Distance, class T> inline InputIter
__uninitialized_fill_n_aux(InputIter first, Distance n, const T& x, __true_type)
{
    for( ; n > 0; ++first, --n)
        *first = x;
    return first;
}

template <class InputIter, class Distance, class T> inline InputIter
__uninitialized_fill_n_aux(InputIter first, Distance n, const T& x, __false_type)
{
    for( ; n > 0; ++first, --n)
        construct(&*first, x);
    return first;
}

template <class InputIter, class Distance, class T, class T2> inline InputIter
__uninitialized_fill_n(InputIter first, Distance n, const T& x, T2)
{
    typedef typename __type_traits<T2>::is_POD_type Is_POD;
    return __uninitialized_fill_n_aux(first, n, x, Is_POD());
}

/**
 * @brief   Copies the given value value to the first count elements
 *          in an uninitialized memory area
 */
template <class InputIter, class Distance, class T> inline InputIter
uninitialized_fill_n(InputIter first, Distance n, const T& x)
{
    return __uninitialized_fill_n(first, n, x, __VALUE_TYPE(first));
}

} // sstl


#endif // SSTL_UNINITIALIZED_H
