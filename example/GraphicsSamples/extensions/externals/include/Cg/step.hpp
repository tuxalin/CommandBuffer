/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_step_hpp__
#define __Cg_step_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <cmath> // for std::step

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> step(const __CGvector<T,N> & a,
                                                                      const __CGvector<T,N> & x)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = x[i] >= a[i];
    return rv;
}
template <typename TA, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> step(const __CGvector_usage<TA,N,TAstore> & a,
                                                                             const __CGvector_usage<TX,N,TXstore> & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = x[i] >= a[i];
    return rv;
}

template <typename TA, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> step(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                             const __CGvector_usage<TX,1,TXstore> & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = x[0] >= a[i];
    return rv;
}
template <typename TA, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> step(const __CGvector_usage<TA,1,TAstore> & a,
                                                                             const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = x[i] >= a[0];
    return rv;
}
template <typename TA, typename TX, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> step(const __CGvector_usage<TA,N,TAstore> & a,
                                                                             const TX & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = static_cast<TX>(x) >= a[i];
    return rv;
}
template <typename TA, typename TX, int N, typename TXstore>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> step(const TA & a,
                                                                             const __CGvector_usage<TX,N,TXstore> & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,N> rv;
    for (int i=0; i<N; i++)
        rv[i] = x[i] >= static_cast<TA>(a);
    return rv;
}
template <typename TA, typename TX>
static inline __CGvector<typename __CGtype_trait<TA,TX>::numericType,1> step(const TA & a,
                                                                             const TX & x)
{
    __CGvector<typename __CGtype_trait<TA,TX>::numericType,1> rv(static_cast<TX>(x) >= static_cast<TA>(a));
    return rv;
}

} // namespace Cg

#endif // __Cg_step_hpp__
