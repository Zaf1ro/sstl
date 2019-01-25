#ifndef SSTL_LIST_H
#define SSTL_LIST_H

#include "sstl_alloc.h"
#include "sstl_iterator.h"
#include "sstl_construct.h"


namespace sstl {

template <class _Tp>
struct _list_node {
    _list_node<_Tp>* m_prev;
    _list_node<_Tp>* m_next;
    _Tp m_data;
};

template <class _Tp, class _Ref, class _Ptr>
struct _list_iterator {
public:
    typedef _list_iterator<_Tp, _Tp&, _Tp*> iterator;
    typedef _list_iterator<_Tp, _Ref, _Ptr> _Self;

    typedef bidirectional_iterator_tag  iterator_category;
    typedef _Tp                         value_type;
    typedef _Ptr                        pointer;
    typedef _Ref                        reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;

    _list_iterator() = default;
    explicit _list_iterator(_list_node<_Tp>* _node): m_node(_node) {}
    explicit _list_iterator(const iterator& it): m_node(it.m_node) {}

    bool operator==(const _Self& it) const { return m_node == it.m_node; }
    bool operator!=(const _Self& it) const { return m_node != it.m_node; }

    reference operator*() const { return m_node->m_data; }
    pointer operator->() const { return &(operator*()); }

    _Self& operator++() { // prefix
        m_node = m_node->m_next;
        return *this;
    }

    const _Self operator++(int) { // postfix
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    _Self& operator--() {
        m_node = m_node->m_prev;
        return *this;
    }

    const _Self operator--(int) {
        _Self tmp = *this;
        --*this;
        return tmp;
    }

protected:
    _list_node<_Tp>* m_node;
};


template <class T, class Alloc>
class _list_base {
public:
    typedef T                                   value_type;
    typedef __SSTL_ALLOC(T, Alloc)              allocator_type;
    typedef __SSTL_ALLOC(_list_node<T>, Alloc)  node_allocator;

    explicit _list_base(const allocator_type&) {
        m_node = allocate_node();
        m_node->m_next = m_node;
        m_node->m_next = m_node;
    }

    ~_list_base() {
        auto cur = m_node->m_next;
        while( cur != m_node ) {
            _list_node<T>* tmp = cur;
            cur = cur->m_next;
            sstl::destroy(&(tmp->m_data));
            deallocate_node(tmp);
        }
        m_node->m_next = m_node;
        m_node->m_prev = m_node;
        deallocate_node(m_node);
    }

    allocator_type get_allocator() const {
        return allocator_type();
    }

    /**
     * @brief   Request memory for a node
     */
    _list_node<T>* allocate_node()
    {
        return node_allocator::allocate(1);
    }

    /**
     * @brief   Recycle memory for a node
     */
    void deallocate_node(_list_node<T>* p)
    {
        node_allocator::deallocate(p);
    }

    /**
     * @brief   Create a node and initiate with specified value
     */
    _list_node<T>* create_node(const value_type& value)
    {
        _list_node<T>* p = allocate_node();
        construct(p, value);
        return p;
    }

protected:
    _list_node<T>* m_node;
};


template <class T, class Alloc = __SSTL_DEFAULT_ALLOC>
class list: protected _list_base<T, Alloc> {
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef _list_node<T>   _Node;

    typedef _list_base<T, Alloc>                    _Base;
    typedef typename _Base::allocator_type          allocator_type;
    typedef _list_iterator<T, T&, T*>               iterator;
    typedef _list_iterator<T, const T&, const T*>   const_iterator;

protected:
    using _Base::m_node;
    using _Base::deallocate_node;

public:
    allocator_type get_allocator() const
    { return _Base::get_allocator(); }

    /**
     * @brief   Construct the list
     */
    explicit list(const allocator_type& a = allocator_type())
     : _Base(a) {}

    list(size_type n, const_reference val, const allocator_type& a = allocator_type())
    : _Base(a)
    { insert(begin(), n, val); }

