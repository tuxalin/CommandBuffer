/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_sign_hpp__
#define __Cg_sign_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> sign(const __CGvector<T,N> & v)
{
    typedef __CGvector<T,N> ResultType;
    return ResultType(v > 0) - ResultType(v < 0);
}
template <typename T, int N, typename Tstore>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> sign(const __CGvector_usage<T,N,Tstore> & v)
{
    typedef __CGvector<T,N> ResultType;
    return ResultType(v > 0) - ResultType(v < 0);
}
template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::numericType,1> sign(const T & v)
{
    typedef __CGvector<T,1> ResultType;
    return ResultType(static_cast<T>(v) > 0) - ResultType(static_cast<T>(v) < 0);
}

} // namespace Cg

#endif // __Cg_sign_hpp__
