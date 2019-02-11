#ifndef SSTL_HASHTABLE_H
#define SSTL_HASHTABLE_H

#include "sstl_functional.h"
#include "sstl_iterator.h"
#include "sstl_alloc.h"
#include "sstl_vector.h"
#include "sstl_config.h"
#include "sstl_pair.h"


namespace sstl {

template <class Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value data;
};

template <class Key, class Value,
        class HashFunc, // type of hash function
        class ExtractKey, // function of getting key from value
        class EqualKey, // function of checking if the arguments are equal
        class Alloc> //
class hashtable;

template <class Key, class Value, class HashFunc,
        class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

template <class Key, class Value, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {
    typedef hashtable<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

private:
    node* m_cur;
    hashtable* m_ht;

public:
    __hashtable_iterator(node* n, hashtable* tab)
     : m_cur(n), m_ht(tab) {}

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
            size_type bucket = m_ht->bkt_num(old->data);
            while(!m_cur && ++bucket < m_ht->m_buckets.size())
                m_cur = m_ht->m_buckets[bucket];
        }
        return *this;
    }

    const iterator operator++(int)
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

template <class Key, class Value, class HashFunc,
          class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator {
    typedef hashtable<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;

    typedef forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

private:
    node* m_cur;
    hashtable* m_ht;

public:
    __hashtable_const_iterator(const node* n, const hashtable* tab)
     : m_cur(n), m_ht(tab) {}

    __hashtable_const_iterator(const iterator& __it)
     : m_cur(__it.m_cur), m_ht(__it.m_ht){}

    __hashtable_const_iterator() = default;

    reference operator*() const { return m_cur->data; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++()
    {
        const node* old = m_cur;
        m_cur = m_cur->next;
        if(!m_cur) {
            size_type bucket = m_ht->bkt_num(old->data);
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

template <class Key, class Value,
          class HashFunc = std::hash<Key>, // type of hash function
          class ExtractKey = sstl::identity<Key>, // function of getting key from value
          class EqualKey = sstl::equal_to<Key>, // function of checking if the arguments are equal
          class Alloc = __SSTL_DEFAULT_ALLOC>
class hashtable {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Value* pointer;
    typedef const Value* const_pointer;
    typedef Value& reference;
    typedef const Value& const_reference;

    typedef __hashtable_iterator<Key, Value, HashFunc, ExtractKey, EqualKey, Alloc> iterator;

    typedef HashFunc hasher;
    typedef EqualKey key_equal;

private:
    hasher m_hash;
    key_equal m_equal;
    ExtractKey m_getKey;
    size_type m_num_element;

    typedef __hashtable_node<Value> node;
    typedef __SSTL_ALLOC(node, Alloc) node_allocator;

    sstl::vector<node*, Alloc> m_buckets;

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

    inline unsigned long __next_prime(unsigned long n)
    {
        const unsigned long* first = __prime_list;
        const unsigned long* last = __prime_list + __num_primes;
        // FIXME: use sstl::lower_bound
        const unsigned long* pos = std::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    size_type max_bucket_count() const
    { return __prime_list[__num_primes - 1]; }

    node* new_node(const value_type& obj)
    {
        node* n = node_allocator::allocate();
        n->next = 0;
        __SSTL_TRY {
            construct(&(n->data), obj);
            return n;
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            node_allocator::deallocate(n);
        }
#endif
        return n;
    }

    void delete_node(node* n)
    {
        destroy(&(n->val));
        node_allocator::deallocate(n);
    }

    size_type bkt_num(const value_type& obj, size_type n) const
    { return bkt_num_key(m_getKey(obj), n); }

    size_type bkt_num_key(const key_type& key) const
    { return bkt_num_key(key, m_buckets.size()); }

    size_type bkt_num_key(const key_type& key, size_type n) const
    { return hasher(key) % n; }

public:
    size_type bucket_count() const
    { return m_buckets.size(); }

    hashtable(size_type n,
              const HashFunc& hf,
              const EqualKey& eql)
     : m_hash(hf), m_equal(eql), m_getKey(ExtractKey()), m_num_element(0)
    {
        initialize_buckets(n);
    }

    void initialize_buckets(size_type n)
    {
        const size_type n_buckets = next_size(n);
        m_buckets.reserve(n_buckets);
        m_buckets.insert(m_buckets.end(), n_buckets, (node*)0);
        m_num_element = 0;
    }

    size_type next_size(size_type n) const
    { return __next_prime(n); }

    pair<iterator, bool> insert_unique(const value_type& obj)
    {
        resize(m_num_element + 1);
        return insert_unique_aux(obj);
    }

    void resize(size_type hint_n)
    {
        const size_type old_n = m_buckets.size();
        if(hint_n > old_n) {
            const size_type new_n = next_size(hint_n);
            if(new_n > old_n){
                vector<node*, Alloc> tmp(new_n, (node*)0);
                __SSTL_TRY {
                    for(size_type bucket = 0; bucket < old_n; ++bucket) {
                        node* first = m_buckets[bucket];
                        while(first) {
                            size_type new_bucket = bkt_num(first->val, new_n);
                            m_buckets[bucket] = first->next;
                            tmp[new_bucket] = first;
                            first = m_buckets[bucket];
                        }
                    }
                    m_buckets.swap(tmp);
                }
            #ifdef __SSTL_USE_EXCEPTIONS
                catch(...) {
                    clear();
                    throw;
                }
            #endif
            }
        }
    }

    pair<iterator, bool>
    insert_unique_aux(const value_type& obj)
    {
        const size_type& n = bkt_num(obj);
        node* first = m_buckets[n];
        for(node* cur = first; cur; cur->next)
            if(m_equal(m_getKey(cur->data), m_getKey(obj)))
                return pair<iterator, bool>(iterator(cur, this), false);
        node* tmp = new_node(obj);
        tmp->next = first;
        m_buckets[n] = tmp;
        ++m_num_element;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }

    iterator insert_equal(const value_type& obj)
    {
        resize(m_num_element + 1);
        return insert_equal_aux(obj);
    }

    iterator insert_equal_aux(const value_type& obj)
    {
        const size_type n = bkt_num(obj);
        node* first = m_buckets[n];

        for(node* cur = first; cur; cur = cur->next)
            if(m_euqal(m_getKey(cur->data), m_getKey(obj))) {
                node* tmp = new_node(obj);
                tmp->next = cur->next;
                cur->next = tmp;
                ++m_num_element;
                return iterator(tmp, this);
            }
        node* tmp = new_node(obj);
        tmp->next = first;
        m_buckets[n] = tmp;
        ++m_num_element;
        return iterator(tmp, this);
    }

    void clear()
    {
        for(size_type i = 0; i < m_buckets.size(); ++i) {
            node* cur = m_buckets[i];
            while(cur) {
                node* next = cur->next;
                delete_node(cur);
                cur = next;
            }
            m_buckets[i] = 0;
        }
        m_num_element = 0;
    }

    void copy_from(const hashtable& ht)
    {
        m_buckets.clear();
        m_buckets.reserve(ht.m_buckets.size());
        m_buckets.insert(m_buckets.end(), ht.m_buckets.size(), (node*)0);

        __SSTL_TRY {
            node* cur = NULL;
            for(size_type i = 0; i < ht.m_buckets.size(); ++i) {
                cur = ht.m_buckets[i];
                if(cur) {
                    node* copy = new_node(cur->data);
                    m_buckets[i] = copy;
                    for(node* next = cur->next; next; cur = next, next = cur->next) {
                        copy->next = new_node(next->data);
                        copy = copy->next;
                    }
                }
            }
            m_num_element = ht.m_num_element;
        }
    #ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            clear();
            throw;
        }
    #endif
    }

    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for(first = m_buckets[n];
            first && !equal(m_getKey(first->data), key);
            first = first->next) {}
        return iterator(first, this);
    }

    size_type count(const key_type& key) const
    {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for(const node* cur = m_buckets[n]; cur; cur = cur->next)
            if(m_equal(m_getKey(cur->data), key))
                ++result;
        return result;
    }
};

} // sstl


#endif // SSTL_HASHTABLE_H
