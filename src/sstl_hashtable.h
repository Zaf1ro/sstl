#ifndef SSTL_HASHTABLE_H
#define SSTL_HASHTABLE_H

#include "sstl_functional.h"
#include "sstl_iterator.h"
#include "sstl_alloc.h"
#include "sstl_vector.h"
#include "sstl_config.h"
#include "sstl_pair.h"


namespace sstl {

template <class _Value>
struct __hashtable_node {
    __hashtable_node* next;
    _Value data;
};

template <class _Key, class _Value,
          class _HashFunc, // type of hash function
          class _ExtractKey, // function of getting key from value
          class _EqualKey, // function of checking if the arguments are equal
          class _Alloc> //
class hashtable;

template <class _Key, class _Value,
          class _HashFunc,
          class _ExtractKey,
          class _EqualKey,
          class _Alloc>
struct __hashtable_const_iterator;

template <class _Key, class _Value,
          class _HashFunc,
          class _ExtractKey,
          class _EqualKey,
          class _Alloc>
class __hashtable_iterator {
public:
    typedef hashtable<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> hashtable;
    typedef __hashtable_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> iterator;
    typedef __hashtable_const_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> const_iterator;

    typedef _Value value_type;
    typedef _Value& reference;
    typedef _Value* pointer;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef forward_iterator_tag iterator_category;

private:
    typedef __hashtable_node<_Value>* _Link_type;

private:
    _Link_type m_cur;
    hashtable* m_ht;

public:
    __hashtable_iterator(_Link_type __cur, hashtable* __ht)
     : m_cur(__cur), m_ht(__ht) {}

    __hashtable_iterator() = default;

    __hashtable_iterator(const iterator& __it)
     : m_cur(__it.m_cur), m_ht(__it.m_ht) {}

    reference operator*() const { return m_cur->data; }

    pointer operator->() const { return &(operator*()); }

    iterator& operator++()
    {
        const iterator old = m_cur;
        m_cur = m_cur->next;
        if(!m_cur) {
            size_t bucket = m_ht->bkt_num(old->data);
            while(!m_cur && ++bucket < m_ht->m_buckets.size())
                m_cur = m_ht->m_buckets[bucket];
        }
        return *this;
    }

    const const_iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const iterator& it) const
    { return m_cur == it.m_cur; }

    bool operator!=(const iterator& it) const
    { return m_cur != it.m_cur; }
};

template <class _Key, class _Value,
          class _HashFunc,
          class _ExtractKey,
          class _EqualKey,
          class _Alloc>
class __hashtable_const_iterator {
public:
    typedef hashtable<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> hashtable;
    typedef __hashtable_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> iterator;
    typedef __hashtable_const_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> const_iterator;

    typedef _Value value_type;
    typedef ptrdiff_t difference_type;
    typedef _Value& reference;
    typedef _Value* pointer;

    typedef forward_iterator_tag iterator_category;

private:
    typedef __hashtable_node<_Value>* _Link_type;

private:
    _Link_type m_cur;
    hashtable* m_ht;

public:
    __hashtable_const_iterator(): m_cur(0), m_ht(0) {}

    __hashtable_const_iterator(_Link_type __cur, hashtable* __ht)
     : m_cur(__cur), m_ht(__ht) {}

    __hashtable_const_iterator(const iterator& __it)
     : m_cur(__it.m_cur), m_ht(__it.m_ht) {}

    reference operator*() const { return m_cur->data; }

    pointer operator->() const { return &(operator*()); }

    const_iterator& operator++()
    {
        _Link_type old = m_cur;
        m_cur = m_cur->next;
        if(!m_cur) {
            size_t bucket = m_ht->bkt_num(old->data);
            while(!m_cur && ++bucket < m_ht->m_buckets.size())
                m_cur = m_ht->m_buckets[bucket];
        }
        return *this;
    }

    const const_iterator operator++(int)
    {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const const_iterator& __it)
    { return m_cur == __it.m_cur; }

    bool operator!=(const const_iterator& __it)
    { return m_cur != __it.m_cur; }
};

template <class _Key, class _Value,
          class _HashFunc = std::hash<_Key>, // type of hash function
          class _ExtractKey = sstl::identity<_Key>, // function of getting key from value
          class _EqualKey = sstl::equal_to<_Key>, // function of checking if the arguments are equal
          class _Alloc = __SSTL_DEFAULT_ALLOC>
class hashtable {
public:
    typedef _Key key_type;
    typedef _Value value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef _Value* pointer;

    typedef const _Value* const_pointer;
    typedef _Value& reference;
    typedef const _Value& const_reference;

    typedef __hashtable_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> iterator;
    typedef __hashtable_const_iterator<_Key, _Value, _HashFunc, _ExtractKey, _EqualKey, _Alloc> const_iterator;

