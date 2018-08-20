/* 
 * Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_upper1x2_hpp__
#define __Cg_upper1x2_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

template <typename T, int M, int N>
__CGmatrix<T,1,2> upper1x2(const __CGmatrix<T,M,N> &m)
{
    const int COLS = 1,
              ROWS = 2;
    __CGmatrix<T,COLS,ROWS> rv(0);
    for (int i=0; i<COLS; i++) {
        for (int j=0; j<ROWS; j++) {
            if (i<M && j<N) {
                rv[i][j] = m[i][j];
            } else {
                rv[i][j] = (i==j) ? T(1) : T(0);
            }
        }
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_upper1x2_hpp__
