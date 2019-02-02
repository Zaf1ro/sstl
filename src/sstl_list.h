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
    _list_iterator(_list_node<_Tp>* _node): m_node(_node) {}
    _list_iterator(const iterator& it): m_node(it.m_node) {}

    bool operator==(const _Self& it) const { return m_node == it.m_node; }
    bool operator!=(const _Self& it) const { return m_node != it.m_node; }

    reference operator*() const { return m_node->m_data; }
    pointer operator->() const { return &(operator*()); }

    _Self& operator++() // prefix
    {
        m_node = m_node->m_next;
        return *this;
    }

    const _Self operator++(int) // postfix
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    _Self& operator--()
    {
        m_node = m_node->m_prev;
        return *this;
    }

    const _Self operator--(int)
    {
        _Self tmp = *this;
        --*this;
        return tmp;
    }

protected:
    _list_node<_Tp>* m_node;
};


template <class _Tp, class _Alloc>
class _list_base {
public:
    typedef _Tp             value_type;
    typedef _list_node<_Tp> _Node;

    typedef __SSTL_ALLOC(_Tp, _Alloc)               allocator_type;
    typedef __SSTL_ALLOC(_list_node<_Tp>, _Alloc)   node_allocator;

protected:
    /**
     * @brief   Request memory for a node
     */
    _Node* allocate_node() const
    {
        return node_allocator::allocate(1);
    }

public:
    allocator_type get_allocator() const
    { return allocator_type(); }

    explicit _list_base(const allocator_type&)
    {
        m_node = allocate_node();
        m_node->m_next = m_node;
        m_node->m_next = m_node;
    }

    ~_list_base()
    {
        auto cur = m_node->m_next;
        while( cur != m_node ) {
            _Node* tmp = cur;
            cur = cur->m_next;
            sstl::destroy(&(tmp->m_data));
            deallocate_node(tmp);
        }
        m_node->m_next = m_node;
        m_node->m_prev = m_node;
        deallocate_node(m_node);
    }

    /**
     * @brief   Release the memory of a node
     */
    void deallocate_node(_Node* __p) const
    { node_allocator::deallocate(__p); }

    /**
     * @brief   Request memory and initiate node with specified value
     */
    _Node* create_node(const value_type& __val) const
    {
        _Node* p = allocate_node();
        construct(p, __val);
        return p;
    }

protected:
    _Node* m_node;
};


template <class _Tp, class _Alloc = __SSTL_DEFAULT_ALLOC>
class list: protected _list_base<_Tp, _Alloc> {
public:
    typedef _Tp             value_type;
    typedef _Tp*            pointer;
    typedef const _Tp*      const_pointer;
    typedef _Tp&            reference;
    typedef const _Tp&      const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    typedef _list_base<_Tp, _Alloc>         _Base;
    typedef typename _Base::allocator_type  allocator_type;

    typedef _list_iterator<_Tp, _Tp&, _Tp*>             iterator;
    typedef _list_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;

protected:
    typedef _list_node<_Tp> _Node;
    using _Base::m_node;
    using _Base::allocate_node;
    using _Base::deallocate_node;

public:
    allocator_type get_allocator()
    { return _Base::get_allocator(); }

    /**
     * @brief   Construct the list
     */
    list(const allocator_type& __alloc = allocator_type())
     : _Base(__alloc) {}

    list(size_type __n, const value_type& __val,
         const allocator_type& __alloc = allocator_type())
     : _Base(__alloc)
    { insert(begin(), __n, __val); }

    explicit list(size_type __n)
     : _Base(allocator_type())
    { insert(begin(), __n, value_type()); }

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
     * @param   __pos: iterator before which
     *          the content will be inserted
     * @param   __val: element value to insert
     */
    iterator insert(iterator __pos, const value_type& __val)
    {
        _Node* p_tmp = create_node(__val);
        p_tmp->m_next = __pos.m_node;
        p_tmp->m_prev = __pos.m_node->m_prev;
        p_tmp->m_prev->m_next = p_tmp;
        p_tmp->m_next->m_prev = p_tmp;
        return p_tmp;
    }

    iterator insert(iterator __pos)
    { return insert(__pos, value_type()); }

    /**
     * @param   __n: number of elements to be inserted
     */
    void insert(iterator __pos, size_type __n,
                const value_type& __val)
    {
        for( ; __n > 0; --__n) {
            insert(__pos, __val);
        }
    }

    /**
     * @param   __first, __last: the range of elements to insert
     */
    template <class InputIter>
    void insert(iterator __pos, InputIter __first,
                InputIter __last)
    {
        for( ; __first != __last; ++__first) {
            insert(__pos, *__first);
        }
    }

    /**
     * @brief   Insert an element to the beginning of list
     */
    void push_front(const value_type& __val)
    { insert(begin(), __val); }

    /**
     * @brief   Add an element to the end of list
     */
    void push_back(const value_type& __val)
    { insert(end(), __val); }

    /**
     * @brief   Remove all elements satisfying specific criteria
     * @param   __val: value of element to remove
     */
    void remove(const value_type& __val)
    {
        iterator first = begin();
        iterator last = end();
        while( first != last ) {
            iterator next = first;
            ++next;
            if( *first == __val ) {
                erase(first);
            }
            first = next;
        }
    }

    /**
     * @brief   Erase the specified elements from list
     * @param   __pos: iterator to the element to remove
     */
    iterator erase(iterator __pos)
    {
        _Node* t_node = __pos.m_node;
        _Node* next_node = t_node->m_next;
        _Node* prev_node = t_node->m_prev;
        prev_node->m_next = next_node;
        next_node->m_prev = prev_node;
        sstl::destroy(t_node->m_data);
        deallocate_node(t_node);
        return next_node;
    }

    /**
     * @param   __first, __last: the range of elements to remove
     */
    iterator erase(iterator __first, iterator __last)
    {
        while( __first != __last ) {
            erase(__first);
            ++__first;
        }
        return __last;
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
    void assign(size_type __n, const value_type& __val)
    {
        iterator i = begin();
        for( ; i != end() && __n > 0; --__n, ++i) {
            *i = __val;
        }
        if( __n > 0 )
            insert(end(), __n, __val);
        else
            erase(i, end());
    }
};

} // sstl


#endif // SSTL_LIST_H
