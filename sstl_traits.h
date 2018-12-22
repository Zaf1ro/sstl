#ifndef SSTL_TRAITS_H
#define SSTL_TRAITS_H

#include <stddef.h>


namespace sstl {

/**
 * @brief   determine certain properties of the iterator
 */
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer_type         pointer_type;
    typedef typename Iterator::reference_type       reference_type;
};

/**
 * @brief   partial specialization for pointer type
 */
template <class T>
struct iterator_traits<T*> {
    typedef T           value_type;         // type of value
    typedef T*          iterator_category;  // type of sstl_iterator
    typedef ptrdiff_t   difference_type;    // type of distance between iterators
    typedef T*          pointer_type;       // type of pointer of value
    typedef T&          reference_type;     // type of reference of value
};

/**
* @brief   partial specialization for const pointer type
*/
template <class T>
struct iterator_traits<const T*> {
    typedef T           value_type;
    typedef T*          iterator_category;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer_type;
    typedef T&          reference_type;
};

/**
 * @brief   type of true or false
 */
struct __false_type {};
struct __true_type {};

/**
 * @brief   obtain type information on compile-time
 */
template <class T>
struct __type_traits {
    typedef __false_type    has_trivial_default_constructor;
    typedef __false_type    has_trivial_copy_constructor;
    typedef __false_type    has_trivial_assginment_operator;
    typedef __false_type    has_trivial_destructor;
    typedef __false_type    is_POD_type;
};

/**
 * @brief   explicit specialization for char
 */
template<> struct __type_traits<char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for signed char
 */
template<> struct __type_traits<signed char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for unsigned char
 */
template<> struct __type_traits<unsigned char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for short
 */
template<> struct __type_traits<short> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for unsigned short
 */
template<> struct __type_traits<unsigned short> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for int
 */
template<> struct __type_traits<int> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for unsigned int
 */
template<> struct __type_traits<unsigned int> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for long
 */
template<> struct __type_traits<long> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for unsigned long
 */
template<> struct __type_traits<unsigned long> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for float
 */
template<> struct __type_traits<float> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for double
 */
template<> struct __type_traits<double> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   explicit specialization for long double
 */
template<> struct __type_traits<long double> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/**
 * @brief   partial specialization for pointer
 */
template<class T>
struct __type_traits<T*> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assginment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

} // sstl


#endif // SSTL_TRAITS_H
