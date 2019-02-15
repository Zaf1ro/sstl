#ifndef SSTL_HASH_SET_H
#define SSTL_HASH_SET_H

#include "sstl_hashtable.h"
#include "sstl_pair.h"


namespace sstl {

template <class _Key,
          class _Value,
          class _HashFunc = std::hash<_Key>,
          class _EqualKey = sstl::equal_to<_Key>,
          class _Alloc = __SSTL_DEFAULT_ALLOC>
class hash_set {
private:
    typedef hashtable<_Key, _Value, _HashFunc,
                      identity<_Value>, _EqualKey,
                      _Alloc> _Ht;
    _Ht m_ht;

public:
    typedef typename _Ht::key_type    key_type;
    typedef typename _Ht::value_type  value_type;
    typedef typename _Ht::hasher      hasher;
    typedef typename _Ht::key_equal   key_equal;

    typedef typename _Ht::size_type       size_type;
    typedef typename _Ht::difference_type difference_type;
    typedef typename _Ht::const_pointer   pointer;
    typedef typename _Ht::const_reference reference;

    typedef typename _Ht::const_pointer   const_pointer;
    typedef typename _Ht::const_reference const_reference;

    typedef typename _Ht::const_iterator  iterator;
    typedef typename _Ht::const_iterator  const_iterator;

    typedef typename _Ht::allocator_type  allocator_type;

    /**
     * @brief   Return the function that hashes the keys
     */
    hasher hash_func() const { return m_ht.hash_func(); }

    /**
     * @brief   Return the function that compares keys
     *          for equality
     */
    key_equal key_eq() const { return m_ht.key_eq(); }

    /**
     * @brief   Return the allocator associated with the container
     */
    allocator_type get_allocator() const
    { return m_ht.get_allocator(); }
    
public:
    hash_set(): m_ht(100, hasher(), key_equal()) {}

    explicit hash_set(size_type __n)
     : m_ht(__n, hasher(), key_equal()) {}

    hash_set(const size_type __n, const hasher& __hf)
     : m_ht(__n, __hf, key_equal()) {}

    hash_set(const size_type __n, const hasher& __hf,
             const key_equal& __eql)
     : m_ht(__n, __hf, __eql) {}

    /**
     * @brief   Return the number of elements
     */
    size_type size() const { return m_ht.size(); }

    /**
     * @brief   Return the maximum possible number of elements
     */
    size_type max_size() const { return m_ht.max_size(); }

    /**
     * @brief   Check whether the set is empty
     */
    bool empty() const { return m_ht.empty(); }

    /**
     * @brief   Exchange the content of container with other
     */
    void swap(hash_set& __hs) { m_ht.swap(__hs.m_ht); }

    friend inline bool operator==(const hash_set& __hs1,
                                  const hash_set& __hs2)
    { return __hs1.m_ht == __hs2.m_ht; }

    /**
     * @brief   Return an iterator to the beginning
     */
    iterator begin() const { return m_ht.begin(); }

    /**
     * @brief   Return an iterator to the end
     */
    iterator end() const { return m_ht.begin(); }

    /**
     * @brief   Insert element into the container
     * @param   __val: element value to insert
     */
    pair<iterator, bool>
    insert(const value_type& __val)
    {
        auto p = m_ht.insert_unique(__val);
        return pair<iterator, bool>(p.first, p.second);
    }

    /**
     * @brief   Insert element(s) into the container
     * @param   __first, __last: range of elements to insert
     */
    template <class InputIter>
    void insert(InputIter __first, InputIter __last)
    { m_ht.insert_unique(__first, __last); }

    /**
     * @brief   Find an element with key equivalent to key
     * @param   __k: key value of the element to search for
     */
    iterator find(const key_type& __k) const
    { return m_ht.find(__k); }

    /**
     * @brief   Return the number of elements with key
     * @param   __k: key value of the elements to count
     */
    size_type count(const key_type& __k) const
    { return m_ht.count(__k); }

    /**
     * @brief   Return a range containing all elements with
     *          the given key in the container
     * @param   __k: key value to compare the elements
     */
    pair<iterator, iterator>
    equal_range(const key_type& __k) const
    { return m_ht.equal_range(__k); }

    /**
     * @brief   Remove the element with key equivalent to __k
     * @param   __k: key value of the elements to remove
     */
    size_type erase(const key_type& __k)
    { return m_ht.erase(__k); }

    /**
     * @brief   Remove the element at certain position
     * @param   __it: iterator to the element to remove
     */
    void erase(iterator __it) { m_ht.erase(__it); }

    /**
     * @brief   Remove the element in the range[first, last)
     * @param   __first, __last: range of elements to remove
     */
    void erase(iterator __first, iterator __last)
    { m_ht.erase(__first, __last); }

    /**
     * @brief   Erase all elements from the container
     */
    void clear() { m_ht.clear(); }

    /**
     * @brief   Resize the container to certain size
     * @param   __hint_n: new size of the container
     */
    void resize(size_type __hint_n)
    { m_ht.resize(__hint_n); }

    /**
     * @brief   Return the number of buckets
     */
    size_type bucket_count() const
    { return m_ht.bucket_count(); }

    /**
     * @brief   Return the maximum number of buckets
     */
    size_type max_bucket_count() const
    { return m_ht.max_bucket_count(); }

    /**
     * @brief   Return the number of elements in the
     *          bucket with index __n
     * @param   __n: the index of bucket to examine
     */
    size_type elems_in_bucket(size_type __n) const
    { return m_ht.elems_in_bucket(__n); }
};

} // sstl


#endif // SSTL_HASH_SET_H
