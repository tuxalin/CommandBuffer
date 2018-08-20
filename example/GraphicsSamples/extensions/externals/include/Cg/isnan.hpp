/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_isnan_hpp__
#define __Cg_isnan_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#undef isnan  // g++ <math.h> header may have defined this

namespace Cg {

template <typename T, int N>
static inline __CGvector<bool,N> isnan(const __CGvector<T,N> & v)
{
    __CGvector<bool,N> rv;
    for (int i=0; i<N; i++) {
        const typename __CGtype_trait<T>::realType s = v[i];
        // By IEEE 754 rule, NaN is not equal to NaN
        rv[i] = s != s;
    }
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<bool,N> isnan(const __CGvector_usage<T,N,Tstore> & v)
{
    __CGvector<bool,N> rv;
    for (int i=0; i<N; i++) {
        const typename __CGtype_trait<T>::realType s = v[i];
        // By IEEE 754 rule, NaN is not equal to NaN
        rv[i] = s != s;
    }
    return rv;
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::boolType,1> isnan(const T & v)
{
    const typename __CGtype_trait<T>::realType s = static_cast<T>(v);
    // By IEEE 754 rule, NaN is not equal to NaN
    __CGvector<typename __CGtype_trait<T>::boolType,1> rv(s != s);
    return rv;
}

} // namespace Cg

#endif // __Cg_isnan_hpp__
