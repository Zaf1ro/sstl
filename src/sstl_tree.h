#ifndef SSTL_TREE_H
#define SSTL_TREE_H

#include "sstl_config.h"
#include "sstl_iterator.h"
#include "sstl_alloc.h"
#include "sstl_construct.h"
#include "sstl_pair.h"


namespace sstl {

typedef bool __rb_tree_color_type;

const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

class __rb_tree_node_base {
private:
    typedef __rb_tree_color_type _Color_type;
    typedef __rb_tree_node_base* _Base_ptr;

public:
    _Color_type m_color;
    _Base_ptr m_parent;
    _Base_ptr m_left;
    _Base_ptr m_right;

    /**
     * @brief   Return the pointer to the node
     *          with minimum value in the subtree
     * @param   __x: the root of subtree
     */
    static _Base_ptr minimum(_Base_ptr __x)
    {
        while(__x->m_left != nullptr) __x = __x->m_left;
        return __x;
    }

    /**
     * @brief   Return the pointer to the node
     *          with maximum value in the subtree
     * @param   __x: the root of subtree
     */
    static _Base_ptr maximum(_Base_ptr __x)
    {
        while(__x->m_right != nullptr ) __x = __x->m_right;
        return __x;
    }
};

template <class _Tp>
class __rb_tree_node: public __rb_tree_node_base
{
public:
    _Tp m_data;
};

class __rb_tree_iterator_base
{
protected:
    typedef __rb_tree_node_base* _Base_ptr;
    _Base_ptr m_node;

    /**
     * @brief   Move m_node to the node whose value
     *          is larger than current value
     */
    void increment()
    {
        if(m_node->m_right != nullptr) {
            m_node = m_node->m_right;
            while(m_node->m_left != nullptr)
                m_node = m_node->m_left;
        } else {
            _Base_ptr p = m_node->m_parent;
            while(m_node == p->m_right) {
                m_node = p;
                p = p->m_parent;
            }
            if(m_node->m_right != p)
                m_node = p;
        }
    }

    /**
     * @brief   Move m_node to the node whose value
     *          is smaller than current value
     */
    void decrement()
    {
        if(m_node->m_color == __rb_tree_red &&
           m_node->m_parent->m_parent == m_node)
            m_node = m_node->m_right;
        else if(m_node->m_left != nullptr) {
            _Base_ptr r = m_node->m_left;
            while(r->m_right != nullptr)
                r = r->m_right;
            m_node = r;
        } else {
            _Base_ptr p = m_node->m_parent;
            while(m_node == p->m_left) {
                m_node = p;
                p = m_node->m_parent;
            }
            m_node = p;
        }
    }
};

template <class _Tp, class _Ref, class _Ptr>
class __rb_tree_iterator: public __rb_tree_iterator_base
{
public:
    typedef _Tp         value_type;
    typedef _Ref        reference;
    typedef _Ptr        pointer;
    typedef ptrdiff_t   difference_type;

    typedef bidirectional_iterator_tag iterator_category;

private:
    typedef __rb_tree_node<_Tp>* _Link_type;
    typedef __rb_tree_iterator<_Tp, _Ref, _Ptr> _Self;
    typedef __rb_tree_iterator<_Tp, _Tp&, _Tp*> iterator;

public:
    __rb_tree_iterator() = default;

    __rb_tree_iterator(_Link_type __x)
    { m_node = __x; }

    __rb_tree_iterator(const iterator& __it)
    { m_node = __it.m_node; }

    reference operator*() const
    { return ((_Link_type)m_node)->m_data; }

    pointer operator->() const
    { return &(operator*()); }

    _Self& operator++()
    {
        increment();
        return *this;
    }

    const _Self operator++(int)
    {
        _Self tmp = *this;
        increment();
        return tmp;
    }

    _Self& operator--()
    {
        decrement();
        return *this;
    }

    const _Self operator--(int)
    {
        _Self tmp = *this;
        decrement();
        return tmp;
    }

    friend inline bool operator==(const iterator& __x,
                                  const iterator& __y)
    { return __x.m_node == __y.m_node; }

