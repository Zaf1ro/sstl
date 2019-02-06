#ifndef SSTL_HEAP_H
#define SSTL_HEAP_H

#include "sstl_traits.h"


namespace sstl {

/**
 * @param   __first: iterator to the first element
 * @param   __holeIndex: length of the range of elements
 * @param   __topIndex: distance of 0
 * @param   __val: value to insert
 */
template <class RandomAccessIter, class Distance, class T>
inline void
__push_heap(RandomAccessIter __first, 
            Distance __holeIndex,
            Distance topIndex, T __val)
{
    Distance parent = (__holeIndex - 1) / 2; // index of parent node
    while(__holeIndex > topIndex && *(__first + parent) < __val) {
        *(__first + __holeIndex) = *(__first + parent);
        __holeIndex = parent;
        parent = (__holeIndex - 1) / 2;
    }
    *(__first + __holeIndex) = __val;
}

/**
 * @brief   Get the type of distance and the type of
 *          iterator value
 */
template<class RandomAccessIter, class Distance, class T>
inline void
__push_heap_aux(RandomAccessIter __first, 
                RandomAccessIter __last,
                Distance*, T*)
{
    __push_heap(__first, DISTANCE(__last - __first - 1),
                Distance(0), T(*(__last - 1)));
}

/**
 * @brief   Insert the element at the position of last-1
 * @param   __first, __last: the range of elements
 *          defining the heap to modify
 */
template <class RandomAccessIter>
inline void
push_heap(RandomAccessIter __first,
          RandomAccessIter __last)
{
    __push_heap_aux(__first, __last, __DISTANCE_TYPE(__first),
                    ____val_TYPE(__first));
}

template <class RandomAccessIter, class Distance, class T>
inline void
__adjust_heap(RandomAccessIter __first, 
              Distance __holeIndex,
              Distance len, T __val)
{
    Distance topIndex = __holeIndex;
    Distance rightIndex = 2 * __holeIndex + 2; // second child of __holeIndex
    while(rightIndex < len) {
        // whether the left child is bigger than right child
        if(*(__first + rightIndex) < *(__first + rightIndex - 1))
            --rightIndex;
        *(__first + __holeIndex) = *(__first + rightIndex);
        __holeIndex = rightIndex;
        rightIndex = 2 * rightIndex + 2;
    }
    if(rightIndex == len) { // only has left child
        *(__first + __holeIndex) = *(__first + (rightIndex - 1));
        __holeIndex = rightIndex - 1;
    }
    *(__first + __holeIndex) = __val;
}

template <class RandomAccessIter, class Distance, class T>
inline void
__pop_heap(RandomAccessIter __first,
           RandomAccessIter __last,
           RandomAccessIter __result,
           T __val, Distance*)
{
    *__result = *__first;
    __adjust_heap(__first, Distance(0),
            Distance(__last - __first), __val);
}

template <class RandomAccessIter, class T>
inline void
__pop_heap_aux(RandomAccessIter __first,
         RandomAccessIter __last, T*)
{
    __pop_heap(__first, __last-1, __last-1,
               T(*(__last-1)), __DISTANCE_TYPE(__first));
}

/**
 * @brief   Swap th value in the position first and the value
 *          in the position last-1 and make the subrange
 *          [first, last-1] into a max heap
 * @param   __first, __last: the range of elements to modify
 */
template <class RandomAccessIter>
inline void
pop_heap(RandomAccessIter __first,
         RandomAccessIter __last)
{
    __pop_heap_aux(__first, __last, __VALUE_TYPE(__first));
}

/**
 * @brief   Convert the max heap [first, last) into a sorted range
 *          in ascending order
 */
template <class RandomAccessIter>
inline void
sort_heap(RandomAccessIter __first,
          RandomAccessIter __last)
{
    while(__last - __first > 1)
        pop_heap(__first, __last--);
}

template <class RandomAccessIter, class Distance, class T>
inline void
__make_heap(RandomAccessIter __first,
            RandomAccessIter __last,
            T*, Distance*)
{
    if(__last - __first < 2)
        return;
    Distance len = __last - __first;
    Distance parent = (len - 2) / 2;

    while(true) {
        __adjust_heap(__first, parent, len, T(*(__first + parent)));
        if(parent == 0)
            return;
        --parent;
    }
}

/**
 * @brief   Construct a max heap in the range of [first, last)
 * @param   __first, __last: the range of elements to make the heap form
 */
template <class RandomAccessIter>
inline void
make_heap(RandomAccessIter __first,
          RandomAccessIter __last)
{
    __make_heap(__first, __last, __VALUE_TYPE(__first),
                __DISTANCE_TYPE(__first));
}

} // sstl


#endif // SSTL_HEAP_H
