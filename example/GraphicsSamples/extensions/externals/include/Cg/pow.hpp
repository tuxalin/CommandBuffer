/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_pow_hpp__
#define __Cg_pow_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <cmath> // for std::exp & std::log

// Actually Cg stdlib really uses exp2 and log2 to implement pow

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> pow(const __CGvector<T,N> & x,
                                                                     const __CGvector<T,N> & y)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(x[i] * std::log(y[i]));
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> pow(const __CGvector_usage<TA,N,TAstore> & x,
                                                                         const __CGvector_usage<TB,N,TBstore> & y)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(x[i] * std::log(y[i]));
    return rv;
}

template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> pow(const __CGvector_plural_usage<TA,N,TAstore> & x,
                                                                         const __CGvector_usage<TB,1,TBstore> & y)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(x[i] * std::log(y[0]));
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> pow(const __CGvector_usage<TA,1,TAstore> & x,
                                                                         const __CGvector_plural_usage<TB,N,TBstore> & y)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(x[0] * std::log(y[i]));
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> pow(const __CGvector_usage<TA,N,TAstore> & x,
                                                                         const TB & y)
{
    typedef typename __CGtype_trait<TA,TB>::realType RealType;
    __CGvector<RealType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(x[i] * std::log(RealType(static_cast<TB>(y))));
    return rv;
}
template <typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> pow(const TA & x,
                                                                         const __CGvector_usage<TB,N,TBstore> & y)
{
    __CGvector<typename __CGtype_trait<TA,TB>::realType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = std::exp(static_cast<TA>(x) * std::log(y[i]));
    return rv;
}
template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,1> pow(const TA & x,
                                                                         const TB & y)
{
    typedef typename __CGtype_trait<TA,TB>::realType RealType;
    __CGvector<RealType,1> rv(std::exp(static_cast<RealType>(x) * std::log(static_cast<RealType>(y))));
    return rv;
}

} // namespace Cg

#endif // __Cg_pow_hpp__