    friend inline bool operator!=(const iterator& __x,
                                  const iterator& __y)
    { return __x.m_node != __y.m_node; }
};

/**
 * @brief   Make a rotation of tree in anti-clockwise direction
 * @param   __x: root node of subtree which will be rotated
 * @param   __root: root node of rb-tree
 */
inline void
__rb_tree_rotate_left(__rb_tree_node_base* __x,
                      __rb_tree_node_base*& __root)
{
    __rb_tree_node_base* y = __x->m_right;
    // connect x and left child of y
    __x->m_right = y->m_left;
    if(y->m_left != nullptr)
        y->m_left->m_parent = __x;

    // connect y and parent of x
    if(__x == __root)
        __root = y;
    else if(__x == __x->m_parent->m_left)
        __x->m_parent->m_left = y;
    else
        __x->m_parent->m_right = y;
    y->m_parent = __x->m_parent;

    // connect x and y
    y->m_left = __x;
    __x->m_parent = y;
}

/**
 * @brief   Make a rotation of tree in clockwise direction
 * @param   __x: root node of subtree which will be rotated
 * @param   __root: root node of rb-tree
 */
inline void
__rb_tree_rotate_right(__rb_tree_node_base* __x,
                       __rb_tree_node_base*& __root)
{
    __rb_tree_node_base* y = __x->m_left;
    // connect x and right child of y
    __x->m_left = y->m_right;
    if(y->m_right != nullptr)
        y->m_right->m_parent = __x;

    // connect y and parent of x
    if(__x == __root)
        __root = y;
    else if(__x == __x->m_parent->m_right)
        __x->m_parent->m_right = y;
    else
        __x->m_parent->m_left = y;
    y->m_parent = __x->m_parent;

    // connect x and y
    y->m_right = __x;
    __x->m_parent = y;
}

/**
 * @brief   Rebalance tree after inserting new node to keep:
 *          1. Each node is either red or black
 *          2. The color of root is black
 *          3. All color of leaves are black
 *          4. If color of a node is red, the both color of its
 *             children are black
 *          5. Every path from give node to any leaves contains
 *             same number of black nodes
 * @param   __x: new node to insert
 * @param   __root: root node of rb-tree
 */
inline void
__rb_tree_rebalance(__rb_tree_node_base* __x,
                    __rb_tree_node_base*& __root)
{
    __x->m_color = __rb_tree_red; // the color of new node must be red
    while(__x != __root && __x->m_parent->m_color == __rb_tree_red) {
        if(__x->m_parent == __x->m_parent->m_parent->m_left) { // parent is the left child node of grandparent
            __rb_tree_node_base* y = __x->m_parent->m_parent->m_right; // uncle
            if(y && y->m_color == __rb_tree_red) { // the color of uncle is red
                __x->m_parent->m_color = __rb_tree_black;
                y->m_color = __rb_tree_black;
                __x->m_parent->m_parent->m_color = __rb_tree_red;
                __x = __x->m_parent->m_parent;
            } else { // the color of uncle is black
                if(__x == __x->m_parent->m_right) {
                    __x = __x->m_parent;
                    __rb_tree_rotate_left(__x, __root);
                }
                __x->m_parent->m_color = __rb_tree_black;
                __x->m_parent->m_parent->m_color = __rb_tree_red;
                __rb_tree_rotate_right(__x->m_parent->m_parent, __root);
            }
        } else { // parent is the right child node of grandparent
            __rb_tree_node_base* y = __x->m_parent->m_parent->m_left; // uncle
            if(y && y->m_color == __rb_tree_red) {
                __x->m_parent->m_color = __rb_tree_black;
                y->m_color = __rb_tree_black;
                __x->m_parent->m_parent->m_color = __rb_tree_red;
                __x = __x->m_parent->m_parent;
            } else { //
                if(__x == __x->m_parent->m_left) {
                    __x = __x->m_parent;
                    __rb_tree_rotate_right(__x, __root);
                }
                __x->m_parent->m_color = __rb_tree_black;
                __x->m_parent->m_parent->m_color = __rb_tree_red;
                __rb_tree_rotate_left(__x->m_parent->m_parent, __root);
            }
        }
    }
    __root->m_color = __rb_tree_black;
}

template <class _Tp, class _Alloc>
class __rb_tree_base {
protected:
    typedef _Alloc allocator_type;
    typedef __SSTL_ALLOC(__rb_tree_node<_Tp>, _Alloc)
            node_allocator_type;

    allocator_type get_allocator() const
    { return allocator_type(); }

    __rb_tree_node<_Tp>* _allocate_node()
    { return node_allocator_type::allocate(1); }

