/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_frac_hpp__
#define __Cg_frac_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <cmath>  // for std::frac

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> frac(const __CGvector<T,N> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = T(v[i] - std::floor(v[i]));
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> frac(const __CGvector_usage<T,N,Tstore> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = T(v[i] - std::floor(v[i]));
    return rv;
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::realType,1> frac(const T & v)
{
    __CGvector<typename __CGtype_trait<T>::realType,1> rv(v - std::floor(static_cast<T>(v)));
    return rv;
}

} // namespace Cg

#endif // __Cg_frac_hpp__
