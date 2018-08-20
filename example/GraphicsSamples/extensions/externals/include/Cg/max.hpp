/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_max_hpp__
#define __Cg_max_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

// Win32's windef.h defines max and min macros (unless NOMINMAX is defined)
// Attempt to push/pop the max macro to avoid interference from the macro
# ifdef _MSC_VER
#  pragma push_macro("max")
#  undef max
# endif

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> max(const __CGvector<T,N> & a, const __CGvector<T,N> & b)
{
    __CGvector<T,N> rv;
    for (int i=0; i<N; i++) {
        if (a[i] < b[i]) {
            rv[i] = b[i];
        } else {
            rv[i] = a[i];
        }
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> max(const __CGvector_usage<TA,N,TAstore> & a,
                                                                            const __CGvector_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        if (a[i] < b[i]) {
            rv[i] = b[i];
        } else {
            rv[i] = a[i];
        }
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> max(const __CGvector_plural_usage<TA,N,TAstore> & a,
                                                                            const __CGvector_usage<TB,1,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        if (a[i] < b) {
            rv[i] = b[0];
        } else {
            rv[i] = a[i];
        }
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> max(const __CGvector_usage<TA,1,TAstore> & a,
                                                                            const __CGvector_plural_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        if (a < b[i]) {
            rv[i] = b[i];
        } else {
            rv[i] = a[0];
        }
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> max(const __CGvector_usage<TA,N,TAstore> & a,
                                                                            const TB & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        if (a[i] < static_cast<TB>(b)) {
            rv[i] = static_cast<TB>(b);
        } else {
            rv[i] = a[i];
        }
    }
    return rv;
}
template <typename TA, typename TB, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> max(const TA & a,
                                                                            const __CGvector_usage<TB,N,TBstore> & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        if (static_cast<TA>(a) < b[i]) {
            rv[i] = b[i];
        } else {
            rv[i] = static_cast<TA>(a);
        }
    }
    return rv;
}
template <typename TA, typename TB>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,1> max(const TA & a,
                                                                            const TB & b)
{
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,1> rv;
    if (static_cast<TA>(a) < static_cast<TB>(b)) {
        rv[0] = static_cast<TB>(b);
    } else {
        rv[0] = static_cast<TA>(a);
    }
    return rv;
}

# ifdef _MSC_VER
#  pragma pop_macro("max")
# endif

} // namespace Cg

#endif // __Cg_max_hpp__
