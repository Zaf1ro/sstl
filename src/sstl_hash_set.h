#ifndef SSTL_HASH_SET_H
#define SSTL_HASH_SET_H

#include "sstl_hashtable.h"
#include "sstl_pair.h"


namespace sstl {

template <class Key,
          class Value,
          class HashFunc = std::hash<Key>,
          class EqualKey = sstl::equal_to<Key>,
          class Alloc = __SSTL_DEFAULT_ALLOC>
class hash_set {
private:
    typedef hashtable<Key, Value, HashFunc, identity<Value>,
                      EqualKey, Alloc> ht;
    ht rep;

public:
    typedef typename ht::key_type   key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher     hasher;
    typedef typename ht::key_equal  key_equal;

    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::const_iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_func() const { return rep.hash_func(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    hash_set(): rep(100, hasher(), key_equal()) {}

    explicit hash_set(size_type n)
     : rep(n, hasher(), key_equal()) {}

    hash_set(size_type n, const hasher& hf)
     : rep(n, hf, key_equal()) {}

    hash_set(size_type n, const hasher& hf,
             const key_equal& eql)
     : rep(n, hf, eql) {}

public:
    size_type size() const { return rep.size(); }

    size_type max_size() const { return rep.max_size(); }

    bool empty() const { return rep.empty(); }

    void swap(hash_set& hs) { rep.swap(hs.rep); }

    friend bool operator==(const hash_set& a, const hash_set& b)
    { return a.rep == b.rep; }

    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.begin(); }

public:
    pair<iterator, bool> insert(const value_type& obj)
    {
        auto p = rep.insert_unique(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    template <class InputIter>
    void insert(InputIter first, InputIter last)
    { rep.insert_unique(first, last); }

    iterator find(const key_type& key) const
    { return rep.find(key); }

    size_type count(const key_type& key) const
    { return rep.count(key); }

    pair<iterator, iterator>
    equal_range(const key_type& key) const
    { return rep.equal_range(key); }

    size_type erase(const key_type& key)
    { return rep.erase(key); }

    void erase(iterator it) { rep.erase(it); }
    void erase(iterator first, iterator last)
    { rep.erase(first, last); }
    void clear() { rep.clear(); }

public:
    void resize(size_type hint) { rep.resize(hint); }

    size_type bucket_count() const
    { return rep.bucket_count(); }

    size_type max_bucket_count() const
    { return rep.max_bucket_count(); }

    size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};

} // sstl


#endif // SSTL_HASH_SET_H
