/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/dot.hpp>

namespace Cg {

float dot(float a, float b)
{
    return dot<float>(a, b);
}

float1 dot(float1 a, float1 b)
{
    return dot<float,1>(a, b);
}

float1 dot(float2 a, float2 b)
{
    return dot<float,2>(a, b);
}

float1 dot(float3 a, float3 b)
{
    return dot<float,3>(a, b);
}

float1 dot(float4 a, float4 b)
{
    return dot<float,4>(a, b);
}

double dot(double a, double b)
{
    return dot<double>(a, b);
}

double1 dot(double1 a, double1 b)
{
    return dot<double,1>(a, b);
}

double1 dot(double2 a, double2 b)
{
    return dot<double,2>(a, b);
}

double1 dot(double3 a, double3 b)
{
    return dot<double,3>(a, b);
}

double1 dot(double4 a, double4 b)
{
    return dot<double,4>(a, b);
}

half dot(half a, half b)
{
    return dot<half>(a, b);
}

half1 dot(half1 a, half1 b)
{
    return dot<half,1>(a, b);
}

half1 dot(half2 a, half2 b)
{
    return dot<half,2>(a, b);
}

half1 dot(half3 a, half3 b)
{
    return dot<half,3>(a, b);
}

half1 dot(half4 a, half4 b)
{
    return dot<half,4>(a, b);
}

fixed dot(fixed a, fixed b)
{
    return dot<fixed>(a, b);
}

fixed1 dot(fixed1 a, fixed1 b)
{
    return dot<fixed,1>(a, b);
}

fixed1 dot(fixed2 a, fixed2 b)
{
    return dot<fixed,2>(a, b);
}

fixed1 dot(fixed3 a, fixed3 b)
{
    return dot<fixed,3>(a, b);
}

fixed1 dot(fixed4 a, fixed4 b)
{
    return dot<fixed,4>(a, b);
}

} // namespace Cg

