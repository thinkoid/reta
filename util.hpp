// -*- mode: c++; -*-

#ifndef RETA_UTIL_HPP
#define RETA_UTIL_HPP

using namespace std;

template< typename T >
inline size_t size_cast (T c) {
    return size_t (typename make_unsigned< T >::type (c));
}

#endif // RETA_UTIL_HPP
