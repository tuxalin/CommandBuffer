/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_saturate_hpp__
#define __Cg_saturate_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> saturate(const __CGvector<T,N> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++) {
        if (T(1) < v[i]) {
            rv[i] = T(1);
        } else if (T(0) < v[i]) {
            rv[i] = v[i];
        } else {
            rv[i] = T(0);
        }
    }
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> saturate(const __CGvector_usage<T,N,Tstore> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++) {
        if (T(1) < v[i]) {
            rv[i] = T(1);
        } else if (T(0) < v[i]) {
            rv[i] = v[i];
        } else {
            rv[i] = T(0);
        }
    }
    return rv;
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::numericType,1> saturate(const T & v)
{
     __CGvector<typename __CGtype_trait<T>::numericType,1> rv;
    if (T(1) < v) {
        rv = T(1);
    } else if (T(0) < v) {
        rv = v;
    } else {
        rv = T(0);
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_saturate_hpp__
