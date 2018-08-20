/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/distance.hpp>

namespace Cg {

float1 distance(float1 a, float1 b)
{
    return distance<float,1>(a, b);
}

float1 distance(float2 a, float2 b)
{
    return distance<float,2>(a, b);
}

float1 distance(float3 a, float3 b)
{
    return distance<float,3>(a, b);
}

float1 distance(float4 a, float4 b)
{
    return distance<float,4>(a, b);
}

double1 distance(double1 a, double1 b)
{
    return distance<double,1>(a, b);
}

double1 distance(double2 a, double2 b)
{
    return distance<double,2>(a, b);
}

double1 distance(double3 a, double3 b)
{
    return distance<double,3>(a, b);
}

double1 distance(double4 a, double4 b)
{
    return distance<double,4>(a, b);
}

half1 distance(half1 a, half1 b)
{
    return distance<half,1>(a, b);
}

half1 distance(half2 a, half2 b)
{
    return distance<half,2>(a, b);
}

half1 distance(half3 a, half3 b)
{
    return distance<half,3>(a, b);
}

half1 distance(half4 a, half4 b)
{
    return distance<half,4>(a, b);
}

fixed1 distance(fixed1 a, fixed1 b)
{
    return distance<fixed,1>(a, b);
}

fixed1 distance(fixed2 a, fixed2 b)
{
    return distance<fixed,2>(a, b);
}

fixed1 distance(fixed3 a, fixed3 b)
{
    return distance<fixed,3>(a, b);
}

fixed1 distance(fixed4 a, fixed4 b)
{
    return distance<fixed,4>(a, b);
}

} // namespace Cg

