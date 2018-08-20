/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_all_hpp__
#define __Cg_all_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <int N>
static inline bool1 all(const __CGvector<bool,N> & v)
{
    bool1 rv = v[0];
    for (int i=1; i<N; i++)
        rv = rv && v[i];
    return rv;
}

template <int N, typename Tstore>
static inline bool1 all(const __CGvector_usage<bool,N,Tstore> & v)
{
    bool1 rv = v[0];
    for (int i=1; i<N; i++)
        rv = rv && v[i];
    return rv;
}

} // namespace Cg

#endif // __Cg_all_hpp__
