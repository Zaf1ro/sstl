#ifndef SSTL_DEQUE_H
#define SSTL_DEQUE_H

#include "sstl_alloc.h"
#include "sstl_iterator.h"
#include "sstl_config.h"
#include "sstl_uninitialized.h"
#include "sstl_construct.h"


namespace sstl {

inline size_t __deque_buf_size(size_t n, size_t sz)
{
    return n != 0 ? n : (sz < 512 ? (size_t)(512 / sz) : (size_t)1);
}

template <class _Tp, class _Ref, class _Ptr, size_t BufSize>
struct _deque_iterator {
    typedef random_access_iterator_tag  iterator_category;
    typedef _Tp                         value_type;
    typedef _Ptr                        pointer;
    typedef _Ref                        reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    typedef _deque_iterator            _Self;

    _deque_iterator()
     : m_node(0), m_cur(0), m_first(0), m_last(0) {}

    reference operator*() const
    { return *m_cur; }

    pointer operator->() const
    { return &(this->operator*()); }

    difference_type operator-(const _Self& x) const
    {
        return buffer_size() * (m_node - x.m_node - 1)
               + (m_cur - m_first) + (x.m_last - x.m_cur);
    }

    _Self& operator++()
    {
        ++m_cur;
        if( m_cur == m_last ) {
            set_node(m_node + 1);
            m_cur = m_first;
        }
        return *this;
    }

    const _Self operator++(int)
    {
        _Self tmp = *this;
        ++*this;
        return tmp;
    }

    _Self& operator--()
    {
        if( m_cur == m_last ) {
            set_node(m_node - 1);
            m_cur = m_last;
        }
        --m_cur;
        return *this;
    }

    const _Self operator--(int)
    {
        _Self tmp = *this;
        --*this;
        return tmp;
    }

    _Self& operator+=(difference_type __n)
    {
        difference_type offset = __n + (m_cur - m_first);
        if (offset >= 0 && offset < buffer_size()) {
            m_cur += __n;
        } else {
            difference_type node_offset = offset > 0 ?
                    offset / buffer_size() :
                    -(-offset - 1) / buffer_size() - 1;
            set_node(m_node + node_offset);
            m_cur = m_first + (offset - node_offset * buffer_size());
        }
        return *this;
    }

    _Self operator+(difference_type n) const
    { return *this += n; }

    _Self& operator-=(difference_type n)
    { return *this += -n; }

    _Self operator-(difference_type n) const
    { return *this -= n; }

    reference operator[](difference_type n)
    { return *(*this + n); }

    bool operator==(const _Self& that) const
    { return this->m_cur == that.m_cur; }

    bool operator!=(const _Self& that) const
    { return !(*this == that); }

    bool operator<(const _Self& that)
    {
        return m_node == that.m_node ?
                m_cur < that.m_cur :
                m_node < that.m_node;
    }

    inline difference_type buffer_size() const
    {
        return __deque_buf_size(BufSize, sizeof(value_type));
    }

    void set_node(value_type** __n_node)
    {
        m_node = __n_node;
        m_first = *__n_node;
        m_last = m_first + buffer_size();
    }

public:
    _Tp** m_node;
    _Tp* m_cur;
    _Tp* m_first;
    _Tp* m_last;
};


template <class _Tp, class _Alloc, size_t BufSize>
class _deque_base
{
public:
    typedef _deque_iterator<_Tp, _Tp&, _Tp*, BufSize>               iterator;
    typedef _deque_iterator<_Tp, const _Tp&, const _Tp*, BufSize>   const_iterator;

    typedef _Alloc                      allocator_type;
    typedef __SSTL_ALLOC(_Tp, _Alloc)   _Node_alloc_type;
    typedef __SSTL_ALLOC(_Tp*, _Alloc)  _Map_alloc_type;

public:
    allocator_type get_allocator() const
    { return allocator_type(); }

    _deque_base(const allocator_type&)
     : m_map(0), m_map_size(0)
    {
        _initialize_map(0);
        m_finish.m_node = m_start.m_node;
    }

