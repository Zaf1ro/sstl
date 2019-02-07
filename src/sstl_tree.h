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

struct __rb_tree_node_base {
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type m_color;
    base_ptr m_parent;
    base_ptr m_left;
    base_ptr m_right;

    static base_ptr minimum(base_ptr x)
    {
        while(x->m_left != NULL)
            x = x->m_left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while(x->m_right != NULL)
            x = x->m_right;
        return x;
    }
};

template <class _Tp>
struct __rb_tree_node: public __rb_tree_node_base
{
    typedef __rb_tree_node<_Tp>* link_type;
    _Tp m_data;
};

struct __rb_tree_iterator_base
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_tag;
    typedef ptrdiff_t difference_type;

    base_ptr m_node;

    void increment()
    {
        if(m_node->m_right != NULL) {
            m_node = m_node->m_right;
            while(m_node->m_left != NULL)
                m_node = m_node->m_left;
        } else {
            base_ptr p = m_node->m_parent;
            while(m_node == p->m_right) {
                m_node = p;
                p = p->m_parent;
            }
            if(m_node->m_right != p)
                m_node = p;
        }
    }

    void decrement()
    {
        if(m_node->m_color == __rb_tree_red &&
           m_node->m_parent->m_parent == m_node)
            m_node = m_node->m_right;
        else if(m_node->m_left != NULL) {
            base_ptr r = m_node->m_left;
            while(r->m_right != NULL)
                r = r->m_right;
            m_node = r;
        } else {
            base_ptr p = m_node->m_parent;
            while(m_node == p->m_left) {
                m_node = p;
                p = m_node->m_parent;
            }
            m_node = p;
        }
    }
};

template <class _Tp, class _Ref, class _Ptr>
struct __rb_tree_iterator: public __rb_tree_iterator_base
{
    typedef _Tp     value_type;
    typedef _Ref    reference;
    typedef _Ptr    pointer;
    typedef __rb_tree_node<_Tp>* link_type;

    typedef __rb_tree_iterator<_Tp, _Tp&, _Tp*>             iterator;
    typedef __rb_tree_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
    typedef __rb_tree_iterator<_Tp, _Ref, _Ptr>             _Self;

    __rb_tree_iterator() {}
    __rb_tree_iterator(link_type x) { m_node = x; }
    __rb_tree_iterator(const iterator& it) { m_node = it.m_node; }

    reference operator*() const
    { return ((link_type)m_node)->m_data; }

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
};

inline void
__rb_tree_rotate_left(__rb_tree_node_base* x,
                      __rb_tree_node_base*& root)
{
    __rb_tree_node_base* y = x->m_right;
    // connect x and left child of y
    x->m_right = y->m_left;
    if(y->m_left != NULL)
        y->m_left->m_parent = x;

    // connect y and parent of x
    if(x == root)
        root = y;
    else if(x == x->m_parent->m_left)
        x->m_parent->m_left = y;
    else
        x->m_parent->m_right = y;
    y->m_parent = x->m_parent;

    // connect x and y
    y->m_left = x;
    x->m_parent = y;
}

inline void
__rb_tree_rotate_right(__rb_tree_node_base* x,
                       __rb_tree_node_base*& root)
{
    __rb_tree_node_base* y = x->m_left;
    // connect x and right child of y
    x->m_left = y->m_right;
    if(y->m_right != NULL)
        y->m_right->m_parent = x;

    // connect y and parent of x
    if(x == root)
        root = y;
    else if(x == x->m_parent->m_right)
        x->m_parent->m_right = y;
    else
        x->m_parent->m_left = y;
    y->m_parent = x->m_parent;

    // connect x and y
    y->m_right = x;
    x->m_parent = y;
}

