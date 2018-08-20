/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_clamp_hpp__
#define __Cg_clamp_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> clamp(const __CGvector<T,N> & x,
                                                                          const __CGvector<T,N> & a,
                                                                          const __CGvector<T,N> & b)
{
    typedef typename __CGtype_trait<T>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType av = a[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType av = a[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_plural_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_plural_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_usage<TA,1,TAstore> & a,
                                                                                  const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_plural_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_usage<TA,1,TAstore> & a,
                                                                                  const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,1,TXstore> & x,
                                                                                  const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,1,TXstore> & x,
                                                                                  const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,1,TXstore> & x,
                                                                                  const __CGvector_usage<TA,1,TAstore> & a,
                                                                                  const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    ResultType av = a[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_usage<TA,N,TAstore> & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,N,TXstore> & x,
                                                                                  const TA & a,
                                                                                  const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TX, typename TA, typename TB, int N, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,N,TXstore> & x,
                                                                                  const TA & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
#endif
template <typename TX, typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const TX & x,
                                                                                  const __CGvector_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const TX & x,
                                                                                  const __CGvector_usage<TA,N,TAstore> & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TX, typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const TX & x,
                                                                                  const TA & a,
                                                                                  const __CGvector_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType av = ResultType(static_cast<TA>(a));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TX, typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,1> clamp(const TX & x,
                                                                                  const TA & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,1> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    if (bv < xv)
        rv = bv;
    else if (av < xv)
        rv = xv;
    else
        rv = av;
    return rv;
}
#endif
template <typename TX, typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const TX & x,
                                                                                  const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                  const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_plural_usage<TX,N,TXstore> & x,
                                                                                  const TA & a,
                                                                                  const __CGvector_usage<TB,1,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const TX & x,
                                                                                  const __CGvector_usage<TA,1,TAstore> & a,
                                                                                  const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType av = a[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_plural_usage<TX,N,TXstore> & x,
                                                                                  const __CGvector_usage<TA,1,TAstore> & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,1,TXstore> & x,
                                                                                  const TA & a,
                                                                                  const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    ResultType av = ResultType(static_cast<TA>(a));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}
template <typename TX, typename TA, typename TB, int N, typename TXstore, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TX,TA,TB>::numericType,N> clamp(const __CGvector_usage<TX,1,TXstore> & x,
                                                                                  const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                  const TB & b)
{
    typedef typename __CGtype_trait3<TX,TA,TB>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        if (bv < xv)
            rv[i] = bv;
        else if (av < xv)
            rv[i] = xv;
        else
            rv[i] = av;
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_clamp_hpp__