    typedef _HashFunc hasher;
    typedef _EqualKey key_equal;

private:
    typedef __hashtable_node<_Value> _Node;
    typedef __hashtable_node<_Value>* _Link_type;
    typedef __SSTL_ALLOC(_Node, _Alloc) node_allocator;

private:
    hasher m_hash;
    key_equal m_equal;
    _ExtractKey m_getKey;
    size_type m_num_element;

    sstl::vector<_Link_type, _Alloc> m_buckets;

    enum { __num_primes = 28 };

    static const unsigned long __prime_list[__num_primes] =
    {
            53ul,         97ul,         193ul,       389ul,       769ul,
            1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
            49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
            1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
            50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
            1610612741ul, 3221225473ul, 4294967291ul
    };

    inline unsigned long __next_prime(unsigned long __n)
    {
        const unsigned long* first = __prime_list;
        const unsigned long* last = __prime_list + __num_primes;
        // FIXME: use sstl::lower_bound
        const unsigned long* pos = std::lower_bound(first, last, __n);
        return pos == last ? *(last - 1) : *pos;
    }

private:
    _Link_type _allocate_node()
    { return node_allocator::allocate(1); }

    void _deallocate_node(_Link_type __p)
    { node_allocator::deallocate(__p, 1); }

    _Link_type _new_node(const value_type& __val)
    {
        _Link_type p = _allocate_node();
        p->next = 0;
        __SSTL_TRY {
            construct(&(p->data), __val);
            return p;
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            _deallocate_node(p);
        }
#endif
        return p;
    }

    void _delete_node(_Link_type __p)
    {
        sstl::destroy(&(__p->val));
        _deallocate_node(__p);
    }

    size_type _bkt_num(const value_type& __val,
                       const size_type __n) const
    { return _bkt_num_key(m_getKey(__val), __n); }

    size_type _bkt_num_key(const key_type& __k) const
    { return _bkt_num_key(__k, m_buckets.size()); }

    size_type _bkt_num_key(const key_type& __k,
                           const size_type __n) const
    { return hasher(__k) % __n; }

public:
    hashtable(size_type __n,
              const _HashFunc& __hf,
              const _EqualKey& __eql)
     : m_hash(__hf), m_equal(__eql), m_getKey(_ExtractKey()), m_num_element(0)
    { _initialize_buckets(__n); }

    hashtable(const hashtable& __ht)
     : m_hash(__ht.m_hash), m_equal(__ht.m_equal),
       m_getKey(__ht.m_getKey), m_buckets(__ht.m_buckets),
       m_num_element(0)
    { _copy_from(__ht); }

    ~hashtable() { clear(); }

    /**
     * @brief   Insert element if the container doesn't contain
     *          an element with an equivalent key
     * @param   __val: element value to insert
     */
    pair<iterator, bool>
    insert_unique(const value_type& __val)
    {
        _resize(m_num_element + 1);
        return insert_unique_noresize(__val);
    }

    /**
     * @brief   Insert element if the container doesn't contain
     *          an element with an equivalent key(not resize container)
     */
    pair<iterator, bool>
    insert_unique_noresize(const value_type& __val)
    {
        const size_type& idx = _bkt_num(__val);
        _Link_type first = m_buckets[idx];
        for(_Link_type cur = first; cur; cur->next)
            // the node already exists in bucket
            if(m_equal(m_getKey(cur->data), m_getKey(__val)))
                return pair<iterator, bool>(iterator(cur, this), false);

        // create new node and connect it with bucket
        _Link_type tmp = _new_node(__val);
        tmp->next = first;
        m_buckets[idx] = tmp;
        ++m_num_element;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }

    /**
     * @brief   Insert element no matter whether the container
     *          contains an element with an equivalent key
     * @brief   __val: element value to insert
     */
    iterator insert_equal(const value_type& __val)
    {
        _resize(m_num_element + 1);
        return insert_equal_noresize(__val);
    }

    /**
     * @brief   Insert element no matter whether the container
     *          contains an element with an equivalent key
     *          (not resize container)
     */
    iterator insert_equal_noresize(const value_type& __val)
    {
        const size_type idx = _bkt_num(__val);
        _Link_type first = m_buckets[idx];

        for(_Link_type cur = first; cur; cur = cur->next)
            // the node already exists in bucket
            if(m_euqal(m_getKey(cur->data), m_getKey(__val))) {
                _Link_type tmp = _new_node(__val);
                tmp->next = cur->next;
                cur->next = tmp;
                ++m_num_element;
                return iterator(tmp, this);
            }
        // the node doesn't exist in bucket
        _Link_type tmp = _new_node(__val);
        tmp->next = first;
        m_buckets[idx] = tmp;
        ++m_num_element;
        return iterator(tmp, this);
    }

