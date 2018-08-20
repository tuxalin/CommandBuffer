/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_cross_hpp__
#define __Cg_cross_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> cross(const __CGvector<TA,3> & a, const __CGvector<TB,3> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> a_yzx(a[1], a[2], a[0]),
                                                              a_zxy(a[2], a[0], a[1]),
                                                              b_yzx(b[1], b[2], b[0]),
                                                              b_zxy(b[2], b[0], b[1]);

    // Ideally would write "a.yzx * b.zxy - a.zxy * b.yzx" but cannot
    // assume <Cg/xyzw.hpp> has been included.
    return a_yzx * b_zxy - a_zxy * b_yzx;
}

template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> cross(const __CGvector_usage<TA,N,TAstore> & a,
                                                                              const __CGvector_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> a_yzx(a[1], a[2], a[0]),
                                                              a_zxy(a[2], a[0], a[1]),
                                                              b_yzx(b[1], b[2], b[0]),
                                                              b_zxy(b[2], b[0], b[1]);

    return a_yzx * b_zxy - a_zxy * b_yzx;
}


template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> crossv1(const __CGvector<TA,2> & a, const __CGvector<TB,2> & b)
{
    return __CGvector<typename __CGtype_trait<TA,TB>::numericType,3>(
        a[1] - b[1], b[0] - a[0], a[0]*b[1] - a[1]*b[0]);
}

template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,3> crossv1(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                const __CGvector_usage<TB,N,TBstore> & b)
{
    return __CGvector<typename __CGtype_trait<TA,TB>::numericType,3>(
        a[1] - b[1], b[0] - a[0], a[0]*b[1] - a[1]*b[0]);
}

} // namespace Cg

#endif // __Cg_cross_hpp__
