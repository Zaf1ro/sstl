#ifndef SSTL_HEAP_H
#define SSTL_HEAP_H

#include "sstl_traits.h"


namespace sstl {

template <class RandomAccessIter, class Distance, class T>
inline void
__push_heap(RandomAccessIter first, Distance holeIndex,
            Distance topIndex, T value)
{
    Distance parent = (holeIndex - 1) / 2; // index of parent node
    while(holeIndex > topIndex && *(first + parent) < value) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template<class RandomAccessIter, class Distance, class T>
inline void
__push_heap_aux(RandomAccessIter first, RandomAccessIter last,
                Distance *, T *)
{
    __push_heap(first, DISTANCE(last - first - 1),
                Distance(0), T(*(last - 1)));
}

template <class RandomAccessIter>
inline void
push_heap(RandomAccessIter first,
          RandomAccessIter last)
{
    __push_heap_aux(first, last, __DISTANCE_TYPE(first),
                    __VALUE_TYPE(first));
}

template <class RandomAccessIter, class Distance, class T>
inline void
__adjust_heap(RandomAccessIter first, Distance holeIndex,
              Distance len, T value)
{
    Distance topIndex = holeIndex;
    Distance rightIndex = 2 * holeIndex + 2; // second child of holeIndex
    while(rightIndex < len) {
        // whether the left child is bigger than right child
        if(*(first + rightIndex) < *(first + rightIndex - 1))
            --rightIndex;
        *(first + holeIndex) = *(first + rightIndex);
        holeIndex = rightIndex;
        rightIndex = 2 * rightIndex + 2;
    }
    if(rightIndex == len) { // only has left child
        *(first + holeIndex) = *(first + (rightIndex - 1));
        holeIndex = rightIndex - 1;
    }
    *(first + holeIndex) = value;
}

template <class RandomAccessIter, class Distance, class T>
inline void
__pop_heap(RandomAccessIter first,
           RandomAccessIter last,
           RandomAccessIter result,
           T value, Distance*)
{
    *result = *first;
    __adjust_heap(first, Distance(0),
            Distance(last - first), value);
}

template <class RandomAccessIter, class T>
inline void
__pop_heap_aux(RandomAccessIter first,
         RandomAccessIter last, T*)
{
    __pop_heap(first, last-1, last-1,
               T(*(last-1)), __DISTANCE_TYPE(first));
}

template <class RandomAccessIter>
inline void
pop_heap(RandomAccessIter first,
         RandomAccessIter last)
{
    __pop_heap_aux(first, last, __VALUE_TYPE(first));
}

template <class RandomAccessIter>
inline void
sort_heap(RandomAccessIter first,
          RandomAccessIter last)
{
    while(last - first > 1)
        pop_heap(first, last--);
}

template <class RandomAccessIter, class Distance, class T>
inline void
__make_heap(RandomAccessIter first,
            RandomAccessIter last,
            T*, Distance*)
{
    if(last - first < 2)
        return;
    Distance len = last - first;
    Distance parent = (len - 2) / 2;

    while(true) {
        __adjust_heap(first, parent, len, T(*(first + parent)));
        if(parent == 0)
            return;
        --parent;
    }
}

template <class RandomAccessIter>
inline void
make_heap(RandomAccessIter first,
          RandomAccessIter last)
{
    __make_heap(first, last, __VALUE_TYPE(first), __DISTANCE_TYPE(first));
}

} // sstl


#endif //SSTL_HEAP_H
