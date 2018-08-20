/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_fmod_hpp__
#define __Cg_fmod_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

#include <math.h> // for ::abs & ::floor

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::realType,N> fmod(const __CGvector<T,N> & a,
                                                                      const __CGvector<T,N> & b)
{
    typedef typename __CGtype_trait<T>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a[i]/b[i]);
        rv[i] = (div - ::floor(div)) * ::fabs(b[i]);
        if (a[i] < 0)
            rv[i] = -rv[i];
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> fmod(const __CGvector_usage<TA,N,TAstore> & a,
                                                                          const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a[i]/b[i]);
        rv[i] = (div - ::floor(div)) * ::fabs(b[i]);
        if (a[i] < 0)
            rv[i] = -rv[i];
    }
    return rv;
}

template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> fmod(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                          const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a[i]/b[0]);
        rv[i] = (div - ::floor(div)) * ::fabs(b[0]);
        if (a[i] < 0)
            rv[i] = -rv[i];
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> fmod(const __CGvector_usage<TA,1,TAstore> & a,
                                                                          const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a[0]/b[i]);
        rv[i] = (div - ::floor(div)) * ::fabs(b[i]);
        if (a[0] < 0)
            rv[i] = -rv[i];
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> fmod(const __CGvector_usage<TA,N,TAstore> & a,
                                                                          const TB & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a[i]/b);
        rv[i] = (div - ::floor(div)) * ::fabs(b);
        if (a[i] < 0)
            rv[i] = -rv[i];
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,N> fmod(const TA & a,
                                                                          const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType div = ::fabs(a/b[i]);
        rv[i] = (div - ::floor(div)) * ::fabs(b[i]);
        if (a < 0)
            rv[i] = -rv[i];
    }
    return rv;
}
template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::realType,1> fmod(const TA & a,
                                                                          const TB & b)
{
    typedef typename __CGtype_trait<TA,TB>::resultType ResultType;
    __CGvector<ResultType,1> rv;
    ResultType div = ResultType(::fabs(a/b));
    rv = ResultType((div - ::floor(div)) * ::fabs(b));
    if (a < 0)
        rv = -rv;
    return rv;
}

} // namespace Cg

#endif // __Cg_fmod_hpp__
