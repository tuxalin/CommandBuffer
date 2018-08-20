/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_transpose_hpp__
#define __Cg_transpose_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

template <typename T, int M, int N>
static inline __CGmatrix<T,N,M> transpose(const __CGmatrix<T,M,N> & m)
{
    __CGmatrix<T,N,M> rv;
    for (int i=0; i<N; i++) {
        for (int j=0; j<M; j++) {
            rv[i][j] = m[j][i];
        }
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_transpose_hpp__
