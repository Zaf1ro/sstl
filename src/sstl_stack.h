#ifndef SSTL_STACK_H
#define SSTL_STACK_H

#include "sstl_deque.h"


namespace sstl {

template <class _Tp, class _Sequence = deque<_Tp>>
class stack {
    friend bool operator==(const stack<_Tp, _Sequence>& __x,
                           const stack<_Tp, _Sequence>& __y)
    { return __x.m_seq == __y.m_seq; }

    friend bool operator!=(const stack<_Tp, _Sequence>& __x,
                           const stack<_Tp, _Sequence>& __y)
    { return !(__x == __y); }

    friend bool operator<(const stack<_Tp, _Sequence>& __x,
                          const stack<_Tp, _Sequence>& __y)
    { return __x.m_seq < __y.m_seq; }

    friend bool operator>(const stack<_Tp, _Sequence>& __x,
                          const stack<_Tp, _Sequence>& __y)
    { return __y < __x; }

    friend bool operator<=(const stack<_Tp, _Sequence>& __x,
                           const stack<_Tp, _Sequence>& __y)
    { return !(__y < __x); }

    friend bool operator>=(const stack<_Tp, _Sequence>& __x,
                           const stack<_Tp, _Sequence>& __y)
    { return !(__x < __y); }

public:
    typedef typename _Sequence::value_type       value_type;
    typedef typename _Sequence::size_type        size_type;
    typedef typename _Sequence::reference        reference;
    typedef typename _Sequence::const_reference  const_reference;

protected:
    _Sequence m_seq;

public:
    stack(): m_seq() {}

    /**
     * @brief   Return whether the stack is empty
     */
    bool empty() const { return m_seq.empty(); }

    /**
     * @brief   Return the number of elements
     */
    size_type size() const { return m_seq.size(); }

    /**
     * @brief   Return a reference to the top element
     */
    reference top() { return m_seq.back(); }
    const_reference top() const { return m_seq.back(); }

    /**
     * @brief   Insert a new element at the top of the stack
     * @param   __val: element value to push
     */
    void push(const value_type& __val) { m_seq.push_back(__val); }

    /**
     * @brief   Remove the element on top of the stack
     */
    void pop() { m_seq.pop_back(); }
};

} // sstl


#endif //SSTL_STACK_H