    void _deallocate_node(__rb_tree_node<_Tp>* __p)
    { node_allocator_type::deallocate(__p, 1); }

public:
    explicit __rb_tree_base(const allocator_type&) {}

protected:
    __rb_tree_node<_Tp>* m_header;
};

template <class _Key,
          class _Value,
          class _KeyOfValue,
          class _Compare,
          class _Alloc>
class __rb_tree: __rb_tree_base<_Value, _Alloc> {
private:
    typedef __rb_tree_node_base*    _Base_ptr;
    typedef __rb_tree_node<_Value>  _Node;
    typedef __rb_tree_node<_Value>* _Link_type;
    typedef __rb_tree_color_type    _Color_type;

    typedef __rb_tree_base<_Value, _Alloc> _Base;

public:
    typedef _Key        key_type;
    typedef _Value      value_type;
    typedef _Value*     pointer;
    typedef _Value&     reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef const _Value* const_pointer;
    typedef const _Value& const_reference;

    typedef typename _Base::allocator_type allocator_type;

    allocator_type get_allocator() const
    { return allocator_type(); }

private:
    using _Base::_allocate_node;
    using _Base::_deallocate_node;

    /**
     * @brief   Allocate and construct an object
     * @param   __x: element value to create
     */
    _Link_type _create_node(const value_type& __x)
    {
        _Link_type p = _allocate_node();
        __SSTL_TRY {
            sstl::construct(p, __x);
        }
    #ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            _deallocate_node(p);
        }
        return p;
    #endif
    }

    /**
     * @brief   Clone a node from another node
     * @param   __p: pointer to the node which will be copied
     */
    _Link_type _clone_node(_Link_type __p)
    {
        _Link_type p_clone = _create_node(__p->m_data);
        p_clone->color = __p->color;
        p_clone->m_left = 0;
        p_clone->m_right = 0;
        return p_clone;
    }

    /**
     * @brief   Destroy data and reallocate a node
     * @param   __p: pointer to node which will be deleted
     */
    void _destroy_node(_Link_type __p)
    {
        sstl::destroy(&__p->m_data);
        _deallocate_node(__p);
    }

protected:
    using _Base::m_header;
    size_type m_node_count;
    _Compare m_key_compare;

    /**
     * @brief   Return the reference to root node
     */
    _Link_type& _root() const
    { return (_Link_type&)(m_header->m_parent); }

    /**
     * @brief   Return the reference to smallest node
     */
    _Link_type& _leftmost() const
    { return (_Link_type&)(m_header->m_left); }

    /**
     * @brief   Return the reference to largest node
     */
    _Link_type& _rightmost() const
    { return (_Link_type&)(m_header->m_right); }

    /**
     * @brief   Return the reference to left-child node
     */
    static _Link_type& _left(_Base_ptr p)
    { return (_Link_type&)(p->m_left); }

    /**
     * @brief   Return the reference to right-child node
     */
    static _Link_type& _right(_Base_ptr p)
    { return (_Link_type&)(p->m_right); }

    /**
     * @brief   Return the reference to parent node
     */
    static _Link_type& _parent(_Base_ptr p)
    { return (_Link_type&)(p->m_parent); }

    /**
     * @brief   Return the reference to node value
     */
    static const value_type& _value(_Base_ptr p)
    { return ((_Link_type&)p)->m_data; }

    /**
     * @brief   Return the reference to node key
     */
    static const key_type& _key(_Base_ptr p)
    { return _KeyOfValue()(_value(p)); }

    /**
     * @brief   Return the reference to node color
     */
    static _Color_type& _color(_Base_ptr p)
    { return p->m_color; }

    /**
     * @brief   Return the reference to leftmost node
     */
    static _Link_type minimum(_Base_ptr p)
    { return __rb_tree_node_base::minimum(p); }

