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

    typedef __deque_iterator self;
    T* m_cur;
    T* m_first;
    T* m_last;
    map_pointer m_node;

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

protected:
    inline difference_type buffer_size() {
        return __deque_buf_size(BufSize, sizeof(T));
    }

    void set_node(map_pointer new_node)
    {
        m_node = new_node;
        m_first = *new_node;
        m_last = m_first + buffer_size();
    }
};


template <class T, class Alloc = __SSTL_DEFAULT_ALLOC(T), size_t BufSize = 0>
class deque {
public:
    typedef T                   value_type;
    typedef size_t              size_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef T&                  reference;
    typedef const value_type&   const_reference;
    typedef ptrdiff_t           difference_type;

protected:
    typedef __deque_iterator<T, T&, T*, BufSize>                iterator;
    typedef __deque_iterator<T, const T&, const T*, BufSize>    const_iterator;
    typedef pointer*    map_pointer;
    typedef size_type   map_size_type;
    typedef Alloc       allocator_type;
    typedef __SSTL_ALLOC(value_type, Alloc)     data_allocator;
    typedef __SSTL_ALLOC(map_pointer, Alloc)    map_allocator;

    map_pointer map;
    map_size_type map_size;
    iterator start;
    iterator finish;

public:
    /**
     * @brief   Construct an new container from a variety of data sources
     */
    deque() {}

    deque(const deque& x)
    {
        uninitialized_copy(x.begin(), x.end(), start);
    }

    deque(size_type n, const value_type& value): start(0), finish(0), map(0), map_size(0) {
        fill_initialize(n ,value);
    }

    explicit deque(size_type n)
    {
        fill_initialize(n, value_type());
    }

    /**
     * @brief   Returns an iterator to the first element
     */
    iterator begin() { return start; }
    const_iterator begin() const { return start; }

    /**
     * @brief   Returns an iterator to the element following the last element
     */
    iterator end() { return finish; }
    const_iterator end() const { return finish; }

    /**
     * @brief   Returns a reference to the element at specified location
     */
    reference operator[](size_type n) {
        return start[(difference_type)n];
    }

    /**
     * @brief   Returns a reference to the first element
     */
    reference front() { return *start; }
    const_reference front() const { return *start; }