inline void
__rb_tree_rebalance(__rb_tree_node_base* x,
                    __rb_tree_node_base*& root)
{
    x->m_color = __rb_tree_red; // the color of new node must be red
    while(x != root && x->m_parent->m_color == __rb_tree_red) {
        if(x->m_parent == x->m_parent->m_left) { // parent is the left child node of grandparent
            __rb_tree_node_base* y = x->m_parent->m_parent->m_right; // uncle
            if(y && y->m_color == __rb_tree_red) { // the color of uncle is red
                x->m_parent->m_color = __rb_tree_black;
                y->m_color = __rb_tree_black;
                x->m_parent->m_parent->m_color = __rb_tree_red;
                x = x->m_parent->m_parent;
            } else { // the color of uncle is black
                if(x == x->m_parent->m_right) {
                    x = x->m_parent;
                    __rb_tree_rotate_left(x, root);
                }
                x->m_parent->m_color = __rb_tree_black;
                x->m_parent->m_parent->m_color = __rb_tree_red;
                __rb_tree_rotate_right(x->m_parent->m_parent, root);
            }
        } else { // parent is the right child node of grandparent
            __rb_tree_node_base* y = x->m_parent->m_parent->m_left;
            if(y && y->m_color == __rb_tree_red) {
                x->m_parent->m_color = __rb_tree_black;
                y->m_color = __rb_tree_black;
                x->m_parent->m_parent->m_color = __rb_tree_red;
                x = x->m_parent->m_parent;
            } else {
                if(x == x->m_parent->m_left) {
                    x = x->m_parent;
                    __rb_tree_rotate_right(x, root);
                }
                x->m_parent->m_color = __rb_tree_black;
                x->m_parent->m_parent->m_color = __rb_tree_red;
                __rb_tree_rotate_left(x->m_parent->m_parent, root);
            }
        }
    }
    root->m_color = __rb_tree_black;
}

template <class Key, class Value, class KeyOfValue,
          class Compare, class Alloc = __SSTL_DEFAULT_ALLOC>
class rb_tree {
protected:
    typedef __rb_tree_node_base* base_ptr;
    typedef __rb_tree_node<Value> node_type;
    typedef __SSTL_ALLOC(node_type, Alloc) node_allocator_type;
    typedef __rb_tree_color_type color_type;

public:
    typedef Key         key_type;
    typedef Value       value_type;
    typedef Value*      pointer;
    typedef Value&      reference;
    typedef node_type*  link_type;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef const Value* const_pointer;
    typedef const Value& const_reference;

protected:
    link_type allocate_node()
    { return node_allocator_type::allocate(); }

    void deallocate_node(link_type p)
    { node_allocator_type::deallocate(p); }

    link_type create_node(const value_type& x)
    {
        link_type p = allocate_node();
        __SSTL_TRY {
            sstl::construct(p, x);
        }
#ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            deallocate_node(p);
        }
        return p;
#endif
    }

    link_type clone_node(link_type p)
    {
        link_type p_clone = create_node(p->m_data);
        p_clone->color = p->color;
        p_clone->m_left = 0;
        p_clone->m_right = 0;
        return p_clone;
    }

    void destroy_node(link_type p)
    {
        destroy(&p->m_data);
        deallocate_node(p);
    }

protected:
    size_type node_count;
    link_type header;
    Compare key_compare;

    link_type& root() const
    { return header->m_parent; }

    link_type& leftmost() const
    { return header->m_left; }

    link_type& rightmost() const
    { return header->m_right; }

    static link_type& left(link_type p)
    { return p->m_left; }

    static link_type& right(link_type p)
    { return p->m_right; }

    static link_type& parent(link_type p)
    { return p->m_parent; }

    static const value_type& value(link_type p)
    { return p->m_data; }

    static const key_type& key(link_type p)
    { return KeyOfValue()(value(p)); }

    static color_type& color(link_type p)
    { return p->m_color; }

    static link_type minimum(link_type p)
    { return __rb_tree_node_base::minimum(p); }

    static link_type maximum(link_type p)
    { return __rb_tree_node_base::maximum(p); }

public:
    typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
    typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