    /**
     * @brief   Return the reference to rightmost node
     */
    static _Link_type maximum(_Base_ptr p)
    { return __rb_tree_node_base::maximum(p); }

public:
    typedef __rb_tree_iterator<_Value, _Value&, _Value*>
            iterator;
    typedef __rb_tree_iterator<_Value, const _Value&, const _Value*>
            const_iterator;
private:
    /**
     * @brief   Insert node into rb-tree
     * @param   __cur: the pointer to insert point
     * @param   __pare: the pointer to the parent of insert point
     * @param   __val: the value which will be inserted
     */
    iterator _insert(_Base_ptr __cur,
                     _Base_ptr __pare,
                     const value_type& __val)
    {
        _Link_type cur = __cur;
        _Link_type pare = __pare;
        _Link_type tmp = _create_node(__val);;

        /* When we add to the left child node:
         * 1. there's no node in rb-tree
         * 2. there's already one node with __val
         * 3. __val is larger than parent's value
         */
        if(pare == m_header || cur != 0 ||
           m_key_compare(_KeyOfValue()(__val), _key(pare))) {
            _left(pare) = tmp;
            if(pare == m_header) { /* parent is m_header */
                _root() = tmp;
                _rightmost() = tmp;
            } else if(pare == _leftmost()) /* parent is the smallest node */
                _leftmost() = tmp;
        } else { /* add node to the right child node */
            _right(pare) = tmp;
            if(pare == _rightmost()) /* parent is the largest node */
                _rightmost() = tmp;
        }

        _parent(tmp) = pare;
        _left(tmp) = nullptr;
        _right(tmp) = nullptr;

        __rb_tree_rebalance(tmp, m_header->m_parent);
        ++m_node_count;
        return iterator(tmp);
    }

    /**
     * @brief   Copy __x and all subtree to another node
     * @param   __x: root node of subtree which will be copied
     * @param   __p: root node to contain subtree
     */
    _Link_type _copy(_Link_type __x, _Link_type __p)
    {
        _Link_type top = _clone_node(__x);
        top->m_parent = __p;

        // clone right child node
        if(__x->m_right)
            top->m_right = _copy(right(__x), top);
        __p = top;
        __x = _left(__x);

        // clone left child node
        while(__x != nullptr) {
            _Link_type y = _clone_node(__x);
            __p->m_left = y;
            y->m_parent = __p;
            if(__x->m_right)
                y->m_right = _copy(right(__x), y);
            __p = y;
            __x = _left(__x);
        }
        return top;
    }

    /**
     * @brief   Erase all nodes of __x and its subtree
     */
    void _erase(_Link_type __x)
    {
        while(__x != nullptr) {
            _erase(_right(__x));
            _Link_type y = _left(__x);
            _destroy_node(__x);
            __x = y;
        }
    }

    /**
     * @brief   initialize header with left, right and color
     */
    void _empty_initialize() {
        m_header = _allocate_node();
        _color(m_header) = __rb_tree_red;
        _root() = 0;
        _leftmost() = m_header;
        _rightmost() = m_header;
    }

public:
    __rb_tree()
     : _Base(allocator_type()), m_node_count(0), m_key_compare()
    { _empty_initialize(); }

    __rb_tree(const _Compare& __comp)
     : _Base(allocator_type()), m_node_count(0), m_key_compare(__comp)
    { _empty_initialize(); }

    __rb_tree(const _Compare& comp, const allocator_type& __a)
     : _Base(__a), m_node_count(0), m_key_compare(comp)
    { _empty_initialize(); }

    __rb_tree(const __rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x)
     : _Base(__x.get_allocator()), m_node_count(0), m_key_compare(__x.m_key_compare) {
        if (__x._root() == nullptr) {
            _empty_initialize();
        } else {
            _color(m_header) = __rb_tree_red;
            _root() = _copy(__x._root(), m_header);
            _leftmost() = minimum(_root());
            _rightmost() = maximum(_root());
        }
    }

    ~__rb_tree() { clear(); }

    __rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>&
    operator=(const __rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>& __x)
    {
        if(this == &__x)
            return *this;

        clear(); // clear all old nodes
        m_node_count = 0;
        m_key_compare = __x.m_key_compare;
        if(__x._root() == nullptr) { // __x is empty
            _root() = nullptr;
            _leftmost() = m_header;
            _rightmost() = m_header;
        } else {
            _root() = _copy(__x._root(), m_header);
            _leftmost() = minimum(_root());
            _rightmost() = maximum(_root());
            m_node_count = __x.m_node_count;
        }
        return *this;
    }

public:
    /**
     * @brief   Return the function object that comapre keys
     */
    _Compare key_comp() const { return m_key_compare; }

    /**
     * @brief   Return an iterator to the leftmost node
     */
    iterator begin() { return _leftmost(); }

    /**
     * @brief   Return an iterator to the rightmost node
     */
    iterator end() { return _rightmost(); }

    /**
     * @brief   Check whether the rb-tree is empty
     */
    bool empty() const { return m_node_count == 0; }

    /**
     * @brief   Return the number of nodes
     */
    size_type size() const { return m_node_count; }

