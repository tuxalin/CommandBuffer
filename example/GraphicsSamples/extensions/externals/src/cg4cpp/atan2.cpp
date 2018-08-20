/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/atan2.hpp>

namespace Cg {

float atan2(float x, float y)
{
    return atan2<float>(x, y);
}

float1 atan2(float1 x, float1 y)
{
    return atan2<float,1>(x, y);
}

float2 atan2(float2 x, float2 y)
{
    return atan2<float,2>(x, y);
}

float3 atan2(float3 x, float3 y)
{
    return atan2<float,3>(x, y);
}

float4 atan2(float4 x, float4 y)
{
    return atan2<float,4>(x, y);
}

double atan2(double x, double y)
{
    return atan2<double>(x, y);
}

double1 atan2(double1 x, double1 y)
{
    return atan2<double,1>(x, y);
}

double2 atan2(double2 x, double2 y)
{
    return atan2<double,2>(x, y);
}

double3 atan2(double3 x, double3 y)
{
    return atan2<double,3>(x, y);
}

double4 atan2(double4 x, double4 y)
{
    return atan2<double,4>(x, y);
}

half atan2(half x, half y)
{
    return atan2<half>(x, y);
}

half1 atan2(half1 x, half1 y)
{
    return atan2<half,1>(x, y);
}

half2 atan2(half2 x, half2 y)
{
    return atan2<half,2>(x, y);
}

half3 atan2(half3 x, half3 y)
{
    return atan2<half,3>(x, y);
}

half4 atan2(half4 x, half4 y)
{
    return atan2<half,4>(x, y);
}

fixed atan2(fixed x, fixed y)
{
    return atan2<fixed>(x, y);
}

fixed1 atan2(fixed1 x, fixed1 y)
{
    return atan2<fixed,1>(x, y);
}

fixed2 atan2(fixed2 x, fixed2 y)
{
    return atan2<fixed,2>(x, y);
}

fixed3 atan2(fixed3 x, fixed3 y)
{
    return atan2<fixed,3>(x, y);
}

fixed4 atan2(fixed4 x, fixed4 y)
{
    return atan2<fixed,4>(x, y);
}

} // namespace Cg