    _deque_base(const allocator_type&, size_t __n)
     : m_map(0), m_map_size(0)
    { _initialize_map(__n); }

    ~_deque_base()
    {
        if(m_map) {
            _destroy_nodes(m_start.m_node, m_finish.m_node + 1);
            _deallocate_map(m_map, m_map_size);
        }
    }

protected:
    inline size_t _initial_map_size() { return 8; }

    inline size_t _buffer_size() {
        return __deque_buf_size(BufSize, sizeof(_Tp));
    }

    /**
     * @brief   Allocate memory for map
     * @param   __n: the number of node in map
     */
    _Tp** _allocate_map(size_t __n)
    { return _Map_alloc_type::allocate(__n); }

    /**
     * @brief   Release memory of map
     */
    void _deallocate_map(_Tp** __p, size_t __n)
    { _Map_alloc_type::deallocate(__p, __n); }

    /**
     * @brief   Allocate uninitialized memory for one node
     */
    _Tp* _allocate_node()
    { return _Node_alloc_type::allocate(_buffer_size()); }

    void _create_nodes(_Tp** __start, _Tp** __finish)
    {
        __SSTL_TRY {
            for(_Tp** cur = __start; cur < __finish; ++cur)
                *cur = _allocate_node();
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            _destroy_nodes(__start, __finish);
        }
#endif
    }

    /**
     * @brief   Deallocate node's memory referenced by pointer __p
     * @param   __p: pointer to be released
     */
    void _deallocate_node(_Tp* __p)
    { _Node_alloc_type::deallocate(__p, _buffer_size()); }

    void _destroy_nodes(_Tp** __start, _Tp** __finish)
    {
        for(_Tp** cur = __start; cur < __finish; ++cur)
            _deallocate_node(*cur);
    }

    /**
     * @brief   Allocate memory for map and each node
     * @param   __n: the number of bytes to allocate memory for node
     */
    void _initialize_map(size_t __n)
    {
        size_t new_map_size = __n / _buffer_size() + 1;
        m_map_size = max(_initial_map_size(), new_map_size + 2);
        m_map = _allocate_map(m_map_size);

        _Tp** p_start = m_map + (m_map_size - new_map_size) / 2;
        _Tp** p_finish = p_start + new_map_size;

        __SSTL_TRY {
            _create_nodes(p_start, p_finish);
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch (...) {
            _deallocate_map(m_map, m_map_size);
            m_map = 0;
            m_map_size = 0;
        }
#endif
        m_start.set_node(p_start);
        m_finish.set_node(p_finish - 1);
        m_start.m_cur = m_start.m_first;
        m_finish.m_cur = m_finish.m_first + __n % _buffer_size();
    }

    /**
     * @brief   Initialize value for all nodes
     */
    void _fill_initialize(const _Tp& __val)
    {
        for(_Tp** cur = m_start.m_node; cur < m_finish.m_node; ++cur) {
            uninitialized_fill(*cur, *cur + _buffer_size(), __val);
        }
        uninitialized_fill(m_finish.m_first, m_finish.m_cur, __val);
    }

protected:
    _Tp** m_map;
    size_t m_map_size;
    iterator m_start;
    iterator m_finish;
};


template <class _Tp, class _Alloc = __SSTL_DEFAULT_ALLOC, size_t BufSize = 512>
class deque: _deque_base<_Tp, _Alloc, BufSize> {
public:
    typedef _Tp         value_type;
    typedef size_t      size_type;
    typedef _Tp*        pointer;
    typedef const _Tp*  const_pointer;
    typedef _Tp&        reference;
    typedef const _Tp&  const_reference;
    typedef ptrdiff_t   difference_type;
    typedef _Tp**       map_pointer;

    typedef _deque_base<_Tp, _Alloc, BufSize>                       _Base;
    typedef _deque_iterator<_Tp, _Tp&, _Tp*, BufSize>               iterator;
    typedef _deque_iterator<_Tp, const _Tp&, const _Tp*, BufSize>   const_iterator;
    typedef typename _Base::allocator_type allocator_type;

