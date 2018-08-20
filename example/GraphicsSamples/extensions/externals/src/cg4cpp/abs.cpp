/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/abs.hpp>

namespace Cg {

int abs(int v)
{
    return abs<int>(v);
}

int1 abs(int1 v)
{
    return abs<int,1>(v);
}

int2 abs(int2 v)
{
    return abs<int,2>(v);
}

int3 abs(int3 v)
{
    return abs<int,3>(v);
}

int4 abs(int4 v)
{
    return abs<int,4>(v);
}

float abs(float v)
{
    return abs<float>(v);
}

float1 abs(float1 v)
{
    return abs<float,1>(v);
}

float2 abs(float2 v)
{
    return abs<float,2>(v);
}

float3 abs(float3 v)
{
    return abs<float,3>(v);
}

float4 abs(float4 v)
{
    return abs<float,4>(v);
}

double abs(double v)
{
    return abs<double>(v);
}

double1 abs(double1 v)
{
    return abs<double,1>(v);
}

double2 abs(double2 v)
{
    return abs<double,2>(v);
}

double3 abs(double3 v)
{
    return abs<double,3>(v);
}

double4 abs(double4 v)
{
    return abs<double,4>(v);
}

half abs(half v)
{
    return abs<half>(v);
}

half1 abs(half1 v)
{
    return abs<half,1>(v);
}

half2 abs(half2 v)
{
    return abs<half,2>(v);
}

half3 abs(half3 v)
{
    return abs<half,3>(v);
}

half4 abs(half4 v)
{
    return abs<half,4>(v);
}

fixed abs(fixed v)
{
    return abs<fixed>(v);
}

fixed1 abs(fixed1 v)
{
    return abs<fixed,1>(v);
}

fixed2 abs(fixed2 v)
{
    return abs<fixed,2>(v);
}

fixed3 abs(fixed3 v)
{
    return abs<fixed,3>(v);
}

fixed4 abs(fixed4 v)
{
    return abs<fixed,4>(v);
}

} // namespace Cg

