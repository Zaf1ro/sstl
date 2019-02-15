#ifndef SSTL_MAP_H
#define SSTL_MAP_H

#include "sstl_pair.h"
#include "sstl_alloc.h"
#include "sstl_tree.h"
#include "sstl_functional.h"


namespace sstl {

template <class _Key, class _Value,
          class _Compare = sstl::less<_Key>,
          class _Alloc = __SSTL_DEFAULT_ALLOC>
class map {
public:
    typedef _Key    key_type;
    typedef _Value  mapped_type;
    typedef pair<const _Key, _Value> value_type;
    typedef _Compare key_compare;

    class value_compare:
            public binary_function<value_type, value_type, bool> {
    private:
        _Compare m_comp;

    public:
        value_compare(_Compare __comp): m_comp(__comp) {}

        bool operator()(const value_type& __x,
                        const value_type& __y) const
        { return m_comp(__x.first, __y.first); }

        friend class map<_Key, _Value, _Compare, _Alloc>;
    };

private:
    typedef __rb_tree<key_type, value_type,
                      sstl::select1st<value_type>,
                      key_compare , _Alloc> _Rep_type;
    _Rep_type m_rb;

public:
    typedef typename _Rep_type::pointer pointer;
    typedef typename _Rep_type::reference reference;
    typedef typename _Rep_type::difference_type difference_type;
    typedef typename _Rep_type::size_type size_type;
    typedef typename _Rep_type::allocator_type allocator_type;

    typedef typename _Rep_type::const_pointer const_pointer;
    typedef typename _Rep_type::const_reference const_reference;

    typedef typename _Rep_type::iterator iterator;
    typedef typename _Rep_type::const_iterator const_iterator;

    map(): m_rb(_Compare()) {}
    explicit map(const _Compare& __comp): m_rb(__comp) {}

    template <class InputIter>
    map(InputIter __first, InputIter __last): m_rb(_Compare())
    { m_rb.insert_unique(__first, __last); }

    template <class InputIter>
    map(InputIter __first, InputIter __last,
        _Compare __comp): m_rb(__comp)
    { m_rb.insert_unique(__first, __last); }

    map(const map<_Key, _Value, _Compare, _Alloc>& __x)
     : m_rb(__x.m_rb) {}

    map<_Key, _Value, _Compare, _Alloc>& 
    operator=(const map<_Key, _Value, _Compare, _Alloc>& __x)
    {
        m_rb = __x.m_rb;
        return *this;
    }

    /**
     * @brief   Return the allocator associated with map
     */
    allocator_type get_allocator() const
    { return m_rb.get_allocator(); }

    /**
     * @brief   Return the function that compare keys
     */
    key_compare key_comp() const { return m_rb.key_comp(); }

    /**
     * @brief   Return the function that compares keys
     *          in objects of type value_type
     */
    value_compare value_comp() const
    { return value_compare(m_rb.key_comp()); }

    /**
     * @brief   Return an iterator to the beginning
     */
    iterator begin() { return m_rb.begin(); }

    const_iterator begin() const { return m_rb.begin(); }

    /**
     * @brief   Return the iterator to the end
     */
    iterator end() { return m_rb.end(); }

    const_iterator end() const { return m_rb.end(); }

    /**
     * @brief   Check whether the map is empty
     */
    bool empty() const { return m_rb.empty(); }

    /**
     * @brief   Return the number of elements
     */
    size_type size() const { return m_rb.size(); }

    /**
     * @brief   Return the maximum possible number of elements
     */
    size_type max_size() const { return m_rb.max_size(); }

    /**
     * @brief   access or insert specified element
     */
    mapped_type& operator[](const key_type& __k)
    {
        return (insert(value_type(__k, mapped_type()))->first).second;
    }

    /**
     * @brief   Exchange contents of the container with other
     */
    void swap(map<_Key, _Value, _Compare, _Alloc>& __x)
    { m_rb.swap(__x.t); }

    /**
     * @brief   Insert element into the container if the container
     *          doesn't contain an element with an equivalent key
     * @param   __val: element value to insert
     */
    pair<iterator, bool> insert(const value_type& __val)
    { return m_rb.insert_unique(__val); }

    iterator insert(iterator __pos, const value_type& __val)
    { return m_rb.insert_unique(__pos, __val); }

    template <class InputIter>
    void insert(InputIter __first, InputIter __last)
    { m_rb.insert_unique(__first, __last); }

    /**
     * @brief   Remove specified element from the container
     * @param   __pos: iterator to the element to remove
     */
    void erase(iterator __pos) { m_rb.erase(__pos); }

    size_type erase(const key_type& __k)
    { return m_rb.erase(__k); }

    void erase(iterator __first, iterator __last)
    { m_rb.erase(__first, __last); }

    /**
     * @brief   Erase all elements from the container
     */
    void clear() { m_rb.clear(); }

    /**
     * @brief   Find element with key equivalent to key
     */
    iterator find(const key_type& __k)
    { return m_rb.find(__k); }

    const_iterator find(const key_type& __k) const
    { return m_rb.find(__k); }

    /**
     * @brief   Return the number of elements with key that
     *          equivalent to the specified argument
     */
    size_type count(const key_type& __k) const
    { return m_rb.count(__k); }

    /**
     * @brief   Return an iterator pointing to the first
     *          element that is not less than key
     * @param   __k: key value to compare the elements to
     */
    iterator lower_bound(const key_type& __k)
    { return m_rb.lower_bound(__k); }

    const_iterator lower_bound(const key_type& __k) const
    { return m_rb.lower_bound(__k); }

    /**
     * @brief   Return an iterator pointing to the first
     *          element that is greater than key
     * @param   __k: key value to compare the elements to
     */
    iterator upper_bound(const key_type& __k)
    { return m_rb.upper_bound(__k); }

    const_iterator upper_bound(const key_type& __k) const
    { return m_rb.upper_bound(__k); }

    /**
     * @brief   Return a range containing all elements
     *          with the given key in the container
     * @param   __k: keu value to compare the elements to
     */
    pair<iterator, iterator>
    equal_range(const key_type& __k)
    { return m_rb.equal_range(__k); }

    pair<const_iterator, const_iterator>
    equal_range(const key_type& __k) const
    { return m_rb.equal_range(__k); }

    friend inline bool
    operator==(const map<_Key, _Value, _Compare, _Alloc>& __x,
               const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return __x.m_rb == __y.m_rb; }

    friend inline bool
    operator<(const map<_Key, _Value, _Compare, _Alloc>& __x,
              const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return __x.m_rb < __y.m_rb; }

    friend inline bool
    operator!=(const map<_Key, _Value, _Compare, _Alloc>& __x,
               const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return !(__x == __y); }

    friend inline bool
    operator>(const map<_Key, _Value, _Compare, _Alloc>& __x,
              const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return __y < __x; }

    friend inline bool
    operator<=(const map<_Key, _Value, _Compare, _Alloc>& __x,
               const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return !(__y < __x); }

    friend inline bool
    operator>=(const map<_Key, _Value, _Compare, _Alloc>& __x,
               const map<_Key, _Value, _Compare, _Alloc>& __y)
    { return !(__x < __y); }
};

} // sstl


#endif // SSTL_MAP_H
