#ifndef SSTL_ITERATOR_H
#define SSTL_ITERATOR_H

#include "sstl_traits.h"


namespace sstl {

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
 * @brief   return the type of iterator
 */
template <class Iterator>
inline auto iterator_category()
-> typename iterator_traits<Iterator>::iterator_category {
    return iterator_traits<Iterator>::iterator_category();
}

/**
 * @brief   return the type of difference between iterators
 */
template <class Iterator>
inline auto difference_type()
-> typename iterator_traits<Iterator>::difference_type {
    return iterator_traits<Iterator>::difference_type(0);
}

/**
 * @brief   return an empty object of the type of value
 */
template <class Iterator>
inline auto value_type()
-> typename iterator_traits<Iterator>::value_type {
    return iterator_traits<Iterator>::value_type(0);
}

/**
 * @brief   return an empty object distance between two iterators
 */
template <class Iterator>
inline auto distance(Iterator first, Iterator last)
-> typename iterator_traits<Iterator>::difference_type {
    return __distance(first, last, iterator_category<Iterator>());
}

/**
 * @brief   __distance() function overloading for Input Iterator
 */
template <class InputIterator>
inline auto __distance(InputIterator first, InputIterator last, input_iterator_tag)
-> typename iterator_traits<InputIterator>::difference_type {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++n;
        ++first;
    }
    return n;
}

/**
 * @brief   __distance() function overloading for Random Access Iterator
 */
template <class RandomAccessIterator>
inline auto __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
-> typename iterator_traits<RandomAccessIterator>::difference_type {
    return last - first;
}

/**
 * @brief   advances the iterator it by n element positions.
 */
template <class Iterator, class Distance>
inline void advance(Iterator& i, Distance n) {
    __advance(i, n, iterator_category<Iterator>());
}

/**
 * @brief   __advance() function overloading for Input Iterator
 */
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while( n-- ) {
        ++i;
    }
}

/**
 * @brief   __advance() function overloading for Bidirectional Iterator
 */
template <class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if( n >= 0 ) {
        while( n -- ) {
            i++;
        }
    } else {
        while( n++ ) {
            --i;
        }
    }
}

/**
 * @brief   __advance() function overloading for Random Access Iterator
 */
template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
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
