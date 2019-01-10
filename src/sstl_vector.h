#ifndef SSTL_VECTOR_H
#define SSTL_VECTOR_H

#include "sstl_alloc.h"
#include "sstl_uninitialized.h"


namespace sstl {

template <class T, class Alloc>
class _vector_base {
public:
    typedef Alloc           allocator_type;

    allocator_type _get_allocator() const { return m_allocator; }

    explicit _vector_base(const allocator_type& _a)
     : m_allocator(_a), m_start(0), m_finish(0), m_end_of_storage(0) {}

    _vector_base(size_t n, const allocator_type& _a)
     : m_allocator(_a)
    {
        m_start = _allocate(n);
        m_finish = m_start;
        m_end_of_storage = m_start + n;
    }

    ~_vector_base()
    {
        _deallocate();
    }

protected:
    T* m_start;
    T* m_finish;
    T* m_end_of_storage;
    allocator_type m_allocator;

    T* _allocate(size_t n)
    {
        return allocator_type::allocate(n);
    }

    void _deallocate()
    {
        if( m_start )
            allocator_type::deallocate(m_start, m_end_of_storage - m_start);
    }
};

template<class T, class Alloc = __SSTL_DEFAULT_ALLOC(T)>
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
    using _Base::_allocate;
    using _Base::_deallocate;

    /**
     * @brief   Make room for new elements and move old element to new place
     * @param   position: iterator before which the content will be inserted
     * @param   value: element value to insert
     */
    void insert_aux(iterator position, const value_type& x)
    {
        if (m_finish != m_end_of_storage) { // enough space to insert
            construct(m_finish, *(m_finish - 1));
            ++m_finish;
            copy_backward(position, m_finish - 2, m_finish - 1); // move all elements one step forward
            value_type x_copy = x; // call copy constructor
            *position = x_copy;
        }
        else // expand new room for vector elements
        {
            const size_type old_size = size();
            const size_type new_size = old_size > 0 ? 2 * old_size : 1;
            iterator new_start = _allocate(new_size);
            iterator new_finish = new_start;

            __SSTL_TRY
            {
                // copy all elements before insert position into new room
                new_finish = uninitialized_copy(m_start, position, new_start);
                construct(new_finish, x);
                ++new_finish;
                new_finish = uninitialized_copy(position, m_finish, new_finish);
            }
        #ifdef __SSTL_USE_EXCEPTIONS // roll back
            catch(...) {
                destroy(new_start, new_finish);
                allocator_type::deallocate(new_start, new_size);
                throw;
            }
        #endif
            destroy(begin(), end());
            _deallocate();
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
    void fill_initialize(size_type n, const value_type& x)
    {
        m_start = allocate_and_fill(n, x);
        m_finish = m_start + n;
        m_end_of_storage = m_finish;
    }

    /**
     * @brief   same as fill_initialize function
     */
    iterator allocate_and_fill(size_type n, const value_type& x)
    {
        iterator result = _allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }

public:
    /**
     * @brief   Returns the allocator associated with the container
     */
     allocator_type get_allocator() const {
         return _Base::_get_allocator();
     }

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
    reference operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }
    reference at(size_type n) { return (*this)[n]; }
    const_reference at(size_type n) const { return (*this)[n]; }

    /**
     * @brief   Constructor
     */
    explicit vector(const allocator_type& _a = allocator_type())
     : _Base(_a) {}

    vector(size_type n, const_reference value, const allocator_type& _a = allocator_type())
     : _Base(n, _a)
    { fill_initialize(n, value); }

    explicit vector(const vector<value_type, allocator_type>& x)
     : _Base(x.size(), x.get_allocator())
    { m_finish = uninitialized_copy(x.begin(), x.end(), value_type()); }

    explicit vector(size_type n): _Base(n, allocator_type())
    { fill_initialize(n, value_type()); }

    template <class InputIter>
    vector(InputIter first, InputIter last,
            const allocator_type& _a = allocator_type()): _Base(_a)
    {
        typedef typename __is_integer<InputIter>::is_Integral _Is_Integral;
        __vector_aux(first, last, _Is_Integral());
    }

    template <class Integer>
    void __vector_aux(Integer first, Integer last, __true_type)
    {
        fill_initialize(first, last);
    }

    template <class InputIter>
    void __vector_aux(InputIter first, InputIter last)
    {
        for( ; first != last; ++first) {
            push_back(*first);
        }
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
    void push_back(const value_type& value)
    {
        if(m_finish != m_end_of_storage) {
            construct(m_finish, value);
            ++m_finish;
        }
        else
            insert_aux(m_finish, value);
    }

    /**
     * @brief   Insert element at the specified position
     * @param   position: iterator before the content which will be inserted
     * @param   value: element value to insert
     */
    void insert(iterator position, const value_type& value)
    {
        size_type n = position - m_start;
        if(m_finish != m_end_of_storage && position != m_finish) {
            construct(position, value);
            ++m_finish;
        }
        else
            insert_aux(position, value);
    }

    void insert(iterator position) { insert(position, value_type()); }

    /**
     * @brief   Insert elements at the specified position from other container
     * @param   position: same as insert(position, value)
     * @param   n: number of elements to insert
     * @param   value: element value to insert
     */
    void insert(iterator position, size_type n, const value_type& value)
    {
        if( n <= 0 )
            return;

        if(m_end_of_storage - m_finish >= n) {
            value_type copy = value;
            const size_type elems_after = m_finish - position;
            iterator old_finish = m_finish;
            if (elems_after > n) {
                sstl::uninitialized_copy(m_finish - n, m_finish, m_finish);
                m_finish += n;
                sstl::copy_backward(position, old_finish - n, old_finish);
                sstl::fill(position, position + n, copy);
            }
            else
            {
                sstl::uninitialized_fill_n(m_finish, n - elems_after, copy);
                m_finish += n - elems_after;
                sstl::uninitialized_copy(position, old_finish, m_finish);
                m_finish += elems_after;
                sstl::fill(position, old_finish, copy);
            }
        }
        else
        {
            const size_type old_size = size();
            const size_type new_size = old_size + max(old_size, n);
            iterator new_start = _allocate(new_size);
            iterator new_finish = new_start;
            __SSTL_TRY
            {
                // copy old elements before insert position to new room
                new_finish = sstl::uninitialized_copy(m_start, position, new_start);
                // fill with new values
                new_finish = sstl::uninitialized_fill_n(new_finish, n, value);
                // copy old elements after insert position to new room
                new_finish = sstl::uninitialized_copy(position, m_finish, new_finish);
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
            _deallocate();
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
    iterator erase(iterator position)
    {
        if(position + 1 != m_finish) {
            sstl::copy(position + 1, m_finish, position);
        }
        destroy(--m_finish);
        return position;
    }

    /**
     * @brief   Resize the container to contain certain number of elements
     * @param   new_size: new size of the container
     */
    void resize(size_type new_size)
    {
        if(new_size < size())
            erase(m_start + new_size, end());
        else
            insert(m_start + new_size, value_type());
    }
};

} // sstl


#endif // SSTL_VECTOR_H
