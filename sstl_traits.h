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

__SSTL_SPEC_TYPE_TRAITS(char)
__SSTL_SPEC_TYPE_TRAITS(signed char)
__SSTL_SPEC_TYPE_TRAITS(unsigned char)
__SSTL_SPEC_TYPE_TRAITS(short)
__SSTL_SPEC_TYPE_TRAITS(unsigned short)
__SSTL_SPEC_TYPE_TRAITS(int)
__SSTL_SPEC_TYPE_TRAITS(unsigned int)
__SSTL_SPEC_TYPE_TRAITS(long)
__SSTL_SPEC_TYPE_TRAITS(unsigned long)
__SSTL_SPEC_TYPE_TRAITS(float)
__SSTL_SPEC_TYPE_TRAITS(double)
__SSTL_SPEC_TYPE_TRAITS(long double)

} // sstl


#endif // SSTL_TRAITS_H
