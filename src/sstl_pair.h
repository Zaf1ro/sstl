#ifndef SSTL_PAIR_H
#define SSTL_PAIR_H


namespace sstl {

template <class _T1, class _T2>
struct pair {
    typedef _T1 first_type;
    typedef _T2 second_type;

    _T1 first;
    _T2 second;

    pair(): first(_T1()), second(_T2()) {}

    pair(const _T1& __a, const _T2& __b)
            : first(__a), second(__b) {}

    template <class _U1, class _U2>
    pair(const pair<_U1, _U2>& __p)
            : first(__p.first), second(__p.second) {}

    inline bool operator==(const pair<_T1, _T2>& __x,
                           const pair<_T1, _T2>& __y)
    { return __x.first == __y.first && __x.second == __y.second; }

    inline bool operator!=(const pair<_T1, _T2>& __x,
                           const pair<_T1, _T2>& __y)
    { return !(__x == __y); }



    inline bool operator<(const pair<_T1, _T2>& __x,
                          const pair<_T1, _T2>& __y)
    { return __x.first < __y.first ||
             !(__y.first < __x.first) && __x.second < __y.second; }

    inline bool operator>(const pair<_T1, _T2>& __x,
                          const pair<_T1, _T2>& __y)
    { return __y < __x; }

    inline bool operator<=(const pair<_T1, _T2>& __x,
                           const pair<_T1, _T2>& __y)
    { return !(__x > __y); }

    inline bool operator>=(const pair<_T1, _T2>& __x,
                           const pair<_T1, _T2>& __y)
    { return !(__x < __y); }
};

template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2 __y)
{ return pair<_T1, _T2>(__x, __y); }

} // sstl


#endif // SSTL_PAIR_H
