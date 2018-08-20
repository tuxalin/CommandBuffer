/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/sqrt.hpp>

namespace Cg {

float sqrt(float v)
{
    return sqrt<float,1>(v);
}

float1 sqrt(float1 v)
{
    return sqrt<float,1>(v);
}

float2 sqrt(float2 v)
{
    return sqrt<float,2>(v);
}

float3 sqrt(float3 v)
{
    return sqrt<float,3>(v);
}

float4 sqrt(float4 v)
{
    return sqrt<float,4>(v);
}

double sqrt(double v)
{
    return sqrt<double,1>(v);
}

double1 sqrt(double1 v)
{
    return sqrt<double,1>(v);
}

double2 sqrt(double2 v)
{
    return sqrt<double,2>(v);
}

double3 sqrt(double3 v)
{
    return sqrt<double,3>(v);
}

double4 sqrt(double4 v)
{
    return sqrt<double,4>(v);
}

half sqrt(half v)
{
    return sqrt<half,1>(v);
}

half1 sqrt(half1 v)
{
    return sqrt<half,1>(v);
}

half2 sqrt(half2 v)
{
    return sqrt<half,2>(v);
}

half3 sqrt(half3 v)
{
    return sqrt<half,3>(v);
}

half4 sqrt(half4 v)
{
    return sqrt<half,4>(v);
}

fixed sqrt(fixed v)
{
    return sqrt<fixed,1>(v);
}

fixed1 sqrt(fixed1 v)
{
    return sqrt<fixed,1>(v);
}

fixed2 sqrt(fixed2 v)
{
    return sqrt<fixed,2>(v);
}

fixed3 sqrt(fixed3 v)
{
    return sqrt<fixed,3>(v);
}

fixed4 sqrt(fixed4 v)
{
    return sqrt<fixed,4>(v);
}

} // namespace Cg