    /**
     * @brief   Returns a reference to the last element
     */
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    const_reference back() const {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    /**
     * @brief   Returns the number of elements
     */
    size_type size() const {
        return (size_type)(finish - start);
    }

    /**
     * @brief   Returns the maximum number of elements
     */
    size_type max_size() const { return (size_type)-1; }

    /**
     * @brief   Checks if the container has no elements
     */
    bool empty() const { return start == finish; }

    /**
     * @brief   Appends the given value to the end of the container
     * @param   value: the value of element to append
     */
    void push_back(const_reference value)
    {
        if(finish.m_cur != finish.m_last - 1) {
            construct(finish.m_cur, value);
            ++finish.m_cur;
        } else {
            value_type copy = value;
            reserve_map_at_back();
            *(finish.m_node + 1) = allocate_node();
            __SSTL_TRY {
                construct(finish.m_cur, copy);
                finish.set_node(finish.m_node + 1);
                finish.m_cur = finish.m_first;
            }
#ifdef __SSTL_USE_EXCEPTIONS
            catch(...) {
                finish.set_node(finish.m_node - 1);
                finish.m_cur = finish.m_last - 1;
                _deallocate_node(*(start.m_node + 1));
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
        if(start.m_cur != start.m_first) {
            construct(start.m_cur - 1, copy);
            --start.m_cur;
        } else {
            value_type copy = value;
            reserve_map_at_front();
            *(start.m_node - 1) = allocate_node();
            __SSTL_TRY {
                start.set_node(start.m_node - 1);
                start.m_cur = start.m_last -1;
                construct(start.m_cur, copy);
            }
#ifdef __SSTL_USE_EXCEPTIONS
            catch(...) {
                start.set_node(start.m_node + 1);
                start.m_cur = start.m_first;
                _deallocate_node(*(start.m_node - 1));
            }
#endif
        }
    }

    /**
     * @brief   Removes the last element of container
     */
    void pop_back()
    {
        if(finish.m_cur != finish.m_first) {
            --finish.m_cur;
            destroy(finish.m_cur);
        } else {
            _deallocate_node(finish.m_first);
            finish.set_node(finish.node-1);
            finish.m_cur = finish.m_last - 1;
            destroy(finish.m_cur);
        }
    }

    /**
     * @brief   Removes the first element of container.
     *          If there are no elements, the behavior is undefined
     */
    void pop_front()
    {
        if(start.m_cur != start.m_last - 1) {
            destroy(start.m_cur);
            ++start.m_cur;
        } else {
            destroy(start.m_cur);
            _deallocate_node(start.m_first);
            start.set_node(start.m_node + 1);
            start.m_cur = start.m_first;
        }
    }

    /**
     * @brief   Erases all elements from container
     */
    void clear()
    {
        for(auto node = start.m_node + 1; node < finish.m_node; ++node) {
            destroy(*node, *node + buffer_size());
            data_allocator::deallocate(*node, (size_type)buffer_size());
        }

        if(start.m_node != finish.m_node) {
            destroy(start.m_cur, start.m_last);
            destroy(finish.m_first, finish.m_cur);
            data_allocator::deallocate(finish.m_first, (size_type)buffer_size());
        } else {
            destroy(start.m_cur, finish.m_cur);
        }

        finish = start;
    }

    /**
     * @brief   Erases the specified elements from container
     * @param   pos: iterator to the element to remove
     */
    iterator erase(iterator pos)
    {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if(index < (size() >> 1)) {
            copy_backward(start, pos, next);
            pop_front();
        } else {
            copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    /**
     * @brief   Insert elements at the specified location in container
     * @param   pos: iterator before which the content will be inserted
     */
    iterator insert(iterator pos, const_reference x)
    {
        if(pos.m_cur == start.m_cur) {
            push_front(x);
            return start;
        } else if(pos.m_cur == finish.m_cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else {
            return insert_aux(pos, x);
        }
    }

protected:
    iterator insert_aux(iterator pos, const_reference x)
    {
        difference_type index = pos - start;
        value_type copy = x;
        if(index < size() / 2) {
            push_front(front());
            iterator front1 = start;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start + index;
            iterator pos1 = pos;
            ++pos1;
            copy(front2, pos1, front1);
        } else {

        }
        *pos = copy;
        return pos;
    }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if(nodes_to_add + 1 > map_size - (finish.m_node - map)) {
            reallocate_map(nodes_to_add, false);
        }
    }

    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if(nodes_to_add > start.m_node - map) {
            reallocate_map(nodes_to_add, true);
        }
    }

    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish.m_node - start.m_node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;

        map_pointer new_start;
        if(map_size > 2 * new_num_nodes) {
            new_start = map + (map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
            if(new_start < start.m_node)
                copy(start.m_node, finish.m_node + 1, new_start);
            else
                copy_backward(start.m_node, finish.m_node + 1, new_start + old_num_nodes);
        } else {
            size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
            map_pointer new_map = map_allocator::allocate(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
            copy(start.m_node, finish.m_node+1, new_start);
            map_allocator::deallocate(map, map_size);
            map = new_map;
            map_size = new_map_size;
        }
        start.set_node(new_start);
        finish.set_node(new_start + old_num_nodes - 1);
    }

    inline size_type buffer_size() {
        return __deque_buf_size(BufSize, sizeof(T));
    }

    void _fill_initialize(size_type n, const_reference value)
    {
        _create_map_and_nodes(n);
        map_pointer cur;
        __SSTL_TRY
        {
            for(cur = start.m_node; cur < finish.m_node; ++cur) {
                uninitialized_fill(*cur, *cur+buffer_size(), value);
                uninitialized_fill(finish.m_first, finish.m_last, value);
            }
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            throw;
        }
#endif
    }

    void _create_map_and_nodes(size_type num_elements) {
        size_type num_nodes = num_elements / buffer_size() + 1;
        map_size = max(initial_map_size(), num_nodes + 2);
        map = map_allocator::allocate(map_size);
        map_pointer p_start = map + (map_size - num_nodes) / 2;
        map_pointer p_finish = p_start + num_nodes - 1;
        map_pointer cur;

        __SSTL_TRY {
            for(cur = p_start; cur <= p_finish; ++cur) {
                *cur = allocate_node();
            }
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch (...) {
            throw;
        }
#endif
        start.set_node(p_start);
        finish.set_node(p_finish);
        start.m_cur = start.m_first;
        finish.m_cur = finish.m_first + num_elements % buffer_size();
    }

    size_type initial_map_size() { return 8; }

    pointer allocate_node()
    {
        return map_allocator::allocate((size_type)buffer_size());
    }

    void _deallocate_node(pointer p)
    {
        data_allocator::deallocate(p, (size_type)buffer_size());
    }
};

}; // sstl

#endif // SSTL_DEQUE_H
