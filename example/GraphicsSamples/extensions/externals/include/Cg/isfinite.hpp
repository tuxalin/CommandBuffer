/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_isfinite_hpp__
#define __Cg_isfinite_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#undef isfinite  // g++ headers may defined this

#include <Cg/vector.hpp>

#undef isfinite  // g++ <math.h> header may have defined this

namespace Cg {

template <typename T, int N>
static inline __CGvector<bool,N> isfinite(const __CGvector<T,N> & v)
{
    __CGvector<bool,N> rv;
    for (int i=0; i<N; i++) {
        const typename __CGtype_trait<T>::realType s = v[i];
        // By IEEE 754 rule, 2*Inf equals Inf
        rv[i] = (s == s) && ((s == 0) || (s != 2*s));
    }
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<bool,N> isfinite(const __CGvector_usage<T,N,Tstore> & v)
{
    __CGvector<bool,N> rv;
    for (int i=0; i<N; i++) {
        const typename __CGtype_trait<T>::realType s = v[i];
        // By IEEE 754 rule, 2*Inf equals Inf
        rv[i] = (s == s) && ((s == 0) || (s != 2*s));
    }
    return rv;
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::boolType,1> isfinite(const T & v)
{
    const typename __CGtype_trait<T>::realType s = static_cast<T>(v);
    // By IEEE 754 rule, 2*Inf equals Inf
    __CGvector<typename __CGtype_trait<T>::boolType,1> rv((s == s) && ((s == 0) || (s != 2*s)));
    return rv;
}

} // namespace Cg

#endif // __Cg_isfinite_hpp__
