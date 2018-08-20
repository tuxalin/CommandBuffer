/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/saturate.hpp>

namespace Cg {

int saturate(int v)
{
    return saturate<int>(v);
}

int1 saturate(int1 v)
{
    return saturate<int,1>(v);
}

int2 saturate(int2 v)
{
    return saturate<int,2>(v);
}

int3 saturate(int3 v)
{
    return saturate<int,3>(v);
}

int4 saturate(int4 v)
{
    return saturate<int,4>(v);
}

float saturate(float v)
{
    return saturate<float>(v);
}

float1 saturate(float1 v)
{
    return saturate<float,1>(v);
}

float2 saturate(float2 v)
{
    return saturate<float,2>(v);
}

float3 saturate(float3 v)
{
    return saturate<float,3>(v);
}

float4 saturate(float4 v)
{
    return saturate<float,4>(v);
}

double saturate(double v)
{
    return saturate<double>(v);
}

double1 saturate(double1 v)
{
    return saturate<double,1>(v);
}

double2 saturate(double2 v)
{
    return saturate<double,2>(v);
}

double3 saturate(double3 v)
{
    return saturate<double,3>(v);
}

double4 saturate(double4 v)
{
    return saturate<double,4>(v);
}

half saturate(half v)
{
    return saturate<half>(v);
}

half1 saturate(half1 v)
{
    return saturate<half,1>(v);
}

half2 saturate(half2 v)
{
    return saturate<half,2>(v);
}

half3 saturate(half3 v)
{
    return saturate<half,3>(v);
}

half4 saturate(half4 v)
{
    return saturate<half,4>(v);
}

fixed saturate(fixed v)
{
    return saturate<fixed>(v);
}

fixed1 saturate(fixed1 v)
{
    return saturate<fixed,1>(v);
}

fixed2 saturate(fixed2 v)
{
    return saturate<fixed,2>(v);
}

fixed3 saturate(fixed3 v)
{
    return saturate<fixed,3>(v);
}

fixed4 saturate(fixed4 v)
{
    return saturate<fixed,4>(v);
}

} // namespace Cg