    allocator_type get_allocator()
    { return _Base::get_allocator(); }

protected:
    using _Base::m_map;
    using _Base::m_map_size;
    using _Base::m_start;
    using _Base::m_finish;

    using _Base::_allocate_node;
    using _Base::_deallocate_node;
    using _Base::_allocate_map;
    using _Base::_deallocate_map;
    using _Base::_fill_initialize;

public:
    /**
     * @brief   Construct an new container from a variety of data sources
     */
    deque(const allocator_type& __a = allocator_type())
     : _Base(__a) {}

    deque(const deque& __x)
     : _Base(__x.get_allocator(), __x.size())
    { uninitialized_copy(__x.begin(), __x.end(), m_start); }

    deque(size_type __n, const allocator_type& __a = allocator_type())
     : _Base(__a, __n)
    { _fill_initialize(value_type()); }

    deque(size_type __n, const value_type& __val,
          const allocator_type& __a = allocator_type())
     : _Base(__a, __n)
    { _fill_initialize(__val); }

    ~deque() = default;

    /**
     * @brief   Returns an iterator to the first element
     */
    iterator begin() { return m_start; }
    const_iterator begin() const { return m_start; }

    /**
     * @brief   Returns an iterator to the element following the last element
     */
    iterator end() { return m_finish; }
    const_iterator end() const { return m_finish; }

    /**
     * @brief   Returns a reference to the element at specified location
     */
    reference operator[](size_type n) {
        return m_start[(difference_type)n];
    }

    /**
     * @brief   Returns a reference to the first element
     */
    reference front() { return *m_start; }
    const_reference front() const { return *m_start; }

    /**
     * @brief   Returns a reference to the last element
     */
    reference back() {
        iterator tmp = m_finish;
        --tmp;
        return *tmp;
    }

    const_reference back() const {
        iterator tmp = m_finish;
        --tmp;
        return *tmp;
    }

    /**
     * @brief   Returns the number of elements
     */
    size_type size() const
    {
        return (size_type)(m_finish - m_start);
    }

    /**
     * @brief   Returns the maximum number of elements
     */
    size_type max_size() const { return (size_type)-1; }

    /**
     * @brief   Checks if the container has no elements
     */
    bool empty() const { return m_start == m_finish; }

    /**
     * @brief   Appends the given value to the end of the container
     * @param   __val: the value of element to append
     */
    void push_back(const_reference __val)
    {
        if(m_finish.m_cur != m_finish.m_last - 1) {
            construct(m_finish.m_cur, __val);
            ++m_finish.m_cur;
        } else {
            value_type copy = __val;
            _reserve_map_at_back();
            *(m_finish.m_node + 1) = _allocate_node();
            __SSTL_TRY {
                construct(m_finish.m_cur, copy);
                m_finish.set_node(m_finish.m_node + 1);
                m_finish.m_cur = m_finish.m_first;
            }
#ifdef __SSTL_USE_EXCEPTIONS
            catch(...) {
                m_finish.set_node(m_finish.m_node - 1);
                m_finish.m_cur = m_finish.m_last - 1;
                _deallocate_node(*(m_start.m_node + 1));
            }
#endif
        }
    }

    /**
     * @brief   Prepends the given element value to the beginning of container
     * @param   value: the value of element to prepend
     */
    void push_front(const value_type& __val)
    {
        if(m_start.m_cur != m_start.m_first) {
            construct(m_start.m_cur - 1, __val);
            --m_start.m_cur;
        } else {
            value_type copy = __val;
            _reserve_map_at_front();
            *(m_start.m_node - 1) = _allocate_node();
            __SSTL_TRY {
                m_start.set_node(m_start.m_node - 1);
                m_start.m_cur = m_start.m_last -1;
                construct(m_start.m_cur, copy);
            }
#ifdef __SSTL_USE_EXCEPTIONS
            catch(...) {
                m_start.set_node(m_start.m_node + 1);
                m_start.m_cur = m_start.m_first;
                _deallocate_node(*(m_start.m_node - 1));
            }
#endif
        }
    }

