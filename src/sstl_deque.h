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

template <class T, class Ref, class Ptr, size_t BufSize>
struct __deque_iterator {
    typedef __deque_iterator<T, T&, T*, BufSize>                iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSize>    const_iterator;

    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef Ptr                         pointer;
    typedef Ref                         reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    typedef T**                         map_pointer;
    typedef __deque_iterator            self;

    __deque_iterator(): m_node(0), m_cur(0), m_first(0), m_last(0) {}

    reference operator*() const {
        return *m_cur;
    }

    pointer operator->() const {
        return &(this->operator*());
    }

    difference_type operator-(const self& x) const {
        return buffer_size() * (m_node - x.m_node - 1) +
                (m_cur - m_first) + (x.m_last - x.m_cur);
    }

    self& operator++() {
        ++m_cur;
        if( m_cur == m_last ) {
            set_node(m_node + 1);
            m_cur = m_first;
        }
        return *this;
    }

    const self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if( m_cur == m_last ) {
            set_node(m_node - 1);
            m_cur = m_last;
        }
        --m_cur;
        return *this;
    }

    const self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (m_cur - m_first);
        if (offset >= 0 && offset < buffer_size()) {
            m_cur += n;
        } else {
            difference_type node_offset = offset > 0 ?
                    offset / buffer_size() :
                    -(-offset - 1) / buffer_size() - 1;
            set_node(m_node + node_offset);
            m_cur = m_first + (offset - node_offset * buffer_size());
        }
        return *this;
    }

    self operator+(difference_type n) const {
        return (self)*this += n;
    }

    self& operator-=(difference_type n) {
        return (self)*this += -n;
    }

    self operator-(difference_type n) const {
        return (self)*this -= n;
    }

    reference operator[](difference_type n) {
        return *(*this + n);
    }

    bool operator==(const self& that) const {
        return this->m_cur == that.m_cur;
    }

    bool operator!=(const self& that) const {
        return !(*this == that);
    }

    bool operator<(const self& that) {
        return m_node == that.m_node ?
                m_cur < that.m_cur :
                m_node < that.m_node;
    }

    inline difference_type buffer_size() const {
        return __deque_buf_size(BufSize, sizeof(T));
    }

    void set_node(map_pointer new_node)
    {
        m_node = new_node;
        m_first = *new_node;
        m_last = m_first + buffer_size();
    }

public:
    map_pointer m_node;
    T* m_cur;
    T* m_first;
    T* m_last;
};


template <class T, class Alloc = __SSTL_DEFAULT_ALLOC, size_t BufSize = 512>
class deque {
public:
    typedef T           value_type;
    typedef size_t      size_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef ptrdiff_t   difference_type;
    typedef T**         map_pointer;
    typedef size_type   map_size_type;
    typedef Alloc       allocator_type;

    typedef __deque_iterator<T, T&, T*, BufSize>                iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSize>    const_iterator;

    typedef __SSTL_ALLOC(value_type, Alloc) data_allocator;
    typedef __SSTL_ALLOC(pointer, Alloc)    map_allocator;

public:
    map_pointer m_map;
    map_size_type m_map_size;
    iterator m_start;
    iterator m_finish;

public:
    /**
     * @brief   Construct an new container from a variety of data sources
     */
    deque(): m_map(0), m_map_size(0)
    {
        fill_initialize(0, value_type());
        m_finish.m_node = m_start.m_node;
    }

    deque(const deque& x) : m_map(0), m_map_size(0)
    { uninitialized_copy(x.begin(), x.end(), m_start); }

    deque(size_type n, const value_type& value)
    : m_map(0), m_map_size(0)
    { fill_initialize(n ,value); }

