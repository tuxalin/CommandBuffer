/* 
 * Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_frexp_hpp__
#define __Cg_frexp_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <math.h>  // for ::frexp

namespace Cg {

template <typename T, int N>
static inline __CGvector<T,N> frexp(__CGvector<T,N> x, __CGvector<T,N>& expOut)
{
    __CGvector<T,N> rv;

    for (int i=0; i<N; i++) {
        int expVal;
        rv[i] = T(::frexp(x[i], &expVal));
        expOut[i] = T(expVal);
    }
    return rv;
}
template <typename T, int N, typename Tstore>
static inline __CGvector<T,N> frexp(__CGvector<T,N> x, __CGvector_usage<T,N,Tstore>& expOut)
{
    __CGvector<T,N> rv;

    for (int i=0; i<N; i++) {
        int expVal;
        rv[i] = T(::frexp(x[i], &expVal));
        expOut[i] = T(expVal);
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_frexp_hpp__