    /**
     * @brief   Removes the last element of container
     */
    void pop_back()
    {
        if(m_finish.m_cur != m_finish.m_first) {
            --m_finish.m_cur;
            destroy(m_finish.m_cur);
        } else {
            _deallocate_node(m_finish.m_first);
            m_finish.set_node(m_finish.m_node - 1);
            m_finish.m_cur = m_finish.m_last - 1;
            destroy(m_finish.m_cur);
        }
    }

    /**
     * @brief   Removes the first element of container.
     *          If there are no elements, the behavior is undefined
     */
    void pop_front()
    {
        if(m_start.m_cur != m_start.m_last - 1) {
            destroy(m_start.m_cur);
            ++m_start.m_cur;
        } else {
            destroy(m_start.m_cur);
            _deallocate_node(m_start.m_first);
            m_start.set_node(m_start.m_node + 1);
            m_start.m_cur = m_start.m_first;
        }
    }

    /**
     * @brief   Erases the specified elements from container
     * @param   pos: iterator to the element to remove
     */
    iterator erase(iterator __pos)
    {
        iterator next = __pos;
        ++next;
        difference_type index = __pos - m_start;
        if(index < (size() >> 1)) {
            copy_backward(m_start, __pos, next);
            pop_front();
        } else {
            copy(next, m_finish, __pos);
            pop_back();
        }
        return m_start + index;
    }

    /**
     * @brief   Insert elements at the specified location in container
     * @param   pos: iterator before which the content will be inserted
     */
    void insert(iterator __pos, const value_type& __val)
    {
        if(__pos.m_cur == m_start.m_cur) { // add element to the beginning of container
            push_front(__val);
        } else if(__pos.m_cur == m_finish.m_cur) { // add element to the end of container
            push_back(__val);
        } else { // add element into the mid of container
            _insert_aux(__pos, __val);
        }
    }

    void insert(iterator __pos)
    { insert(__pos, value_type()); }

protected:
    void _insert_aux(iterator __pos, const value_type& __val)
    {
        difference_type index = __pos - m_start;
        value_type copy = __val;
        if(index < size() / 2) { // pos is before the middle of container
            push_front(front());
            iterator front1 = m_start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            __pos = m_start + index;
            iterator pos1 = __pos;
            ++pos1;
            copy(front2, pos1, front1);
        } else {
            push_back(back());

        }
        *__pos = copy;
        return __pos;
    }

    void _reserve_map_at_back(size_type __n_add = 1) {
        if(__n_add + 1 > m_map_size - (m_finish.m_node - m_map)) {
            _reallocate_map(__n_add, false);
        }
    }

    void _reserve_map_at_front(size_type __n_add = 1) {
        if(__n_add > m_start.m_node - m_map) {
            _reallocate_map(__n_add, true);
        }
    }

    void _reallocate_map(size_type __n_add, bool add_at_front) {
        size_type old_num_nodes = m_finish.m_node - m_start.m_node + 1;
        size_type new_num_nodes = old_num_nodes + __n_add;

        map_pointer new_start;
        if(m_map_size > 2 * new_num_nodes) {
            new_start = m_map + (m_map_size - new_num_nodes) / 2
                    + (add_at_front ? __n_add : 0);
            if(new_start < m_start.m_node)
                copy(m_start.m_node, m_finish.m_node + 1, new_start);
            else
                copy_backward(m_start.m_node, m_finish.m_node + 1, new_start + old_num_nodes);
        } else {
            size_type new_map_size = m_map_size + max(m_map_size, __n_add) + 2;
            map_pointer new_map = _allocate_map(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                    + (add_at_front ? __n_add : 0);
            copy(m_start.m_node, m_finish.m_node+1, new_start);
            _deallocate_map(m_map, m_map_size);
            m_map = new_map;
            m_map_size = new_map_size;
        }
        m_start.set_node(new_start);
        m_finish.set_node(new_start + old_num_nodes - 1);
    }
};

}; // sstl


#endif // SSTL_DEQUE_H
