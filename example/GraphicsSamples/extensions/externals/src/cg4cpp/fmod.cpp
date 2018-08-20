/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/fmod.hpp>

namespace Cg {

float1 fmod(float1 x, float1 y)
{
    return fmod<float,1>(x, y);
}

float2 fmod(float2 x, float2 y)
{
    return fmod<float,2>(x, y);
}

float3 fmod(float3 x, float3 y)
{
    return fmod<float,3>(x, y);
}

float4 fmod(float4 x, float4 y)
{
    return fmod<float,4>(x, y);
}

double1 fmod(double1 x, double1 y)
{
    return fmod<double,1>(x, y);
}

double2 fmod(double2 x, double2 y)
{
    return fmod<double,2>(x, y);
}

double3 fmod(double3 x, double3 y)
{
    return fmod<double,3>(x, y);
}

double4 fmod(double4 x, double4 y)
{
    return fmod<double,4>(x, y);
}

half1 fmod(half1 x, half1 y)
{
    return fmod<half,1>(x, y);
}

half2 fmod(half2 x, half2 y)
{
    return fmod<half,2>(x, y);
}

half3 fmod(half3 x, half3 y)
{
    return fmod<half,3>(x, y);
}

half4 fmod(half4 x, half4 y)
{
    return fmod<half,4>(x, y);
}

fixed1 fmod(fixed1 x, fixed1 y)
{
    return fmod<fixed,1>(x, y);
}

fixed2 fmod(fixed2 x, fixed2 y)
{
    return fmod<fixed,2>(x, y);
}

fixed3 fmod(fixed3 x, fixed3 y)
{
    return fmod<fixed,3>(x, y);
}

fixed4 fmod(fixed4 x, fixed4 y)
{
    return fmod<fixed,4>(x, y);
}

} // namespace Cg

