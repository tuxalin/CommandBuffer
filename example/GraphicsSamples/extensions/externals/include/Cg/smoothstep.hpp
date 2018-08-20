/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_smoothstep_hpp__
#define __Cg_smoothstep_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> smoothstep(const __CGvector<T,N> & a,
                                                                               const __CGvector<T,N> & b,
                                                                               const __CGvector<T,N> & x)
{
    typedef typename __CGtype_trait<T>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_usage<TX,1,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_usage<TB,1,TBstore> & b,
                                                                                       const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_usage<TB,1,TBstore> & b,
                                                                                       const __CGvector_usage<TX,1,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                       const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                       const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_usage<TX,1,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                       const __CGvector_usage<TB,1,TBstore> & b,
                                                                                       const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_usage<TB,N,TBstore> & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                       const TB & b,
                                                                                       const __CGvector_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TX, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                       const TB & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
#endif
template <typename TA, typename TB, typename TX, int N, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const TA & a,
                                                                                       const __CGvector_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const TA & a,
                                                                                       const __CGvector_usage<TB,N,TBstore> & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TX, int N, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const TA & a,
                                                                                       const TB & b,
                                                                                       const __CGvector_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TX>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,1> smoothstep(const TA & a,
                                                                                       const TB & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,1> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType xv = ResultType(static_cast<TX>(x));
    ResultType y = (xv - av)/(bv - av);
    if (1 < xv)
        y = 1;
    else if (0 < y)
        y = y;
    else
        y = 0;
    rv = y*y*(3 - 2*y);
    return rv;
}
#endif
template <typename TA, typename TB, typename TX, int N, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const TA & a,
                                                                                       const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                       const __CGvector_usage<TX,1,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                       const TB & b,
                                                                                       const __CGvector_usage<TX,1,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType xv = x[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TBstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const TA & a,
                                                                                       const __CGvector_usage<TB,1,TBstore> & b,
                                                                                       const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                       const __CGvector_usage<TB,1,TBstore> & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TXstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                       const TB & b,
                                                                                       const __CGvector_plural_usage<TX,N,TXstore> & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType xv = x[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}
template <typename TA, typename TB, typename TX, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TX>::numericType,N> smoothstep(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                       const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                       const TX & x)
{
    typedef typename __CGtype_trait3<TA,TB,TX>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType xv = ResultType(static_cast<TX>(x));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType y = (xv - av)/(bv - av);
        if (1 < y)
            y = 1;
        else if (0 < y)
            y = y;
        else
            y = 0;
        rv[i] = y*y*(3 - 2*y);
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_smoothstep_hpp__
