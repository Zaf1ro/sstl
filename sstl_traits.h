#ifndef SSTL_TRAITS_H
#define SSTL_TRAITS_H

#include <stddef.h>


namespace sstl {

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
 * @brief   partial specialization for pointer
 */
template<class T>
struct __type_traits<T*> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

/* explicit template specialization for POD */
#define __SSTL_SPEC_TYPE_TRAITS(_Tp)                            \
template<> struct __type_traits<_Tp>                            \
{                                                               \
    typedef __true_type     has_trivial_default_constructor;    \
    typedef __true_type     has_trivial_copy_constructor;       \
    typedef __true_type     has_trivial_assginment_operator;    \
    typedef __true_type     has_trivial_destructor;             \
    typedef __true_type     is_POD_type;                        \
};

template<> struct __type_traits<short> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<unsigned short> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<int> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<unsigned int> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<signed char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};



template<> struct __type_traits<unsigned char> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<long> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<unsigned long> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<float> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<double> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

template<> struct __type_traits<long double> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
};

} // sstl


#endif // SSTL_TRAITS_H
