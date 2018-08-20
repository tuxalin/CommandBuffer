/* 
 * Copyright 20062006-2012 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_mulv0_hpp__
#define __Cg_mulv0_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

// Matrix-by-vector multiplication (assuming a column vector) assuming vector has "implied 0" final component
template <typename TA, typename TB, int M, int N>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> mulv0(const __CGmatrix<TA,M,N> & m,
                                                                              const __CGvector<TB,N-1> & v)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> rv;
    for (int i=0; i<M; i++) {
        dotType sum = dotType(m[i][0]) * v[0];
        for (int j=1; j<N-1; j++) {
            sum += m[i][j] * v[j];
        }
        // sum += zero * m[i][N-1];
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}
template <typename TA, typename TB, int M, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> mulv0(const __CGmatrix<TA,M,N> & m,
                                                                              const __CGvector_usage<TB,N-1,TBstore> & v)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> rv;
    for (int i=0; i<M; i++) {
        dotType sum = dotType(m[i][0]) * v[0];
        for (int j=1; j<N-1; j++) {
            sum += m[i][j] * v[j];
        }
        // sum += zero * m[i][N-1];
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}

// Vector-by-matrix multiplication (assuming a row vector) assuming vector has "implied 0" final component
template <typename TA, typename TB, int M, int N>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> mulv0(const __CGvector<TA,M-1> & v,
                                                                              const __CGmatrix<TB,M,N> & m)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        dotType sum = dotType(v[0]) * m[0][i];
        for (int j=1; j<M-1; j++) {
            sum += v[j] * m[j][i];
        }
        // sum += zero * m[N-1][i];
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}
template <typename TA, typename TB, int M, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> mulv0(const __CGvector_usage<TA,M-1,TAstore> & v,
                                                                              const __CGmatrix<TB,M,N> & m)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        dotType sum = dotType(v[0]) * m[0][i];
        for (int j=1; j<M-1; j++) {
            sum += v[j] * m[j][i];
        }
        // sum += zero * m[N-1][i];
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_mul_hpp__
