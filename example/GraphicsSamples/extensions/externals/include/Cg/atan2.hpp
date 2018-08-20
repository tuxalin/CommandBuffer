/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_atan2_hpp__
#define __Cg_atan2_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <cmath> // for std::atan2

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> atan2(const __CGvector<T,N> & a, const __CGvector<T,N> & b)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(a[i], b[i]);
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> atan2(const __CGvector_usage<TA,N,TAstore> & a,
                                                                           const __CGvector_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(a[i], b[i]);
    return rv;
}

template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> atan2(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                           const __CGvector_usage<TB,1,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(a[i], b[0]);
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> atan2(const __CGvector_usage<TA,1,TAstore> & a,
                                                                           const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(a[0], b[i]);
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> atan2(const __CGvector_usage<TA,N,TAstore> & a,
                                                                           const TB & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(a[i], static_cast<TB>(b));
    return rv;
}
template <typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> atan2(const TA & a,
                                                                           const __CGvector_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::atan2(static_cast<TA>(a), b[i]);
    return rv;
}
template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,1> atan2(const TA & a,
                                                                           const TB & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,1> rv(std::atan2(static_cast<TA>(a), static_cast<TB>(b)));
    return rv;
}

} // namespace Cg

#endif // __Cg_atan2_hpp__
