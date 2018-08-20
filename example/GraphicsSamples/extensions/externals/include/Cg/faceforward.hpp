/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_faceforward_hpp__
#define __Cg_faceforward_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/vector.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<typename __CGtype_trait<T>::numericType,N> faceforward(const __CGvector<T,N> & n,
                                                                                const __CGvector<T,N> & iv,
                                                                                const __CGvector<T,N> & ng)
{
    typedef typename __CGtype_trait<T>::resultType ResultType;
    typename __CGtype_trait<T>::dotType sum = iv[0] * ng[0];
    for (int i=1; i<N; i++)
        sum += iv[i] * ng[i];
    return __CGvector<ResultType,N>(sum < 0 ? n : -n);
}
template <typename TN, typename TI, typename TNG, int N, typename TNstore, typename TIstore, typename TNGstore>
static inline __CGvector<typename __CGtype_trait3<TN,TI,TNG>::numericType,N> faceforward(const __CGvector_usage<TN,N,TNstore> & n,
                                                                                         const __CGvector_usage<TI,N,TIstore> & iv,
                                                                                         const __CGvector_usage<TNG,N,TNGstore> & ng)
{
    typedef typename __CGtype_trait3<TN,TI,TNG>::resultType ResultType;
    typename __CGtype_trait3<TN,TI,TNG>::dotType sum = iv[0] * ng[0];
    for (int i=1; i<N; i++)
        sum += iv[i] * ng[i];
    return __CGvector<ResultType,N>(sum < 0 ? n : -n);
}

} // namespace Cg

#endif // __Cg_faceforward_hpp__
