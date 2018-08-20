/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/double.hpp>
#include <Cg/matrix/0based.hpp>
#include <Cg/matrix/rows.hpp>
#include <Cg/matrix.hpp>
#include <Cg/stdlib.hpp>

namespace Cg {

template <typename T>
static inline __CGvector<T,1> determinant1x1(const __CGmatrix<T,1,1> & A)
{
    return A[0][0];
}

template <typename T>
static inline __CGvector<T,1> determinant2x2(const __CGmatrix<T,2,2> & A)
{
    return A._m00*A._m11 - A._m01*A._m10;
}

template <typename T>
static inline __CGvector<T,1> determinant3x3(const __CGmatrix<T,3,3> & A)
{
    return dot(A._m00_m01_m02,
               A._m11_m12_m10 * A._m22_m20_m21
               - A._m12_m10_m11 * A._m21_m22_m20);
}

template <typename T>
static inline __CGvector<T,1> determinant4x4(const __CGmatrix<T,4,4> & A)
{
    return dot(float4(1,-1,1,-1) * A._m00_m01_m02_m03,
                 A._m11_m12_m13_m10*(  A._m22_m23_m20_m21*A._m33_m30_m31_m32
                                     - A._m23_m20_m21_m22*A._m32_m33_m30_m31)
               + A._m12_m13_m10_m11*(  A._m23_m20_m21_m22*A._m31_m32_m33_m30
                                     - A._m21_m22_m23_m20*A._m33_m30_m31_m32)
               + A._m13_m10_m11_m12*(  A._m21_m22_m23_m20*A._m32_m33_m30_m31
                                     - A._m22_m23_m20_m21*A._m31_m32_m33_m30));
}

float1 determinant(float1x1 A)
{
    return determinant1x1<float>(A);
}

float1 determinant(float2x2 A)
{
    return determinant2x2<float>(A);
}

float1 determinant(float3x3 A)
{
    return determinant3x3<float>(A);
}

float1 determinant(float4x4 A)
{
    return determinant4x4<float>(A);
}

double1 determinant(double1x1 A)
{
    return determinant1x1<double>(A);
}

double1 determinant(double2x2 A)
{
    return determinant2x2<double>(A);
}

double1 determinant(double3x3 A)
{
    return determinant3x3<double>(A);
}

double1 determinant(double4x4 A)
{
    return determinant4x4<double>(A);
}

} // namespace Cg

