#ifndef SSTL_VECTOR_H
#define SSTL_VECTOR_H

#include "sstl_alloc.h"
#include "sstl_uninitialized.h"


namespace sstl {

template <class T, class Alloc>
class _vector_base {
public:
    typedef __SSTL_ALLOC(T, Alloc)  allocator_type;

    allocator_type get_allocator() const
    { return m_allocator; }

    T* allocate(size_t n)
    { return allocator_type::allocate(n); }

    void deallocate()
    {
        if( m_start )
            allocator_type::deallocate(m_start, m_end_of_storage - m_start);
    }

    explicit _vector_base(const allocator_type& __a)
     : m_allocator(__a), m_start(0), m_finish(0), m_end_of_storage(0) {}

    _vector_base(size_t __n, const allocator_type& __a)
     : m_allocator(__a)
    {
        m_start = allocate(__n);
        m_finish = m_start;
        m_end_of_storage = m_start + __n;
    }

    ~_vector_base()
    { deallocate(); }

protected:
    T* m_start;
    T* m_finish;
    T* m_end_of_storage;
    allocator_type m_allocator;
};

template<class T, class Alloc = __SSTL_DEFAULT_ALLOC>
class vector: protected _vector_base<T, Alloc> {
private:
    typedef _vector_base<T, Alloc> _Base;
public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

    typedef typename _Base::allocator_type allocator_type;

protected:
    using _Base::m_start;
    using _Base::m_finish;
    using _Base::m_end_of_storage;
    using _Base::allocate;
    using _Base::deallocate;
    using _Base::get_allocator;

    /**
     * @brief   Make room for new elements and move old element to new place
     * @param   position: iterator before which the content will be inserted
     * @param   value: element value to insert
     */
    void _expand_and_insert(iterator __pos, const value_type& __val)
    {
        if (m_finish != m_end_of_storage) { // enough space to insert
            construct(m_finish, *(m_finish - 1));
            ++m_finish;
            copy_backward(__pos, m_finish - 2, m_finish - 1); // move all elements one step forward
            value_type x_copy = __val; // call copy constructor
            *__pos = x_copy;
        }
        else // expand new room for vector elements
        {
            const size_type old_size = size();
            const size_type new_size = old_size > 0 ? 2 * old_size : 1;
            iterator new_start = allocate(new_size);
            iterator new_finish = new_start;

            __SSTL_TRY
            {
                // copy all elements before insert position into new room
                new_finish = uninitialized_copy(m_start, __pos, new_start);
                construct(new_finish, __val);
                ++new_finish;
                new_finish = uninitialized_copy(__pos, m_finish, new_finish);
            }
#ifdef __SSTL_USE_EXCEPTIONS // roll back
            catch(...) {
                destroy(new_start, new_finish);
                allocator_type::deallocate(new_start, new_size);
                throw;
            }
#endif
            destroy(begin(), end());
            deallocate();
            m_start = new_start;
            m_finish = new_finish;
            m_end_of_storage = new_start + new_size;
        }
     }

    /**
     * @brief   allocate memory and initialize all elements
     * @param   n: size of memory to be allocated
     * @param   x: value used for initialization of element
     */
    void _fill_initialize(size_type n, const value_type& x)
    {
        m_start = allocate(n);
        sstl::uninitialized_fill_n(m_start, n, x);
        m_finish = m_start + n;
        m_end_of_storage = m_finish;
    }

    template <class Integer>
    void _vector_aux(Integer first, Integer last, __true_type)
    {
        _fill_initialize(first, last);
    }

    template <class InputIter>
    void _vector_aux(InputIter first, InputIter last, __false_type)
    {
        for( ; first != last; ++first) {
            push_back(*first);
        }
    }

public:
    /**
     * @brief   Returns the allocator associated with the container
     */
     allocator_type get_allocator() const
     { return _Base::get_allocator(); }

    /**
     * @brief   Returns an iterator to the beginning of the given container
     */
    iterator begin() { return m_start; }
    const_iterator begin() const { return m_start; }

    /**
     * @brief   Returns an iterator to the end of the given container
     */
    iterator end() { return m_finish; }
    const_iterator end() const { return m_finish; }

    /**
     * @brief   Return the number of elements
     */
    size_type size() { return m_finish - m_start; }

    /**
     * @brief   Return the capacity of allocated storage
     */
    size_type capacity() { return m_end_of_storage - m_start; }

    /**
     * @brief   Check if the container has no elements
     */
    bool empty() const { return m_start == m_finish; }

    /**
     * @brief   Return the reference to the element at specified position
     */
    reference operator[](size_type __n)
    { return *(begin() + __n); }

    const_reference operator[](size_type __n) const
    { return *(begin() + __n); }

    reference at(size_type __n)
    { return (*this)[__n]; }