private:
    /**
     * @param   __x: the pointer to insert point
     * @param   __y: the pointer to the parent of insert point
     * @param   __val: the value which will be inserted
     */
    iterator __insert(base_ptr __cur, base_ptr __pare, const value_type& __val)
    {
        link_type cur = __cur;
        link_type pare = __pare;
        link_type tmp;

        /* When we add to the left child node:
         * 1. there's no node in rb-tree
         * 2. there's already one node with __val
         * 3. __val is larger than parent's value
         */
        if(pare == header || cur != 0 ||
           key_compare(KeyOfValue()(__val), key(pare))) {
            tmp = create_node(__val);
            left(pare) = tmp;
            if(pare == header) { /* parent is header */
                root() = tmp;
                rightmost() = tmp;
            } else if(pare == leftmost()) /* parent is the smallest node */
                leftmost() = tmp;
        } else { /* add node to the right child node */
            tmp = create_node(__val);
            right(pare) = tmp;
            if(pare == rightmost()) /* parent is the largest node */
                rightmost() = tmp;
        }
        parent(tmp) = pare;
        left(tmp) = 0;
        right(tmp) = 0;

        __rb_tree_rebalance(tmp, header->m_parent);
        ++node_count;
        return iterator(tmp);
    }

    link_type __copy(link_type x, link_type p)
    {
        link_type top = clone_node(x);
        top->m_parent = p;

        // clone right child node
        if(x->m_right)
            top->m_right = __copy(right(x), top);
        p = top;
        x = left(x);

        // clone left child node
        while(x != NULL) {
            link_type y = clone_node(x);
            p->m_left = y;
            y->m_parent = p;
            if(x->m_right)
                y->m_right = __copy(right(x), y);
            p = y;
            x = left(x);
        }
        return top;
    }

    void __erase(link_type x)
    {
        while(x != 0) {
            __erase(right(x));
            link_type y = left(x);
            destroy_node(x);
            x = y;
        }
    }

    void init() {
        header = allocate_node();
        color(header) = __rb_tree_red;
        root() = 0;
        leftmost() = header;
        rightmost() = header;
    }

    void clear() {
        if (node_count == 0)
            return;
        __erase(root());
        leftmost() = header;
        root() = 0;
        rightmost() = header;
        node_count = 0;
    }

public:
    explicit rb_tree(const Compare& comp = Compare())
     : node_count(0), key_compare(comp)
    { init(); }

    ~rb_tree()
    {
        clear();
        deallocate_node(header);
    }

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x)
    {
        if(this == &x)
            return *this;

        clear();
        node_count = 0;
        key_compare = x.key_compare;
        if(x.root() == NULL) {
            root() = NULL;
            leftmost() = NULL;
            rightmost() = NULL;
        } else {
            root() = __copy(x.root(), header);
            leftmost() = minimum(root());
            rightmost() = maximum(root());
            node_count = x.node_count;
        }
        return *this;
    }

public:
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    iterator end() { return rightmost(); }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }
    size_type max_size() const { return size_type(-1); }

public:
    /**
     * @brief   Returns an iterator pointing to the
     *          first element that is not less than value
     */
    iterator lower_bound(const key_type& __k)
    {
        link_type y = header;
        link_type x = root();
        while(x != NULL) {
            if(!key_compare(key(x), __k)) { // the value of current node is larger than __k
                y = x;
                x = left(x);
            } else
                x = right(x);
        }
        return y;
    }

    iterator upper_bound(const key_type& __k)
    {
        link_type y = header;
        link_type x = root();
        while(x != NULL) {
            if(key_compare(__k, key(x))) {
                y = x;
                x = left(x);
            } else
                x = right(x);
        }
        return y;
    }

    pair<iterator, iterator> equal_range(const key_type& __k)
    {
        return pair<iterator, iterator>(lower_bound(__k), upper_bound(__k));
    }

    difference_type count(const key_type& __k) const
    {
        pair<const_iterator, const_iterator> p = equal_range(__k);
        difference_type n = __DISTANCE(p.first, p.second);
        return n;
    }

    pair<iterator, bool> insert_unique(const value_type& val)
    {
        link_type y = header; // parent of x
        link_type x = root(); // root node
        bool comp = true;
        while(x != 0) {
            y = x;
            comp = key_compare(KeyOfValue()(val), key(x));
            x = comp ? left(x) : right(x);
        }
        iterator j = iterator(y);
        if(comp) { // insert node into left
            if(j == begin()) {
                return pair<iterator, bool>(__insert(x, y, val), true);
            } else {
                --j;
            }
        }
        if(key_compare(key(j.m_node), KeyOfValue()(val))) { // insert node into right
            return pair<iterator, bool>(__insert(x, y, val), true);
        }
        return pair<iterator, bool>(j, false); // duplicate node
    }

    iterator insert_equal(const value_type& val)
    {
        link_type y = header;
        link_type x = root();
        while(x != 0) {
            y = x;
            x = key_compare(KeyOfValue()(val), key(x)) ? left(x) : right(x);
        }
        return __insert(x, y, val);
    }

    iterator find(const Key& k)
    {
        link_type y = header();
        link_type x = root();

        while(x != 0) {
            if(!key_compare(key(x), k)) {
                y = x;
                x = left(x);
            } else
                x = right(x);
        }
        iterator j = y;
        return j == end() || key_compare(k, key(j.m_node)) ? end() : j;
    }
};

}; // sstl


#endif //SSTL_TREE_H
