#ifndef SSTL_SSTL_TREE_H
#define SSTL_SSTL_TREE_H

#include "sstl_config.h"
#include "sstl_iterator.h"


sstl {

typedef bool __rb_tree_color_type;

const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base {
    typedef __rb_tree_color_type color_type;
    typedef __rb_tree_node_base* base_ptr;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x)
    {
        while(x->left != NULL)
            x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x)
    {
        while(x->right != NULL)
            x = x->right;
        return x;
    }
};

template <class _Tp>
struct __rb_tree_node: public __rb_tree_node_base
{
    typedef __rb_tree_node<_Tp>* node_ptr;
    _Tp value;
};

struct __rb_tree_iterator_base
{
    typedef __rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag iterator_tag;
    typedef ptrdiff_t difference_type;

    base_ptr m_node;

    void increment()
    {
        if(node->right != NULL) {
            m_node = m_node->right;
            while(m_node->left != NULL)
                m_node = m_node.left;
        } else {
            base_ptr p = m_node->parent;
            while(m_node == p->right) {
                m_node = p;
                p = p->parent;
            }
            if(m_node->right != p)
                m_node = p;
        }
    }

    void decrement()
    {
        if(m_node->color == __rb_tree_red &&
           m_node->parent->parent == m_node)
            m_node = m_node->right;
        else if(m_node->left != NULL) {
            base_ptr r = m_node->left;
            while(r->right != NULL)
                r = r->right;
            m_node = r;
        } else {
            base_ptr p = m_node->parent;
            while(m_node == p->left) {
                m_node = p;
                p = m_node->parent;
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
    typedef __rb_tree_iterator<_Tp, _Tp&, _Tp*> iterator
    typedef __rb_tree_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
    typedef __rb_tree_iterator<_Tp, _Ref, _Ptr> self;
    typedef __rb_tree_node<_Tp>* node_ptr;

    __rb_tree_iterator() {}
    __rb_tree_iterator(node_ptr x) { m_node = x; }
    __rb_tree_iterator(const iterator& it) { m_node = it.m_node; }

    reference operator*() const
    {
        return (node_ptr)m_node->value;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    self& operator++()
    {
        increment();
        return *this;
    }

    self& operator++(int)
    {
        self tmp = *this;
        increment();
        return tmp;
    }

    self& operator--()
    {
        decrement();
        return *this;
    }

    self& operator--(int)
    {
        self tmp = *this;
        decrement();
        return tmp;
    }
};


}; // sstl


#endif //SSTL_SSTL_TREE_H
