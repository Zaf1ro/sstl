#ifndef SSTL_ITERATOR_H
#define SSTL_ITERATOR_H

#include <cstddef>


namespace sstl {

#define __ITERATOR_CATEGORY(_Iter)  __iterator_category(_Iter)
#define __DISTANCE_TYPE(_Iter)      __distance_type(_Iter)
#define __VALUE_TYPE(_Iter)         __value_type(_Iter)
#define __DISTANCE(Iter1, Iter2)    __distance(Iter1, Iter2)
#define __ADVANCE(Iter, n)          __advance(Iter, n)

// type of iterator determines the type of distance() and advance()
struct input_iterator_tag {};       // read-only
struct output_iterator_tag {};      // write-only
struct forward_iterator_tag:        // forward direction with read & write
        public input_iterator_tag, public output_iterator_tag {};
struct bidirectional_iterator_tag:  // both direction with read & write
        public forward_iterator_tag {};
struct random_access_iterator_tag:  // all pointer operation with read & write
        public bidirectional_iterator_tag{};

/**
 * @brief   determine certain properties of the iterator
 */
template <class Iter>
struct __iterator_traits {
    typedef typename Iter::value_type           value_type;
    typedef typename Iter::iterator_category    iterator_category;
    typedef typename Iter::difference_type      difference_type;
    typedef typename Iter::pointer_type         pointer_type;
    typedef typename Iter::reference_type       reference_type;
};

/**
 * @brief   partial specialization for pointer type
 */
template <class T>
struct __iterator_traits<T*> {
    typedef T           value_type;         // type of value
    typedef T*          iterator_category;  // type of sstl_iterator
    typedef ptrdiff_t   difference_type;    // type of distance between iterators
    typedef T*          pointer_type;       // type of pointer of value
    typedef T&          reference_type;     // type of reference of value
};

/**
 * @brief   partial specialization for const pointer type
 */
template <class T>
struct __iterator_traits<const T*> {
    typedef T           value_type;
    typedef T*          iterator_category;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer_type;
    typedef T&          reference_type;
};


/**
 * @brief   return an object of type of iterator
 */
template <class Iter> inline auto
__iterator_category(const Iter&)
-> typename __iterator_traits<Iter>::iterator_category
{
    typedef typename __iterator_traits<Iter>::iterator_category _Category;
    return _Category();
}

/**
 * @brief   return a pointer to the type of difference between iterators
 */
template <class Iter> inline auto
__distance_type(const Iter)
-> typename __iterator_traits<Iter>::difference_type*
{
    return static_cast<typename __iterator_traits<Iter>::difference_type*>(0);
}

/**
 * @brief   return a pointer to the type of value
 */
template <class Iter> inline auto
__value_type(const Iter&)
-> typename __iterator_traits<Iter>::value_type*
{
    return static_cast<typename __iterator_traits<Iter>::value_type*>(0);
}

/**
 * @brief   return the distance between two iterators
 */
template <class Iter> inline auto
__distance(Iter first, Iter last)
-> typename __iterator_traits<Iter>::difference_type
{
    typedef typename __iterator_traits<Iter>::iterator_category Category;
    return __distance_aux(first, last, Category());
}

template <class InputIter> inline auto
__distance_aux(InputIter first, InputIter last, input_iterator_tag)
-> typename __iterator_traits<InputIter>::difference_type
{
    typename __iterator_traits<InputIter>::difference_type n = 0;
    while (first != last) {
        ++n;
        ++first;
    }
    return n;
}

template <class RandomAccessIter> inline auto
__distance_aux(RandomAccessIter first, RandomAccessIter last, random_access_iterator_tag)
-> typename __iterator_traits<RandomAccessIter>::difference_type
{
    return last - first;
}

/**
 * @brief   advances the iterator it by n element positions.
 */
template <class Iter, class Distance> inline void
__advance(Iter& i, Distance n)
{
    __advance_aux(i, n, __ITERATOR_CATEGORY(i));
}

template <class InputIter, class Distance> inline void
__advance_aux(InputIter& i, Distance n, input_iterator_tag)
{
    while( n-- ) ++i;
}

template <class BidirectionalIter, class Distance> inline void
__advance_aux(BidirectionalIter& i, Distance n, bidirectional_iterator_tag)
{
    if( n >= 0 )
        while( n -- ) ++i;
    else
        while( n++ ) --i;
}

template <class RandomAccessIter, class Distance> inline void
__advance_aux(RandomAccessIter& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

/**
 * @brief   Class iterator
 */
template <class T,
          class Category,
          class Distance = ptrdiff_t,
          class Pointer = T*,
          class Reference = T&>
class iterator {
private:
    typedef T           value_type;
    typedef Category    iterator_category;
    typedef Distance    difference_type;
    typedef Pointer     pointer_type;
    typedef Reference   reference_type;
};

} // sstl


#endif // SSTL_ITERATOR_H
