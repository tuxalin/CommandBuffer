/* 
 * Copyright 2007 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_intBitsToFloat_hpp__
#define __Cg_intBitsToFloat_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<float,N> intBitsToFloat(const __CGvector<T,N> & iv)
{
    __CGvector<float,N> v = __CGvector<float,N>(iv);
    __CGvector<float,N> rv;
    for (int i=0; i<N; i++) {
        union { float f; int i; } combo;
        combo.i = int(v[i]);
        rv[i] = combo.f;
    }
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<float,N> intBitsToFloat(const __CGvector_usage<T,N,Tstore> & iv)
{
    __CGvector<float,N> v = __CGvector<float,N>(iv);
    __CGvector<float,N> rv;
    for (int i=0; i<N; i++) {
        union { float f; int i; } combo;
        combo.i = int(v[i]);
        rv[i] = combo.f;
    }
    return rv;
}
static inline __CGvector<float,1> intBitsToFloat(const int & v)
{
    union { float f; int i; } combo;
    combo.i = int(v);
    __CGvector<float,1> rv(combo.f);
    return rv;
}

} // namespace Cg

#endif // __Cg_intBitsToFloat_hpp__
