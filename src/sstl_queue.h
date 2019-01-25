#ifndef SSTL_SSTL_QUEUE_H
#define SSTL_SSTL_QUEUE_H

#include "sstl_deque.h"


namespace sstl {

template <class T, class Sequence = deque<T>>
class queue {
    friend bool operator==(const queue& x, const queue& y)
    {
        return x.c == y.c;
    }

    friend bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y)
    {
        return x.c < y.c;
    }

public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

protected:
    Sequence* m_seq;

public:
    queue() { m_seq = new Sequence(); }
    explicit queue(size_type n) { m_seq = new Sequence(n); }
    queue(size_type n, const value_type& x) {
        m_seq = new Sequence(n, x);
    }
    ~queue() { delete m_seq; }

    bool empty() const { return m_seq->empty(); }
    size_type size() const { return m_seq->size(); }
    reference front() { return m_seq->front(); }
    const_reference front() const { return m_seq->front(); }
    reference back() { return m_seq->back(); }
    const_reference back() const { return m_seq->back(); }
    void push(const value_type& x) { m_seq->push_back(x); }
    void pop() { m_seq->pop_front(); }
};

} // sstl


#endif //SSTL_SSTL_QUEUE_H
