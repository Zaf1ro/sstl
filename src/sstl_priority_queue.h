#ifndef SSTL_SSTL_PRIORITY_QUEUE_H
#define SSTL_SSTL_PRIORITY_QUEUE_H

#include "sstl_config.h"
#include "sstl_heap.h"
#include "sstl_vector.h"


namespace sstl {

template <class _Tp, class _Sequence = sstl::vector<_Tp>,
          class Compare = std::less<typename _Sequence::value_type>>
class priority_queue {
public:
    typedef typename _Sequence::value_type  value_type;
    typedef typename _Sequence::size_type   size_type;
    typedef typename _Sequence::reference   reference;

protected:
    _Sequence m_seq;
    Compare m_comp;

public:
    /**
     * @brief   Construct new underlying container
     */
    priority_queue(): m_seq() {}

    /**
     * @param   __comp: the comparison function object to
     *                  initialize the underlying comparison functor
     */
    explicit priority_queue(const Compare& __comp):
    m_seq(), m_comp(__comp) {}

    /**
     * @param   __first, __last: range of elements to initialize with
     */
    template <class InputIter>
    priority_queue(InputIter __first, InputIter __last,
                   const Compare& __comp)
     : m_seq(__first, __last), m_comp(__comp)
    {
        make_heap(m_seq.begin(), m_seq.end(), m_comp);
    }

    template <class InputIter>
    priority_queue(InputIter __first, InputIter __last)
     : m_seq(__first, __last)
    {
        make_heap(m_seq.begin(), m_seq.end(), m_comp);
    }

    /**
     * @brief   Check whether the underlying container is empty
     */
    bool empty() const { return m_seq.empty(); }

    /**
     * @brief   Return the number of elements
     */
    size_type size() const { return m_seq.size(); }

    /**
     * @brief   Return reference to the top element
     */
    reference top() { return m_seq.front(); }

    /**
     * @brief   Push the given element value
     * @param   __val: the value of element to push
     */
    void push(const value_type& __val)
    {
        __SSTL_TRY {
            m_seq.push_back(__val);
            push_heap(m_seq.begin(), m_seq.end(), m_comp);
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch (...) { }
#endif
    };

    /**
     * @brief   Remove the top element
     */
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
