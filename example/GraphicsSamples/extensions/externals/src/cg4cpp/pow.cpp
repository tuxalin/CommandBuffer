/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/pow.hpp>

namespace Cg {

float pow(float x, float y)
{
    return pow<float>(x, y);
}

float1 pow(float1 x, float1 y)
{
    return pow<float,1>(x, y);
}

float2 pow(float2 x, float2 y)
{
    return pow<float,2>(x, y);
}

float3 pow(float3 x, float3 y)
{
    return pow<float,3>(x, y);
}

float4 pow(float4 x, float4 y)
{
    return pow<float,4>(x, y);
}

double pow(double x, double y)
{
    return pow<double>(x, y);
}

double1 pow(double1 x, double1 y)
{
    return pow<double,1>(x, y);
}

double2 pow(double2 x, double2 y)
{
    return pow<double,2>(x, y);
}

double3 pow(double3 x, double3 y)
{
    return pow<double,3>(x, y);
}

double4 pow(double4 x, double4 y)
{
    return pow<double,4>(x, y);
}

half pow(half x, half y)
{
    return pow<half>(x, y);
}

half1 pow(half1 x, half1 y)
{
    return pow<half,1>(x, y);
}

half2 pow(half2 x, half2 y)
{
    return pow<half,2>(x, y);
}

half3 pow(half3 x, half3 y)
{
    return pow<half,3>(x, y);
}

half4 pow(half4 x, half4 y)
{
    return pow<half,4>(x, y);
}

fixed pow(fixed x, fixed y)
{
    return pow<fixed>(x, y);
}

fixed1 pow(fixed1 x, fixed1 y)
{
    return pow<fixed,1>(x, y);
}

fixed2 pow(fixed2 x, fixed2 y)
{
    return pow<fixed,2>(x, y);
}

fixed3 pow(fixed3 x, fixed3 y)
{
    return pow<fixed,3>(x, y);
}

fixed4 pow(fixed4 x, fixed4 y)
{
    return pow<fixed,4>(x, y);
}

} // namespace Cg

