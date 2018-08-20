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
static inline __CGvector<T,N> refract(const __CGvector<T,N> & i, const __CGvector<T,N> & n, const T & eta)
{
    float1 cosi(dot(-i, n));
    float1 cost2(1.0f - eta * eta * (1.0f - cosi*cosi));
    __CGvector<T,N> t(eta*i + ((eta*cosi - sqrt(abs(cost2))) * n));
    return t * __CGvector<T,N>(cost2 > 0);
}

float1 refract(float1 i, float1 n, float eta)
{
    return refract<float,1>(i, n, eta);
}
float2 refract(float2 i, float2 n, float eta)
{
    return refract<float,2>(i, n, eta);
}

float3 refract(float3 i, float3 n, float eta)
{
    return refract<float,3>(i, n, eta);
}

float4 refract(float4 i, float4 n, float eta)
{
    return refract<float,4>(i, n, eta);
}

double1 refract(double1 i, double1 n, double eta)
{
    return refract<double,1>(i, n, eta);
}

double2 refract(double2 i, double2 n, double eta)
{
    return refract<double,2>(i, n, eta);
}

double3 refract(double3 i, double3 n, double eta)
{
    return refract<double,3>(i, n, eta);
}

double4 refract(double4 i, double4 n, double eta)
{
    return refract<double,4>(i, n, eta);
}

half1 refract(half1 i, half1 n, half eta)
{
    return refract<half,1>(i, n, eta);
}

half2 refract(half2 i, half2 n, half eta)
{
    return refract<half,2>(i, n, eta);
}

half3 refract(half3 i, half3 n, half eta)
{
    return refract<half,3>(i, n, eta);
}

half4 refract(half4 i, half4 n, half eta)
{
    return refract<half,4>(i, n, eta);
}

fixed1 refract(fixed1 i, fixed1 n, fixed eta)
{
    return refract<fixed,1>(i, n, eta);
}

fixed2 refract(fixed2 i, fixed2 n, fixed eta)
{
    return refract<fixed,2>(i, n, eta);
}

fixed3 refract(fixed3 i, fixed3 n, fixed eta)
{
    return refract<fixed,3>(i, n, eta);
}

fixed4 refract(fixed4 i, fixed4 n, fixed eta)
{
    return refract<fixed,4>(i, n, eta);
}

} // namespace Cg

