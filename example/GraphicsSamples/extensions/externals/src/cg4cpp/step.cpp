/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/step.hpp>

namespace Cg {

int1 step(int1 a, int1 x)
{
    return step<int,1>(a, x);
}

int2 step(int2 a, int2 x)
{
    return step<int,2>(a, x);
}

int3 step(int3 a, int3 x)
{
    return step<int,3>(a, x);
}

int4 step(int4 a, int4 x)
{
    return step<int,4>(a, x);
}

float1 step(float1 a, float1 x)
{
    return step<float,1>(a, x);;
}

float2 step(float2 a, float2 x)
{
    return step<float,2>(a, x);
}

float3 step(float3 a, float3 x)
{
    return step<float,3>(a, x);
}

float4 step(float4 a, float4 x)
{
    return step<float,4>(a, x);
}

double1 step(double1 a, double1 x)
{
    return step<double,1>(a, x);
}

double2 step(double2 a, double2 x)
{
    return step<double,2>(a, x);
}

double3 step(double3 a, double3 x)
{
    return step<double,3>(a, x);
}

double4 step(double4 a, double4 x)
{
    return step<double,4>(a, x);
}

half1 step(half1 a, half1 x)
{
    return step<half,1>(a, x);
}

half2 step(half2 a, half2 x)
{
    return step<half,2>(a, x);
}

half3 step(half3 a, half3 x)
{
    return step<half,3>(a, x);
}

half4 step(half4 a, half4 x)
{
    return step<half,4>(a, x);
}

fixed1 step(fixed1 a, fixed1 x)
{
    return step<fixed,1>(a, x);
}

fixed2 step(fixed2 a, fixed2 x)
{
    return step<fixed,2>(a, x);
}

fixed3 step(fixed3 a, fixed3 x)
{
    return step<fixed,3>(a, x);
}

fixed4 step(fixed4 a, fixed4 x)
{
    return step<fixed,4>(a, x);
}

} // namespace Cg

