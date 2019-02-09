#ifndef SSTL_VECTOR_H
#define SSTL_VECTOR_H

#include "sstl_alloc.h"
#include "sstl_uninitialized.h"
#include "sstl_algobase.h"


namespace sstl {

template <class _Tp, class _Alloc>
class _vector_base {
public:
    typedef _Alloc                      allocator_type;
    typedef __SSTL_ALLOC(_Tp, _Alloc)   m_data_allocator;

    allocator_type get_allocator() const
    { return allocator_type(); }

    _vector_base(const allocator_type&)
     : m_start(0), m_finish(0), m_end_of_storage(0) {}

    _vector_base(size_t __n, const allocator_type&)
     : m_start(0), m_finish(0), m_end_of_storage(0)
    {
        m_start = _allocate(__n);
        m_finish = m_start;
        m_end_of_storage = m_start + __n;
    }

    ~_vector_base()
    { _deallocate(m_start, m_end_of_storage - m_start); }

protected:
    _Tp* _allocate(size_t __n) const
    { return m_data_allocator::allocate(__n); }

    void _deallocate(_Tp* __p, size_t)
    { m_data_allocator::deallocate(m_start, m_end_of_storage - m_start); }

    template <class ForwardIter>
    _Tp* _allocate_and_copy(size_t __n, ForwardIter __first, ForwardIter __last)
    {
        _Tp* res = _allocate(__n);
        __SSTL_TRY {
            sstl::uninitialized_copy(__first, __last, res);
        }
    #ifdef __SSTL_USE_EXCEPTIONS
        catch(...) {
            _deallocate(res, __n);
            throw;
        }
    #endif
        return res;
    }

protected:
    _Tp* m_start;
    _Tp* m_finish;
    _Tp* m_end_of_storage;
};

template<class _Tp, class _Alloc = __SSTL_DEFAULT_ALLOC>
class vector: protected _vector_base<_Tp, _Alloc> {
private:
    typedef _vector_base<_Tp, _Alloc> _Base;
public:
    typedef _Tp         value_type;
    typedef _Tp*        pointer;
    typedef const _Tp*  const_pointer;
    typedef _Tp*        iterator;
    typedef const _Tp*  const_iterator;
    typedef _Tp&        reference;
    typedef const _Tp&  const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    typedef typename _Base::allocator_type allocator_type;

protected:
    using _Base::m_start;
    using _Base::m_finish;
    using _Base::m_end_of_storage;
    using _Base::_allocate;
    using _Base::_deallocate;
    using _Base::get_allocator;
    using _Base::_allocate_and_copy;

    /**
     * @brief   Make room for new elements and move old element to new place
     * @param   __pos: iterator before which the content will be inserted
     * @param   __val: element value to insert
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
            iterator new_start = _allocate(new_size);
            iterator new_finish = new_start;

            __SSTL_TRY
            {
                // copy all elements before insert position into new room
                new_finish = uninitialized_copy(m_start, __pos, new_start);
                construct(new_finish, __val);
                ++new_finish;
                new_finish = uninitialized_copy(__pos, m_finish, new_finish);
            }
#ifdef __SSTL_USE_EXCEPTIONS
            catch(...) {
                destroy(new_start, new_finish);
                _deallocate(new_start, new_size);
                throw;
            }
#endif
            destroy(begin(), end());
            _deallocate(m_start, m_end_of_storage - m_start);
            m_start = new_start;
            m_finish = new_finish;
            m_end_of_storage = new_start + new_size;
        }
     }

    /**
     * @brief   allocate memory and initialize all elements
     * @param   __n: size of memory to be allocated
     * @param   __val: value used for initialization of element
     */
    template <class Integer>
    void _vector_aux(Integer __n, Integer __val, __true_type)
    {
        m_start = _allocate(__n);
        m_finish = uninitialized_fill_n(m_start, __n, __val);
        m_end_of_storage = m_finish;
    }

    template <class InputIter>
    void _vector_aux(InputIter __first, InputIter __last,
                     __false_type)
    {
        for( ; __first != __last; ++__first)
            push_back(*__first);
    }

public:
    vector<_Tp, _Alloc>&
    operator=(const vector<_Tp, _Alloc>& __x)
    {
        if(*this == __x)
            return *this;

        const size_type len = __x.size();
        if(len > capacity()) { // expand the space of container
            iterator tmp = _allocate_and_copy(len, __x.begin(), __x.end());
            sstl::destroy(m_start, m_finish);
            _deallocate(m_start, m_end_of_storage - m_start);
            m_start = tmp;
            m_end_of_storage = m_start + len;
        } else if(size() >= len) { // have to destroy some old elements
            iterator tmp = sstl::copy(__x.begin(), __x.end(), begin());
            destroy(tmp, m_finish);
        } else {
            sstl::copy(__x.begin(), __x.begin()+size(), m_start);
            sstl::uninitialized_copy(__x.begin() + size(), __x.end(), m_finish);
        }
        m_finish = m_start + len;
        return *this;
    }

