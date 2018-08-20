/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_dotv1_hpp__
#define __Cg_dotv1_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

// A one shorter than B

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,1> dotv1(const __CGvector<T,N-1> & a, const __CGvector<T,N> & b)
{
    typedef typename __CGtype_trait<T>::numericType numericType;
    typename __CGtype_trait<T>::dotType sum = a[0] * b[0];
    for (int i=1; i<N-1; i++)
        sum += a[i] * b[i];
    sum += b[N-1];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,1> dotv1(const __CGvector_usage<TA,N-1,TAstore> & a,
                                                                              const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * b[0];
    for (int i=1; i<N-1; i++)
        sum += a[i] * b[i];
    sum += b[N-1];
    return __CGvector<numericType,1>(numericType(sum));
}

// B one shorter than A

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,1> dotv1(const __CGvector<T,N> & a, const __CGvector<T,N-1> & b)
{
    typedef typename __CGtype_trait<T>::numericType numericType;
    typename __CGtype_trait<T>::dotType sum = a[0] * b[0];
    for (int i=1; i<N-1; i++)
        sum += a[i] * b[i];
    sum += a[N-1];
    return __CGvector<numericType,1>(numericType(sum));
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,1> dotv1(const __CGvector_usage<TA,N,TAstore> & a,
                                                                              const __CGvector_usage<TB,N-1,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::numericType numericType;
    typename __CGtype_trait<TA,TB>::dotType sum = a[0] * b[0];
    for (int i=1; i<N-1; i++)
        sum += a[i] * b[i];
    sum += a[N-1];
    return __CGvector<numericType,1>(numericType(sum));
}

} // namespace Cg

#endif // __Cg_dotv1_hpp__
