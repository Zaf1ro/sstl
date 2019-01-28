#ifndef SSTL_ALGO_H
#define SSTL_ALGO_H

#include "sstl_iterator.h"


namespace sstl {

template<class ForwardIter>
ForwardIter adjacent_find(ForwardIter first, ForwardIter last)
{
    if(first == last)
        return last;

    while(++next != last) {
        if(*first == * next)
            return first;
        first = next;
    }
    return last;
}

template <class InputIter, class T>
typename __iterator_traits<InputIter>::difference_type
count(InputIter first, InputIter last, const T& value)
{
    typename __iterator_traits<InputIter>::difference_type n = 0;
    for(; first != last; ++first)
        if(*first == value)
            ++n;
    return n;
}

template <class InputIter, class Predicate>
template __iterator_traits<InputIter>::difference_type
count_if(InputIter first, InputIter last, Predicate pred)
{
    typename __iterator_traits<InputIter>::difference_type n = 0;
    for(; first != last; ++first)
        if(pred(*first))
            ++n;
    return n;
}

template <class InputIter, class T>
InputIter find(InputIter first, InputIter last, const T& value)
{
    while(first != last && *first != value)
        ++first;
    return first;
}

template <class InputIter, class Predicate>
InputIter find_if(InputIter first, InputIter last, Predicate pred)
{
    while(first != last && !pred(*first))
        ++first;
    return first;
}

template <class ForwardIter1, class ForwardIter2>
ForwardIter1
search(ForwardIter1 first1, ForwardIter1 last1,
       ForwardIter2 first2, ForwardIter2 last2)
{
    if(first1 == last1 || first2 == last2)
        return first1;

    ForwardIter2 next, p;
    next = first2;
    ++next;

    ForwardIter1 current = first1;

    while(first1 != last1) {
        first1 = find(first1, last1, *first2);
        if(first1 == last1)
            return last1;

        p = next;
        current = first1;
        while(++current == *p) {
            if(++p == last2)
                return first1;
            if(++current == last1)
                return last1;
        }
        ++first1;
    }

    return first1;
}

template <class ForwardIter1, class ForwardIter2>
inline ForwardIter1
__find_end(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2,
           forward_iterator_tag, forward_iterator_tag)
{
    if(first2 == last2)
        return last1;
    else {
        ForwardIter1 result = last1;
        while(1) {
            ForwardIter1 new_result = search(first1, last1, first2, last2);
            if(new_result == last1)
                return result;
            else {
                result = first1 = new_result;
                ++first1;
            }
        }
    }
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator1
__find_end(BidirectionalIterator1 first1, BidirectionalIterator1 last1,
           BidirectionalIterator2 first2, BidirectionalIterator2 last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
    typedef reverse_iterator<BidirectionalIterator1> reviter1;
    typedef reverse_iterator<BidirectionalIterator2> reviter2;
    reviter1 rlast1(first1);
    reviter2 rlast2(first2);

    reviter1 search_res = search(reviter1(last1), rlast1,
                             reviter2(last2), rlast2);
    if(search_res == rlast1)
        return last1;
    BidirectionalIterator1 res = search_res.base();
    __ADVANCE(res, -__DISTANCE(first2, last2));
    return res;
}

template <class ForwardIter1, class ForwardIter2>
ForwardIter1
find_end(ForwardIter1 first1, ForwardIter1 last1,
         ForwardIter2 first2, ForwardIter1 last2)
{
    typedef typename __iterator_traits<ForwardIter1>::iterator_category category1;
    typedef typename __iterator_traits<ForwardIter2>::iterator_category category2;
    return __find_end(first1, last1, first2, last2, category1(), category2());
}

template <class InputIter, class ForwardIter>
InputIter find_first_of(InputIter first1, InputIter last1,
                        ForwardIter first2, ForwardIter last2)
{
    for(; first1 != last1; ++first1)
        for(ForwardIter iter = first2; iter != last2; ++iter)
            if(*firs1 == *iter)
                return first1;
    return last1;
}

template <class InputIter, class Function>
Function for_each(InputIter first, InputIter last, Function f)
{
    for(; first != last; ++first)
        f(*first);
    return f;
}

template <class ForwardIter, class Generator>
void generate(ForwardIter first, ForwardIter last, Generator gen)
{
    for(; first != last; ++first)
        *first = gen();
}

template <class OutputIter, class Size, Class Generator>
OutputIter generate_n(OutputIter first, Size n, Generator gen)
{
    for(; n > 0; --n, ++first)
        *first = gen();
    return first;
}

template <class InputIter1, class InputIter2>
bool include(InputIter1 first1, InputIter1 last1,
             InputIter2 first2, InputIter2 last2)
{
    while(first1 != last1 && first2 != last2)
        if(*first2 < *first1)
            return false;
        else if(*first1 < *first2)
            ++first1;
        else
            ++first1, ++first2;
    return first2 == last2;
}

template <class InputIter1, class InputIter2, class Compare>
bool includes(InputIter1 first1, InputIter1 last1,
              InputIter2 first2, InputIter2 last2,
              Compare comp)
{
    while(first1 != last1 && first2 != last2)
        if(comp(*first2, *first1))
            return false;
        else if(comp(*first1, *first2))
            ++first1;
        else
            ++first1, ++first2;
    return first2 == last2;
}

} // sstl


#endif //SSTL_ALGO_H