    explicit deque(size_type n): m_map(0), m_map_size(0)
    { fill_initialize(n, value_type()); }

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
     * @param   value: the value of element to append
     */
    void push_back(const_reference value)
    {
        if(m_finish.m_cur != m_finish.m_last - 1) {
            construct(m_finish.m_cur, value);
            ++m_finish.m_cur;
        } else {
            value_type copy = value;
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
    void push_front(const_reference value)
    {
        if(m_start.m_cur != m_start.m_first) {
            construct(m_start.m_cur - 1, value);
            --m_start.m_cur;
        } else {
            value_type copy = value;
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
     * @brief   Erases all elements from container
     */
    void clear()
    {
        for(auto node = m_start.m_node + 1; node < m_finish.m_node; ++node) {
            destroy(*node, *node + _buffer_size());
            data_allocator::deallocate(*node, (size_type)_buffer_size());
        }

        if(m_start.m_node != m_finish.m_node) {
            destroy(m_start.m_cur, m_start.m_last);
            destroy(m_finish.m_first, m_finish.m_cur);
            data_allocator::deallocate(m_finish.m_first, (size_type)_buffer_size());
        } else {
            destroy(m_start.m_cur, m_finish.m_cur);
        }

        m_finish = m_start;
    }

    /**
     * @brief   Erases the specified elements from container
     * @param   pos: iterator to the element to remove
     */
    iterator erase(iterator pos)
    {
        iterator next = pos;
        ++next;
        difference_type index = pos - m_start;
        if(index < (size() >> 1)) {
            copy_backward(m_start, pos, next);
            pop_front();
        } else {
            copy(next, m_finish, pos);
            pop_back();
        }
        return m_start + index;
    }

    /**
     * @brief   Insert elements at the specified location in container
     * @param   pos: iterator before which the content will be inserted
     */
    void insert(iterator pos, const_reference x)
    {
        if(pos.m_cur == m_start.m_cur) { // add element to the beginning of container
            push_front(x);
        } else if(pos.m_cur == m_finish.m_cur) { // add element to the end of container
            push_back(x);
        } else { // add element into the mid of container
            _insert_aux(pos, x);
        }
    }

protected:
    void _insert_aux(iterator pos, const_reference x)
    {
        difference_type index = pos - m_start;
        value_type copy = x;
        if(index < size() / 2) { // pos is before the middle of container
            push_front(front());
            iterator front1 = m_start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = m_start + index;
            iterator pos1 = pos;
            ++pos1;
            copy(front2, pos1, front1);
        } else {
            push_back(back());

        }
        *pos = copy;
        return pos;
    }

    void _reserve_map_at_back(size_type nodes_to_add = 1) {
        if(nodes_to_add + 1 > m_map_size - (m_finish.m_node - m_map)) {
            _reallocate_map(nodes_to_add, false);
        }
    }

    void _reserve_map_at_front(size_type nodes_to_add = 1) {
        if(nodes_to_add > m_start.m_node - m_map) {
            _reallocate_map(nodes_to_add, true);
        }
    }

    void _reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = m_finish.m_node - m_start.m_node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_start;
        if(m_map_size > 2 * new_num_nodes) {
            new_start = m_map + (m_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
            if(new_start < m_start.m_node)
                copy(m_start.m_node, m_finish.m_node + 1, new_start);
            else
                copy_backward(m_start.m_node, m_finish.m_node + 1, new_start + old_num_nodes);
        } else {
            size_type new_map_size = m_map_size + max(m_map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
            copy(m_start.m_node, m_finish.m_node+1, new_start);
            map_allocator::deallocate(m_map, m_map_size);
            m_map = new_map;
            m_map_size = new_map_size;
        }
        m_start.set_node(new_start);
        m_finish.set_node(new_start + old_num_nodes - 1);
    }

    inline size_type _buffer_size() {
        return __deque_buf_size(BufSize, sizeof(T));
    }

    void fill_initialize(size_type n, const_reference value)
    {
        size_type new_map_size = n / _buffer_size() + 1;
        m_map_size = max(_initial_map_size(), new_map_size + 2);
        m_map = map_allocator::allocate(m_map_size);
        map_pointer p_start = m_map + (m_map_size - new_map_size) / 2;
        map_pointer p_finish = p_start + new_map_size - 1;

        __SSTL_TRY {
            for(map_pointer p_map = p_start; p_map <= p_finish; ++p_map) {
                *p_map = _allocate_node();
            }
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch (...) {
            throw;
        }
#endif
        m_start.set_node(p_start);
        m_finish.set_node(p_finish);
        m_start.m_cur = m_start.m_first;
        m_finish.m_cur = m_finish.m_first + n % _buffer_size();

        __SSTL_TRY
        {
            for(map_pointer cur = m_start.m_node; cur < m_finish.m_node; ++cur) {
                uninitialized_fill(*cur, *cur + _buffer_size(), value);
            }
            uninitialized_fill(m_finish.m_first, m_finish.m_last, value);
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            throw;
        }
#endif
    }

    size_type _initial_map_size() { return 8; }

    pointer _allocate_node()
    {
        return data_allocator::allocate(_buffer_size());
    }

    void _deallocate_node(pointer p)
    {
        data_allocator::deallocate(p, _buffer_size());
    }
};

}; // sstl


#endif // SSTL_DEQUE_H