    const_reference at(size_type __n) const
    { return (*this)[__n]; }

    /**
     * @brief   Constructor
     */
    explicit vector(const allocator_type& __a = allocator_type())
     : _Base(__a) {}

    vector(size_type __n, const_reference __value, const allocator_type& __a = allocator_type())
     : _Base(__n, __a)
    { __fill_initialize(__n, __value); }

    explicit vector(const vector<value_type, allocator_type>& __x)
     : _Base(__x.size(), __x.get_allocator())
    { m_finish = uninitialized_copy(__x.begin(), __x.end(), value_type()); }

    explicit vector(size_type __n): _Base(__n, allocator_type())
    { __fill_initialize(__n, value_type()); }

    template <class InputIter>
    vector(InputIter __first, InputIter __last,
            const allocator_type& __a = allocator_type())
            : _Base(__a)
    {
        typedef typename __is_integer<InputIter>::is_Integral _Is_Integral;
        _vector_aux(__first, __last, _Is_Integral());
    }

    /**
     * @brief    release memory of each element
     */
    ~vector() { destroy(m_start, m_finish); }

    /**
     * @brief   Return the first element
     */
    reference front() { return *m_start; }

    /**
     * @brief   Return the last element
     */
    reference back() { return *(m_finish - 1); }

    /**
     * @brief   Add the element to the end
     */
    void push_back(const value_type& __val)
    {
        if(m_finish != m_end_of_storage) {
            construct(m_finish, __val);
            ++m_finish;
        }
        else
            _expand_and_insert(m_finish, __val);
    }

    /**
     * @brief   Insert element at the specified position
     * @param   position: iterator before the content which will be inserted
     * @param   value: element value to insert
     */
    void insert(iterator __pos, const value_type& __val)
    {
        size_type n = __pos - m_start;
        if(m_finish != m_end_of_storage && __pos == m_finish) {
            construct(__pos, __val);
            ++m_finish;
        }
        else
            _expand_and_insert(__pos, __val);
    }

    void insert(iterator __pos) { insert(__pos, value_type()); }

    /**
     * @brief   Insert elements at the specified position from other container
     * @param   position: same as insert(position, value)
     * @param   n: number of elements to insert
     * @param   value: element value to insert
     */
    void insert(iterator __pos, size_type __n, const value_type& __val)
    {
        if( __n <= 0 )
            return;

        if(m_end_of_storage - m_finish >= __n) {
            value_type copy = __val;
            const size_type elems_after = m_finish - __pos;
            iterator old_finish = m_finish;
            if (elems_after > __n) {
                sstl::uninitialized_copy(m_finish - __n, m_finish, m_finish);
                m_finish += __n;
                sstl::copy_backward(__pos, old_finish - __n, old_finish);
                sstl::fill(__pos, __pos + __n, copy);
            }
            else
            {
                sstl::uninitialized_fill_n(m_finish, __n - elems_after, copy);
                m_finish += __n - elems_after;
                sstl::uninitialized_copy(__pos, old_finish, m_finish);
                m_finish += elems_after;
                sstl::fill(__pos, old_finish, copy);
            }
        }
        else
        {
            const size_type old_size = size();
            const size_type new_size = old_size + max(old_size, __n);
            iterator new_start = allocate(new_size);
            iterator new_finish = new_start;
            __SSTL_TRY
            {
                // copy old elements before insert position to new room
                new_finish = sstl::uninitialized_copy(m_start, __pos, new_start);
                // fill with new values
                new_finish = sstl::uninitialized_fill_n(new_finish, __n, __val);
                // copy old elements after insert position to new room
                new_finish = sstl::uninitialized_copy(__pos, m_finish, new_finish);
            }
        #ifdef __SSTL_USE_EXCEPTIONS
            catch(...)
            {
                destroy(new_start, new_finish);
                allocator_type::deallocate(new_start, new_size);
                throw;
            }
        #endif
            destroy(m_start, m_finish);
            deallocate();
            m_start = new_start;
            m_finish = new_finish;
            m_end_of_storage = new_start + new_size;
        }
    }

    /**
     * @brief   Remove the last element
     */
    void pop_back()
    {
        if(m_finish != m_start) {
            destroy(--m_finish);
        }
    }

    /**
     * @brief   Erase the element at the specified position
     */
    iterator erase(iterator __pos)
    {
        if(__pos + 1 != m_finish) {
            sstl::copy(__pos + 1, m_finish, __pos);
        }
        destroy(--m_finish);
        return __pos;
    }

    /**
     * @brief   Resize the container to contain certain number of elements
     * @param   new_size: new size of the container
     */
    void resize(size_type __new_size)
    {
        if(__new_size < size())
            erase(m_start + __new_size, end());
        else
            insert(m_start + __new_size, value_type());
    }
};

} // sstl


#endif // SSTL_VECTOR_H
