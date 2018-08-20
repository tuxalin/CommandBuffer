/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/min.hpp>

namespace Cg {

int min(int a, int b)
{
    return min<int>(a, b);
}

int1 min(int1 a, int1 b)
{
    return min<int,1>(a, b);
}

int2 min(int2 a, int2 b)
{
    return min<int,2>(a, b);
}

int3 min(int3 a, int3 b)
{
    return min<int,3>(a, b);
}

int4 min(int4 a, int4 b)
{
    return min<int,4>(a, b);
}

float min(float a, float b)
{
    return min<float>(a, b);
}

float1 min(float1 a, float1 b)
{
    return min<float,1>(a, b);;
}

float2 min(float2 a, float2 b)
{
    return min<float,2>(a, b);
}

float3 min(float3 a, float3 b)
{
    return min<float,3>(a, b);
}

float4 min(float4 a, float4 b)
{
    return min<float,4>(a, b);
}

double min(double a, double b)
{
    return min<double>(a, b);
}

double1 min(double1 a, double1 b)
{
    return min<double,1>(a, b);
}

double2 min(double2 a, double2 b)
{
    return min<double,2>(a, b);
}

double3 min(double3 a, double3 b)
{
    return min<double,3>(a, b);
}

double4 min(double4 a, double4 b)
{
    return min<double,4>(a, b);
}

half min(half a, half b)
{
    return min<half>(a, b);
}

half1 min(half1 a, half1 b)
{
    return min<half,1>(a, b);
}

half2 min(half2 a, half2 b)
{
    return min<half,2>(a, b);
}

half3 min(half3 a, half3 b)
{
    return min<half,3>(a, b);
}

half4 min(half4 a, half4 b)
{
    return min<half,4>(a, b);
}

fixed min(fixed a, fixed b)
{
    return min<fixed>(a, b);
}

fixed1 min(fixed1 a, fixed1 b)
{
    return min<fixed,1>(a, b);
}

fixed2 min(fixed2 a, fixed2 b)
{
    return min<fixed,2>(a, b);
}

fixed3 min(fixed3 a, fixed3 b)
{
    return min<fixed,3>(a, b);
}

fixed4 min(fixed4 a, fixed4 b)
{
    return min<fixed,4>(a, b);
}

} // namespace Cg

