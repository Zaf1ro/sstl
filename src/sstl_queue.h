#ifndef SSTL_QUEUE_H
#define SSTL_QUEUE_H

#include "sstl_deque.h"


namespace sstl {

template <class _Tp, class _Sequence = deque<_Tp>>
class queue {
    friend bool operator==(const queue& __x,
                           const queue& __y)
    { return __x.m_seq == __y.m_seq; }

    friend bool operator<(const queue<_Tp, _Sequence>& __x,
                          const queue<_Tp, _Sequence>& __y)
    { return __x.m_seq < __y.m_seq; }

    friend bool operator>(const queue<_Tp, _Sequence>& __x,
                          const queue<_Tp, _Sequence>& __y)
    { return __y < __x; }

    friend bool operator<=(const queue<_Tp, _Sequence>& __x,
                           const queue<_Tp, _Sequence>& __y)
    { return !(__x > __y); }

    friend bool operator>=(const queue<_Tp, _Sequence>& __x,
                           const queue<_Tp, _Sequence>& __y)
    { return !(__x < __y); }

public:
    typedef typename _Sequence::value_type       value_type;
    typedef typename _Sequence::size_type        size_type;
    typedef typename _Sequence::reference        reference;
    typedef typename _Sequence::const_reference  const_reference;

protected:
    _Sequence m_seq;

public:
    queue(): m_seq() {}

    /**
     * @brief   Return whether the queue is empty
     */
    bool empty() const { return m_seq.empty(); }

    /**
     * @brief   Return the number of elements in the queue
     */
    size_type size() const { return m_seq.size(); }

    /**
     * @brief   Return a reference to the next element
     */
    reference front() { return m_seq.front(); }
    const_reference front() const { return m_seq.front(); }

    /**
     * @brief   Return a reference to the last element
     */
    reference back() { return m_seq.back(); }
    const_reference back() const { return m_seq.back(); }

    /**
     * @brief   Insert a new element at the end of the queue
     */
    void push(const value_type& __val) { m_seq.push_back(__val); }

    /**
     * @brief   Remove the next element in the queue
     */
    void pop() { m_seq.pop_front(); }
};

} // sstl


#endif //SSTL_QUEUE_H