    explicit list(size_type n): _Base(allocator_type())
    { insert(begin(), n, T()); }

    /**
     * @brief   Destruct the list
     */
    ~list() = default;

    /**
     * @brief   Return an iterator to the first element of list
     */
    iterator begin() { return m_node->m_next; }
    const_iterator begin() const { return m_node->m_next; }

    /**
     * @brief   Return an iterator to the element
     *          following the last element of list
     */
    iterator end() { return m_node; }
    const_iterator end() const { return m_node; }

    /**
     * @brief   Return the number of elements
     */
    size_type size() const { return (size_type)__DISTANCE(begin(), end()); }

    /**
     * @brief   Return the maximum possible number of elements
     */
    size_type max_size() const { return (size_type)-1; }

    /**
     * @brief   Check whether the list is empty
     */
    bool empty() const { return m_node->next == m_node; }

    /**
     * @brief   Return a reference to the first element
     */
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }

    /**
     * @brief   Return a reference to the last element
     */
    reference back() { return *(--end()); }
    const_reference back() const { return *end(); }

    /**
     * @brief   Insert element(s) at the specified location
     * @param   position: iterator before which
     *          the content will be inserted
     * @param   value: element value to insert
     */
    iterator insert(iterator position, const_reference value)
    {
        _Node* p_tmp = create_node(value);
        p_tmp->m_next = position.m_node;
        p_tmp->m_prev = position.m_node->m_prev;
        p_tmp->m_prev->m_next = p_tmp;
        p_tmp->m_next->m_prev = p_tmp;
        return p_tmp;
    }

    iterator insert(iterator position) {
        return insert(position, value_type());
    }

    /**
     * @param   n: number of elements to be inserted
     */
    void insert(iterator position, size_type n, const_reference value) {
        for( ; n > 0; --n) {
            insert(position, value);
        }
    }

    /**
     * @param   first, last: the range of elements to insert
     */
    template <class InputIter>
    void insert(iterator position, InputIter first, InputIter last) {
        for( ; first != last; ++first) {
            insert(position, *first);
        }
    }

    /**
     * @brief   Insert an element to the beginning of list
     */
    void push_front(const_reference value) { insert(begin(), value); }

    /**
     * @brief   Add an element to the end of list
     */
    void push_back(const_reference value) { insert(end(), value); }

    /**
     * @brief   Remove all elements satisfying specific criteria
     * @param   value: value of element to remove
     */
    void remove(const_reference value)
    {
        iterator first = begin();
        iterator last = end();
        while( first != last ) {
            iterator next = first;
            ++next;
            if( *first == value ) {
                erase(first);
            }
            first = next;
        }
    }

    /**
     * @brief   Erase the specified elements from list
     * @param   position: iterator to the element to remove
     */
    iterator erase(iterator position)
    {
        _Node* t_node = position.m_node;
        _Node* next_node = t_node->m_next;
        _Node* prev_node = t_node->m_prev;
        prev_node->m_next = next_node;
        next_node->m_prev = prev_node;
        destroy(t_node->m_data);
        deallocate_node(t_node);
        return next_node;
    }

    /**
     * @param   first, last: the range of elements to remove
     */
    iterator erase(iterator first, iterator last)
    {
        while( first != last ) {
            erase(first);
            ++first;
        }
        return last;
    }

    /**
     * @brief   Remove the first element
     */
    void pop_front() { erase(begin()); }

    /**
     * @brief   Remove the last element
     */
    void pop_back() { erase(--end()); }

    /**
     * @brief   Replace the elements' value with certain number of value
     * @param   n: the new size of container
     * @param   val: the value to initialize elements
     */
    void assign(size_type n, const_reference val)
    {
        iterator i = begin();
        for( ; i != end() && n > 0; --n, ++i) {
            *i = val;
        }
        if( n > 0 )
            insert(end(), n, val);
        else
            erase(i, end());
    }
};

} // sstl


#endif // SSTL_LIST_H