    /**
     * @brief   Return the maximum possible number of nodes
     */
    size_type max_size() const { return size_type(-1); }

public:
    /**
     * @brief   Returns an iterator pointing to the
     *          first element that is not less than value
     * @param   __k: key value to search
     */
    iterator lower_bound(const key_type& __k)
    {
        _Link_type y = m_header;
        _Link_type x = _root();
        while(x != nullptr) {
            if(!m_key_compare(_key(x), __k)) { // the value of current node is larger than __k
                y = x;
                x = _left(x);
            } else
                x = _right(x);
        }
        return y;
    }

    const_iterator lower_bound(const key_type& __k) const
    {
        _Link_type y = m_header;
        _Link_type x = _root();
        while(x != nullptr) {
            if(!m_key_compare(_key(x), __k)) { // the value of current node is larger than __k
                y = x;
                x = _left(x);
            } else
                x = _right(x);
        }
        return y;
    }

    /**
     * @brief   Returns an iterator pointing to the
     *          first element that is not larger than value
     * @param   __k: key value to search
     */
    iterator upper_bound(const key_type& __k)
    {
        _Link_type y = m_header;
        _Link_type x = _root();
        while(x != nullptr) {
            if(m_key_compare(__k, _key(x))) {
                y = x;
                x = _left(x);
            } else
                x = _right(x);
        }
        return y;
    }

    const_iterator upper_bound(const key_type& __k) const
    {
        _Link_type y = m_header;
        _Link_type x = _root();
        while(x != nullptr) {
            if(m_key_compare(__k, _key(x))) {
                y = x;
                x = _left(x);
            } else
                x = _right(x);
        }
        return y;
    }

    /**
     * @brief   Return a range containing all elements
     *          with the give key
     * @param   __k: key value to compare the elements to
     */
    pair<iterator, iterator>
    equal_range(const key_type& __k)
    {
        return pair<iterator, iterator>(
                lower_bound(__k), upper_bound(__k));
    }

    pair<const_iterator, const_iterator>
    equal_range(const key_type& __k) const
    {
        return pair<const_iterator, const_iterator>(
            lower_bound(__k), upper_bound(__k));
    }

    /**
     * @brief   Return the number of elements with key
     * @param   key value of the elements to count
     */
    size_type count(const key_type& __k) const
    {
        pair<const_iterator, const_iterator> p = equal_range(__k);
        difference_type n = __DISTANCE(p.first, p.second);
        return (size_type)n;
    }

    /**
     * @brief   Insert element which is not in rb-tree
     * @param   __val: value to insert
     */
    pair<iterator, bool> insert_unique(const value_type& __val)
    {
        _Link_type y = m_header; // parent node
        _Link_type x = _root(); // root node
        bool comp = true;
        while(x != 0) {
            y = x;
            comp = m_key_compare(_KeyOfValue()(__val), _key(x));
            x = comp ? _left(x) : _right(x);
        }
        iterator j = iterator(y);
        if(comp) // insert node into left
            if(j == begin()) {
                return pair<iterator, bool>(_insert(x, y, __val), true);
            } else {
                --j;
            }
        
        // insert node into right
        if(m_key_compare(_key(j.m_node), _KeyOfValue()(__val))) {
            return pair<iterator, bool>(_insert(x, y, __val), true);
        }
        return pair<iterator, bool>(j, false); // duplicate node
    }

    /**
     * @brief   Insert element which may be existed
     */
    iterator insert_equal(const value_type& __val)
    {
        _Link_type y = m_header;
        _Link_type x = _root();
        while(x != 0) {
            y = x;
            x = m_key_compare(_KeyOfValue()(__val), _key(x)) ? _left(x) : _right(x);
        }
        return _insert(x, y, __val);
    }

    /**
     * @brief   Find an element with key equivalent to key
     * @param   __k: keu value of the element to search for
     */
    iterator find(const key_type& __k)
    {
        _Link_type y = m_header();
        _Link_type x = _root();

        while(x != 0) {
            if(!m_key_compare(_key(x), __k)) {
                y = x;
                x = _left(x);
            } else
                x = _right(x);
        }
        iterator j = y;
        return j == end() || m_key_compare(__k, _key(j.m_node)) ?
               end() : j;
    }

    /**
     * @brief   Erase all elements from rb-tree
     */
    void clear() {
        if (m_node_count == 0)
            return;
        _erase(_root());
        _leftmost() = m_header;
        _root() = 0;
        _rightmost() = m_header;
        m_node_count = 0;
    }
};

}; // sstl


#endif // SSTL_TREE_H
