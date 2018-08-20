/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_lerp_hpp__
#define __Cg_lerp_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> lerp(const __CGvector<T,N> & a,
                                                                         const __CGvector<T,N> & b,
                                                                         const __CGvector<T,N> & t)
{
    typedef typename __CGtype_trait<T>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_usage<TT,1,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType tv = t[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_usage<TB,1,TBstore> & b,
                                                                                 const __CGvector_plural_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_usage<TB,1,TBstore> & b,
                                                                                 const __CGvector_usage<TT,1,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    ResultType tv = t[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                 const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_plural_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                 const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_usage<TT,1,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType tv = t[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                 const __CGvector_usage<TB,1,TBstore> & b,
                                                                                 const __CGvector_plural_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_usage<TB,N,TBstore> & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType tv = ResultType(static_cast<TT>(t));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                 const TB & b,
                                                                                 const __CGvector_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TT, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,N,TAstore> & a,
                                                                                 const TB & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType tv = ResultType(static_cast<TT>(t));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
#endif
template <typename TA, typename TB, typename TT, int N, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const TA & a,
                                                                                 const __CGvector_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const TA & a,
                                                                                 const __CGvector_usage<TB,N,TBstore> & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType tv = ResultType(static_cast<TT>(t));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TT, int N, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const TA & a,
                                                                                 const TB & b,
                                                                                 const __CGvector_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
#endif
#if 1 //ndef _MSC_VER
template <typename TA, typename TB, typename TT>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,1> lerp(const TA & a,
                                                                                 const TB & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,1> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType tv = ResultType(static_cast<TT>(t));
    rv = av + tv*(bv-av);
    return rv;
}
#endif
template <typename TA, typename TB, typename TT, int N, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const TA & a,
                                                                                 const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                 const __CGvector_usage<TT,1,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType tv = t[0];
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                 const TB & b,
                                                                                 const __CGvector_usage<TT,1,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = ResultType(static_cast<TB>(b));
    ResultType tv = t[0];
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TBstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const TA & a,
                                                                                 const __CGvector_usage<TB,1,TBstore> & b,
                                                                                 const __CGvector_plural_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = ResultType(static_cast<TA>(a));
    ResultType bv = b[0];
    for (int i=0; i<N; i++) {
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                                 const __CGvector_usage<TB,1,TBstore> & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType bv = b[0];
    ResultType tv = ResultType(static_cast<TT>(t));
    for (int i=0; i<N; i++) {
        ResultType av = a[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TTstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                 const TB & b,
                                                                                 const __CGvector_plural_usage<TT,N,TTstore> & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType bv = ResultType(static_cast<TB>(b));
    for (int i=0; i<N; i++) {
        ResultType tv = t[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}
template <typename TA, typename TB, typename TT, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait3<TA,TB,TT>::numericType,N> lerp(const __CGvector_usage<TA,1,TAstore> & a,
                                                                                 const __CGvector_plural_usage<TB,N,TBstore> & b,
                                                                                 const TT & t)
{
    typedef typename __CGtype_trait3<TA,TB,TT>::resultType ResultType;
    __CGvector<ResultType,N> rv;
    ResultType av = a[0];
    ResultType tv = ResultType(static_cast<TT>(t));
    for (int i=0; i<N; i++) {
        ResultType bv = b[i];
        rv[i] = av + tv*(bv-av);
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_lerp_hpp__
