/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_round_hpp__
#define __Cg_round_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <cmath>  // for std::floor

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> round(const __CGvector<T,N> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::floor(v[i] + T(0.5));
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> round(const __CGvector_usage<T,N,Tstore> & v)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::floor(v[i] + T(0.5));
    return rv;
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::realType,1> round(const T & v)
{
    __CGvector<typename __CGtype_trait<T>::realType,1> rv(std::floor(static_cast<T>(v)) + T(0.5));
    return rv;
}

} // namespace Cg

#endif // __Cg_round_hpp__
