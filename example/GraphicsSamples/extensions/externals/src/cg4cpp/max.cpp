/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/max.hpp>

namespace Cg {

int max(int a, int b)
{
    return max<int>(a, b);
}

int1 max(int1 a, int1 b)
{
    return max<int,1>(a, b);
}

int2 max(int2 a, int2 b)
{
    return max<int,2>(a, b);
}

int3 max(int3 a, int3 b)
{
    return max<int,3>(a, b);
}

int4 max(int4 a, int4 b)
{
    return max<int,4>(a, b);
}

float max(float a, float b)
{
    return max<float>(a, b);
}

float1 max(float1 a, float1 b)
{
    return max<float,1>(a, b);;
}

float2 max(float2 a, float2 b)
{
    return max<float,2>(a, b);
}

float3 max(float3 a, float3 b)
{
    return max<float,3>(a, b);
}

float4 max(float4 a, float4 b)
{
    return max<float,4>(a, b);
}

double max(double a, double b)
{
    return max<double>(a, b);
}

double1 max(double1 a, double1 b)
{
    return max<double,1>(a, b);
}

double2 max(double2 a, double2 b)
{
    return max<double,2>(a, b);
}

double3 max(double3 a, double3 b)
{
    return max<double,3>(a, b);
}

double4 max(double4 a, double4 b)
{
    return max<double,4>(a, b);
}

half max(half a, half b)
{
    return max<half>(a, b);
}

half1 max(half1 a, half1 b)
{
    return max<half,1>(a, b);
}

half2 max(half2 a, half2 b)
{
    return max<half,2>(a, b);
}

half3 max(half3 a, half3 b)
{
    return max<half,3>(a, b);
}

half4 max(half4 a, half4 b)
{
    return max<half,4>(a, b);
}

fixed max(fixed a, fixed b)
{
    return max<fixed>(a, b);
}

fixed1 max(fixed1 a, fixed1 b)
{
    return max<fixed,1>(a, b);
}

fixed2 max(fixed2 a, fixed2 b)
{
    return max<fixed,2>(a, b);
}

fixed3 max(fixed3 a, fixed3 b)
{
    return max<fixed,3>(a, b);
}

fixed4 max(fixed4 a, fixed4 b)
{
    return max<fixed,4>(a, b);
}

} // namespace Cg