    /**
     * @brief   Clean all elements from container
     */
    void clear()
    {
        for(size_type idx = 0; idx < m_buckets.size(); ++idx) {
            _Link_type cur = m_buckets[idx];
            while(cur) {
                _Link_type next = cur->next;
                _delete_node(cur);
                cur = next;
            }
            m_buckets[idx] = nullptr;
        }
        m_num_element = 0;
    }

    /**
     * @brief   Find an element with key equivalent to key
     * @param   __k: key value of the element to search for
     */
    iterator find(const key_type& __k) {
        size_type idx = _bkt_num_key(__k);
        _Link_type first;
        // try to find the node from bucket
        for(first = m_buckets[idx];
            first && !equal(m_getKey(first->data), __k);
            first = first->next) {}
        return iterator(first, this);
    }

    /**
     * @brief   Return the number of elements with key that
     *          equivalent to the argument
     * @param   key value of the elements to count
     */
    size_type count(const key_type& __k) const
    {
        const size_type idx = _bkt_num_key(__k);
        size_type count = 0;
        for(_Link_type cur = m_buckets[idx]; cur; cur = cur->next)
            if(m_equal(m_getKey(cur->data), __k))
                ++count;
        return count;
    }

    /**
     * @brief   Return the number of buckets
     */
    size_type bucket_count() const
    { return m_buckets.size(); }

    /**
     * @brief   Return the maximum number of buckets
     */
    size_type max_bucket_count() const
    { return __prime_list[__num_primes - 1]; }

    /**
     * @brief   Return the number of elements in specific bucket
     */
    size_type elems_in_bucket(size_type __idx) const
    {
        size_type count = 0;
        for(_Link_type cur = m_buckets[__idx]; cur; cur = cur->next)
            ++count;
        return count;
    }

private:
    /**
     * @brief   Initialize buckets in the container
     * @param   __n: minimal number of buckets to initialize
     */
    void _initialize_buckets(size_type __n)
    {
        const size_type new_n = __next_prime(__n);
        m_buckets.reserve(new_n);
        m_buckets.insert(m_buckets.end(), new_n, (_Link_type)0);
        m_num_element = 0;
    }

    /**
     * @brief   Copy hastable from another hashtable
     * @param   __ht: hashtable object to copy with
     */
    void _copy_from(const hashtable& __ht)
    {
        m_buckets.clear();
        m_buckets.reserve(__ht.m_buckets.size());
        m_buckets.insert(m_buckets.end(), __ht.m_buckets.size(), (_Link_type)0);

        __SSTL_TRY {
            _Link_type cur = NULL;
            // copy all nodes from all bucket
            for(size_type idx = 0; idx < __ht.m_buckets.size(); ++idx) {
                cur = __ht.m_buckets[idx];
                if(cur) {
                    // copy all nodes from certain bucket
                    _Link_type copy = _new_node(cur->data);
                    m_buckets[idx] = copy;
                    for(_Link_type next = cur->next; next; cur = next, next = cur->next) {
                        copy->next = _new_node(next->data);
                        copy = copy->next;
                    }
                }
            }
            m_num_element = __ht.m_num_element;
        }
    #ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            clear();
            throw;
        }
    #endif
    }

    /**
     * @brief   Resize the container to certain size
     * @param   __hint_n: new size of container
     */
    void _resize(size_type __hint_n)
    {
        const size_type old_n = m_buckets.size();
        if(__hint_n > old_n) {
            // find the next larger prime number
            const size_type new_n = __next_prime(__hint_n);
            if(new_n > old_n){
                // create an new vector and move all elements to new vector
                vector<_Link_type, _Alloc> tmp(new_n, (_Link_type)0);
                __SSTL_TRY {
                    for(size_type idx = 0; idx < old_n; ++idx) {
                        _Link_type first = m_buckets[idx];
                        while(first) { // rehash all nodes in bucket
                            size_type new_bucket = _bkt_num(first->val, new_n);
                            m_buckets[idx] = first->next; // disconnect first from old bucket
                            first->next = tmp[new_bucket];
                            tmp[new_bucket] = first; // connect node with new bucket
                            first = m_buckets[idx];
                        }
                    }
                    m_buckets.swap(tmp);
                }
#ifdef __SSTL_USE_EXCEPTIONS
                catch(...) {
                    // delete all nodes in new vector
                    for(size_type idx = 0; idx < tmp.size(); ++idx) {
                        while(tmp[idx]) {
                            _Link_type next = tmp[idx]->next;
                            _delete_node(tmp[idx]);
                            tmp[idx] = next;
                        }
                    }
                    throw;
                }
#endif
            }
        }
    }
};

} // sstl


#endif // SSTL_HASHTABLE_H
