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

template<class T>
struct __type_traits<T*> {
    typedef __true_type     has_trivial_default_constructor;
    typedef __true_type     has_trivial_copy_constructor;
    typedef __true_type     has_trivial_assignment_operator;
    typedef __true_type     has_trivial_destructor;
    typedef __true_type     is_POD_type;
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


/**
 * @brief   Determine whether the passed value is an integer
 */
template <class T>
struct __is_integer {
    typedef __false_type is_Integral;
};

template <>
struct __is_integer<bool> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<char> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<signed char> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<unsigned char> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<short> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<unsigned short> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<int> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<unsigned int> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<long> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<unsigned long> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<long long> {
    typedef __true_type is_Integral;
};

template <>
struct __is_integer<unsigned long long> {
    typedef __true_type is_Integral;
};


} // sstl


#endif // SSTL_TRAITS_H
