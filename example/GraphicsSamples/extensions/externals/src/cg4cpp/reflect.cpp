/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/stdlib.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<T,N> reflect(const __CGvector<T,N> & i, const __CGvector<T,N> & n)
{
    return i - 2 * n * dot(n,i);
}

float1 reflect(float1 i, float1 n)
{
    return reflect<float,1>(i, n);
}

float2 reflect(float2 i, float2 n)
{
    return reflect<float,2>(i, n);
}

float3 reflect(float3 i, float3 n)
{
    return reflect<float,3>(i, n);
}

float4 reflect(float4 i, float4 n)
{
    return reflect<float,4>(i, n);
}

double1 reflect(double1 i, double1 n)
{
    return reflect<double,1>(i, n);
}

double2 reflect(double2 i, double2 n)
{
    return reflect<double,2>(i, n);
}

double3 reflect(double3 i, double3 n)
{
    return reflect<double,3>(i, n);
}

double4 reflect(double4 i, double4 n)
{
    return reflect<double,4>(i, n);
}

half1 reflect(half1 i, half1 n)
{
    return reflect<half,1>(i, n);
}

half2 reflect(half2 i, half2 n)
{
    return reflect<half,2>(i, n);
}

half3 reflect(half3 i, half3 n)
{
    return reflect<half,3>(i, n);
}

half4 reflect(half4 i, half4 n)
{
    return reflect<half,4>(i, n);
}

fixed1 reflect(fixed1 i, fixed1 n)
{
    return reflect<fixed,1>(i, n);
}

fixed2 reflect(fixed2 i, fixed2 n)
{
    return reflect<fixed,2>(i, n);
}

fixed3 reflect(fixed3 i, fixed3 n)
{
    return reflect<fixed,3>(i, n);
}

fixed4 reflect(fixed4 i, fixed4 n)
{
    return reflect<fixed,4>(i, n);
}

} // namespace Cg

