#ifndef SSTL_FUNCTION_H
#define SSTL_FUNCTION_H


namespace sstl {

/**
 * @brief   Base class for creating function objects
 *          with one argument
 */
template <class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

/**
 * @brief   Base class for creating function objects
 *          with two arguments
 */
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

/**
 * @brief   Function object for performing addition
 * @param   x, y: values to sum
 */
template <class T>
struct plus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const
    { return x + y; }
};

/**
 * @brief   Function object for performing subtraction
 * @param   x, y: values to subtract from one another
 */
template <class T>
struct minus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const
    { return x - y; }
};

/**
 * @brief   Function object for performing multiplication
 * @param   x, y: value to multiply
 */
template <class T>
struct multiplies: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const
    { return x * y; }
};

/**
 * @brief   Function object ot performing division
 * @param   x, y: values to divide one by other
 */
template <class T>
struct divides: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const
    { return x / y; }
};

/**
 * @brief   Function object for computing remainders of divisions
 * @param   x, y: values to divide one by another
 */
template <class T>
struct modulus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const
    { return x % y; }
};

/**
 * @brief   Function object for performing negation
 * @param   value to compute negation of
 */
template <class T>
struct negate: public unary_function<T, T> {
    T operator()(const T& x) const
    { return -x; }
};

/**
 * @brief   Function object for performing comparisons.
 *          Checks whether x is equal to y
 * @param   x, y: values to compare
 */
template <class T>
struct equal_to: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x == y; }
};

/**
 * @brief   Check if the arguments are not equal
 * @param   x, y: values to compare
 */
template <class T>
struct not_equal_to: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x != y; }
};

/**
 * @brief   Check whether the first argument is
 *          greater than the second argument
 * @param   x, y: values to compare
 */
template <class T>
struct greater: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x > y; }
};

/**
 * @brief   Check whether the first argument is
 *          less than the second argument
 * @param   x, y: values to compare
 */
template <class T>
struct less: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x < y; }
};

/**
 * @brief   Check if the first argument is
 *          greater than or equal to the second
 * @param   x, y: values to compare
 */
template <class T>
struct greater_equal: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x >= y; }
};

/**
 * @brief   Check if the first argument is
 *          less than or equal to the second
 * @param   x, y: values to compare
 */
template <class T>
struct less_equal: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x <= y; }
};

/**
 * @brief   Return the logical AND of the two arguments
 * @param   x, y: values to compute logical AND of
 */
template <class T>
struct logical_and: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x && y; }
};

/**
 * @brief   Return the logical OR of the two arguments
 * @param   x, y: 	values to compute logical OR of
 */
template <class T>
struct logical_or: public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const
    { return x || y; }
};

/**
 * @brief   Return the logical NOT of the argument
 * @param   x: value to compute logical NOT of
 */
template <class T>
struct logical_not: public unary_function<T, bool> {
    bool operator()(const T& x) const
    { return !x; }
};

/**
 * @brief   Return the argument unchanged
 * @param   x: argument to return
 */
template <class _Tp>
struct identity
{
    _Tp operator()(_Tp x) const { return x; }
};

/**
 * @brief   Return the pair's first element
 * @param   x: the pair object
 */
template <class Pair>
struct select1st: public unary_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type&
    operator()(const Pair& x) const
    { return x.first; }
};

/**
 * @brief   Return the pair's second element
 * @param   x: the pair object
 */
template <class Pair>
struct select2nd: public unary_function<Pair, typename Pair::second_type>
{
    const typename Pair::second_type&
    operator()(const Pair& x) const
    { return x.second; }
};

/**
 * @brief   Return the first argument
 */
template <class Arg1, class Arg2>
struct project1st: public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& x, const Arg2&) const
    { return x; }
};

/**
 * @brief   Return the second argument
 */
template <class Arg1, class Arg2>
struct project2nd: public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1&, const Arg2& y) const
    { return y; }
};

} // sstl


#endif // SSTL_FUNCTION_H
