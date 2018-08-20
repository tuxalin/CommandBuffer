/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_dot_hpp__
#define __Cg_dot_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,1> dot(const __CGvector<T,N> & a, const __CGvector<T,N> & b)
{
    typedef typename __CGtype_trait<T>::numericType numericType;
    typename __CGtype_trait<T>::dotType sum = a[0] * b[0];
    for (int i=1; i<N; i++)
        sum += a[i] * b[i];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,1> dot(const __CGvector_usage<TA,N,TAstore> & a,
                                                                            const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * b[0];
    for (int i=1; i<N; i++)
        sum += a[i] * b[i];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::resultType,1> dot(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                           const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * b[0];
    for (int i=1; i<N; i++)
        sum += a[i] * b;
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::resultType,1> dot(const __CGvector_usage<TA,1,TAstore> & a,
                                                                           const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * b[0];
    for (int i=1; i<N; i++)
        sum += a * b[i];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::resultType,1> dot(const __CGvector_usage<TA,N,TAstore> & a,
                                                                           const TB & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * static_cast<TB>(b);
    for (int i=1; i<N; i++)
        sum += a[i] * static_cast<TB>(b);
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::resultType,1> dot(const TA & a,
                                                                           const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = static_cast<TA>(a) * b[0];
    for (int i=1; i<N; i++)
        sum += static_cast<TA>(a) * b[i];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::resultType,1> dot(const TA & a,
                                                                           const TB & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = static_cast<TA>(a) * static_cast<TB>(b);
    return __CGvector<numericType,1>(numericType(sum));
}

} // namespace Cg

#endif // __Cg_dot_hpp__
