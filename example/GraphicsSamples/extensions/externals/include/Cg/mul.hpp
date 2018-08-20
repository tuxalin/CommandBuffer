/* 
 * Copyright 2006-2012 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_mul_hpp__
#define __Cg_mul_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

// Matrix-by-vector multiplication (assuming a column vector)
template <typename TA, typename TB, int M, int N>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> mul(const __CGmatrix<TA,M,N> & m,
                                                                            const __CGvector<TB,N> & v)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> rv;
    for (int i=0; i<M; i++) {
        dotType sum = dotType(m[i][0]) * v[0];
        for (int j=1; j<N; j++) {
            sum += m[i][j] * v[j];
        }
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}
template <typename TA, typename TB, int M, int N, typename TBstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> mul(const __CGmatrix<TA,M,N> & m,
                                                                            const __CGvector_usage<TB,N,TBstore> & v)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,M> rv;
    for (int i=0; i<M; i++) {
        dotType sum = dotType(m[i][0]) * v[0];
        for (int j=1; j<N; j++) {
            sum += m[i][j] * v[j];
        }
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}

// Vector-by-matrix multiplication (assuming a row vector)
template <typename TA, typename TB, int M, int N>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> mul(const __CGvector<TA,M> & v,
                                                                            const __CGmatrix<TB,M,N> & m)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        dotType sum = dotType(v[0]) * m[0][i];
        for (int j=1; j<M; j++) {
            sum += v[j] * m[j][i];
        }
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}
template <typename TA, typename TB, int M, int N, typename TAstore>
static inline __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> mul(const __CGvector_usage<TA,M,TAstore> & v,
                                                                            const __CGmatrix<TB,M,N> & m)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGvector<typename __CGtype_trait<TA,TB>::numericType,N> rv;
    for (int i=0; i<N; i++) {
        dotType sum = dotType(v[0]) * m[0][i];
        for (int j=1; j<M; j++) {
            sum += v[j] * m[j][i];
        }
        rv[i] = typename __CGtype_trait<TA,TB>::numericType(sum);
    }
    return rv;
}

// Matrix-by-matrix multiplication
template <typename TA, typename TB, int X, int Y, int Z>
static inline __CGmatrix<typename __CGtype_trait<TA,TB>::numericType,X,Z> mul(const __CGmatrix<TA,X,Y> & a,
                                                                              const __CGmatrix<TB,Y,Z> & b)
{
    typedef typename __CGtype_trait<TA,TB>::dotType dotType;
    __CGmatrix<typename __CGtype_trait<TA,TB>::numericType,X,Z> rv;
    for (int i=0; i<X; i++) {
        for (int j=0; j<Z; j++) {
            dotType sum = dotType(a[i][0]) * b[0][j];
            for (int k=1; k<Y; k++) {
                sum += a[i][k] * b[k][j];
            }
            rv[i][j] = typename __CGtype_trait<TA,TB>::numericType(sum);
        }
    }
    return rv;
}

} // namespace Cg

#endif // __Cg_mul_hpp__
