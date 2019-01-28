#ifndef SSTL_SSTL_PRIORITY_QUEUE_H
#define SSTL_SSTL_PRIORITY_QUEUE_H

#include "sstl_config.h"
#include "sstl_heap.h"
#include "sstl_vector.h"


namespace sstl {

template <class T, class Sequence = sstl::vector<T>,
          class Compare = std::less<typename Sequence::value_type>>
class priority_queue {
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;

protected:
    Sequence m_seq;
    Compare m_comp;

public:
    priority_queue(): m_seq() {}

    explicit priority_queue(const Compare& x):
    m_seq(), m_comp(x) {}

    template <class InputIter>
    priority_queue(InputIter first, InputIter last, const Compare& x)
     : m_seq(first, last), m_comp(x)
    {
        make_heap(m_seq.begin(), m_seq.end(), m_comp);
    }

    template <class InputIter>
    priority_queue(InputIter first, InputIter last)
     : m_seq(first, last)
    {
        make_heap(m_seq.begin(), m_seq.end(), m_comp);
    }

    bool empty() const { return m_seq.empty(); }
    size_type size() const { return m_seq.size(); }
    reference top() { return m_seq.front(); }
    void push(const value_type& x)
    {
        __SSTL_TRY {
            m_seq.push_back(x);
            push_heap(m_seq.begin(), m_seq.end(), m_comp);
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch (...) { }
#endif
    };

    void pop() {
        __SSTL_TRY {
            pop_heap(m_seq.begin(), m_seq.end(), m_comp);
            m_seq.pop_back();
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) { }
#endif
    };
};

} // sstl


#endif //SSTL_SSTL_PRIORITY_QUEUE_H