    /**
     * @brief   Returns the allocator associated with the container
     */
    allocator_type get_allocator()
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
    size_type size() const { return m_finish - m_start; }

    /**
     * @brief   Return the capacity of allocated storage
     */
    size_type capacity() const { return m_end_of_storage - m_start; }

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
    explicit vector(const allocator_type& __alloc = allocator_type())
     : _Base(__alloc) {}

    explicit vector(size_type __n)
     : _Base(__n, allocator_type())
    { uninitialized_fill_n(m_start, __n, value_type()); }

    vector(size_type __n, const value_type& __val,
           const allocator_type& __alloc = allocator_type())
     : _Base(__n, __alloc)
    { uninitialized_fill_n(m_start, __n, __val); }

    explicit vector(const vector<value_type, allocator_type>& __x)
     : _Base(__x.size(), __x.get_allocator())
    { m_finish = uninitialized_copy(__x.begin(), __x.end(), value_type()); }

    template <class _InputIter>
    vector(_InputIter __first, _InputIter __last,
           const allocator_type& __alloc = allocator_type())
     : _Base(__alloc)
    {
        typedef typename __is_integer<_InputIter>::is_Integral _Is_Integral;
        _vector_aux(__first, __last, _Is_Integral());
    }

    /**
     * @brief    release memory of each element
     */
    virtual ~vector() { clear(); }

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
     * @param   __val: element value to push
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
     * @param   __pos: iterator before the content which will be inserted
     * @param   __val: element value to insert
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
     * @param   __pos: same as insert(position, value)
     * @param   __n: number of elements to insert
     * @param   __val: element value to insert
     */
    void insert(iterator __pos, size_type __n,
                const value_type& __val)
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
            iterator new_start = _allocate(new_size);
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
                _deallocate(new_start, new_size);
                throw;
            }
        #endif
            destroy(m_start, m_finish);
            _deallocate(m_start, m_end_of_storage - m_start);
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
     * @param   __pos: iterator to the element which will be erased
     */
    iterator erase(iterator __pos)
    {
        if(__pos + 1 != m_finish) { // move forward
            sstl::copy(__pos + 1, m_finish, __pos);
        }
        sstl::destroy(--m_finish);
        return __pos;
    }

    /**
     * @brief   Remove the elements in the range [__first, __last)
     * @param   __first, __last: range of elements to remove
     */
    iterator erase(iterator __first, iterator __last) {
        iterator tmp = copy(__last, m_finish, __first); // move forward
        destroy(tmp, m_finish);
        m_finish = m_finish - (__last - __first);
        return __first;
    }

    /**
     * @brief   Erase all elements from the container
     */
    void clear() { erase(begin(), end()); }

    /**
     * @brief   Resize the container to contain certain number of elements
     * @param   __n_size: new size of the container
     */
    void resize(size_type __n_size)
    {
        if(__n_size < size())
            erase(m_start + __n_size, end());
        else
            insert(m_start + __n_size, value_type());
    }

    /**
     * @brief   Increase the capacity of the container to
     *          the specified size
     * @param   __n: new capacity of the container
     */
    void reserve(size_type __n)
    {
        if(capacity() < __n) {
            const size_type old_size = size();
            iterator tmp = _allocate_and_copy(__n, m_start, m_finish);
            destroy(m_start, m_finish);
            _deallocate(m_start, m_end_of_storage - m_start);
            m_start = tmp;
            m_finish = tmp + old_size;
            m_end_of_storage = tmp + __n;
        }
    }

    /**
     * @brief   Exchange the content of container
     * @param   __x: container to exchange the contents with
     */
    void swap(vector<_Tp, _Alloc>& __x)
    {
        sstl::swap(m_start, __x.m_start);
        sstl::swap(m_finish, __x.m_finish);
        sstl::swap(m_end_of_storage, m_end_of_storage);
    }
};

} // sstl


#endif // SSTL_VECTOR_H
