#ifndef SSTL_SET_H
#define SSTL_SET_H

#include "sstl_tree.h"
#include "sstl_alloc.h"
#include "sstl_functional.h"


namespace sstl {

template<class _Key,
        class _Compare = sstl::less<_Key>,
        class _Alloc = __SSTL_DEFAULT_ALLOC>
class set {
public:
    typedef _Key key_type;
    typedef _Key value_type;
    typedef _Compare key_compare;
    typedef _Compare value_compare;

private:
    typedef rb_tree<key_type, value_type, identity<value_type>,
                    key_compare, _Alloc> _Rep_type;
    _Rep_type m_t;

public:
    typedef typename _Rep_type::const_pointer pointer;
    typedef typename _Rep_type::const_pointer const_pointer;
    typedef typename _Rep_type::const_reference reference;
    typedef typename _Rep_type::const_reference const_reference;
    typedef typename _Rep_type::const_iterator iterator;
    typedef typename _Rep_type::const_iterator const_iterator;
    typedef typename _Rep_type::size_type size_type;
    typedef typename _Rep_type::difference_type difference_type;

    set(): m_t(_Compare()) {}
    explicit set(const _Compare& __comp): m_t(__comp) {}

    template <class InputIterator>
    set(InputIterator __first, InputIterator __last)
     : m_t(_Compare()) { m_t.insert_unique(__first, __last); }

    template <class InputIterator>
    set(InputIterator __first, InputIterator __last,
        const _Compare& __comp): m_t(__comp)
    { m_t.insert_unique(__first, __last); }

    set(const set<_Key, _Compare, _Alloc>& __x)
     : m_t(__x.m_t) {}

    /**
     * @brief   copy assignment operator. Replaces the contents
     *          with a copy of the contents of other
     */
    set<_Key, _Compare, _Alloc>&
    operator=(const set<_Key, _Compare, _Alloc>& __x)
    { m_t = __x.m_t; return *this; }

    /**
     * @brief   Returns the function object that compares the keys
     */
    key_compare key_comp() const { return m_t.key_comp(); }

    /**
     * @brief   Returns the function object that compares the values
     */
    value_compare value_comp() const { return m_t.key_comp(); }

    /**
     * @brief   Returns an iterator to the first element
     */
    iterator begin() const { return m_t.begin(); }

    /**
     * @brief   Returns an iterator to the element following
     *          the last element
     */
    iterator end() const { return m_t.end(); }

    /**
     * @brief   Check if the container has no elements
     */
    bool empty() const { return m_t.empty(); }

    /**
     * @brief   the number of elements
     */
    size_type size() const { return m_t.size(); }

    /**
     * @brief   Return the maximum number of elements
     *          the container is able to hold
     */
    size_type max_size() const { return m_t.max_size(); }

    /**
     * @brief   Exchange the contents of the container
     * @param   x: container to exchange the contents with
     */
    void swap(set<_Key, _Compare, _Alloc>& __x)
    { m_t.swap(__x.m_t); }

    /**
     * @brief   Insert element into the container
     * @param   val: element value to insert
     */
    pair<iterator, bool> insert(const value_type& __val)
    { return m_t.insert_unique(__val); }

    /**
     * @brief   Insert element(s) into the container
     * @param   first, last: range of elements to insert
     */
    template <class InputIterator>
    void insert(InputIterator __first, InputIterator __last)
    { m_t.insert_unique(__first, __last); }

    /**
     * @brief   Remove specified element from the container
     * @param   position: iterator to the element to remove
     */
    void erase(iterator __pos)
    {
        typedef typename _Rep_type::iterator rep_iterator;
        m_t.erase((rep_iterator)__pos);
    }

    /**
     * @param   x: key value of the element to remove
     */
    size_type erase(const key_type& __x) { return m_t.erase(__x); }

    /**
     * @param first, last: range of elements to remove
     */
    void erase(iterator __first, iterator __last)
    {
        typedef typename _Rep_type::iterator rep_iterator;
        m_t.erase((rep_iterator&)__first, (rep_iterator&)__last);
    }

    /**
     * @brief   Erase all elements from the container
     */
    void clear() { m_t.clear(); }

    /**
     * @brief   Find an element with key
     * @param   x: 	key value of the element to search for
     */
    iterator find(const key_type& __x) const
    { return m_t.find(__x); }

    /**
     * @brief   Return the number of elements with key
     * @param   x: key value of the elements to count
     */
    size_type count(const key_type& __x) const
    { return m_t.count(__x); }

    /**
     * @brief   Return an iterator pointing to the first element
     *          that is not less than key
     * @param   x: key value to compare the element
     */
    iterator lower_bound(const key_type& __x) const
    { return m_t.lower_bound(__x); }

    /**
     * @brief   Return an iterator pointing to the first element
     *          that is not bigger than key
     * @param   x: key value to compare the element
     */
    iterator upper_bound(const key_type& __x) const
    { return m_t.upper_bound(__x); }

    /**
     * @brief   Return a range containing all elements
     *          with the given key
     * @param   x: key value to compare the element
     */
    pair<iterator, iterator>
    equal_range(const key_type& __x) const
    { return m_t.equal_range(__x); }

    friend inline bool operator==(const set<_Key, _Compare, _Alloc>& __x,
                                  const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t == __y.m_t; }

    friend inline bool operator!=(const set<_Key, _Compare, _Alloc>& __x,
                                  const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t != __y.m_t; }

    friend inline bool operator<(const set<_Key, _Compare, _Alloc>& __x,
                                 const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t < __y.m_t; }

    friend inline bool operator>(const set<_Key, _Compare, _Alloc>& __x,
                                 const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t > __y.m_t; }

    friend inline bool operator<=(const set<_Key, _Compare, _Alloc>& __x,
                                  const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t <= __y.m_t; }

    friend inline bool operator>=(const set<_Key, _Compare, _Alloc>& __x,
                                  const set<_Key, _Compare, _Alloc>& __y)
    { return __x.m_t >= __y.m_t; }
};

} // sstl


#endif //SSTL_SET_H
