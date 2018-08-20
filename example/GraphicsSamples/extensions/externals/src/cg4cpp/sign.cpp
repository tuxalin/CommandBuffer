/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/sign.hpp>

namespace Cg {

int sign(int v)
{
    return sign<int>(v);
}

int1 sign(int1 v)
{
    return sign<int,1>(v);
}

int2 sign(int2 v)
{
    return sign<int,2>(v);
}

int3 sign(int3 v)
{
    return sign<int,3>(v);
}

int4 sign(int4 v)
{
    return sign<int,4>(v);
}

float sign(float v)
{
    return sign<float>(v);
}

float1 sign(float1 v)
{
    return sign<float,1>(v);
}

float2 sign(float2 v)
{
    return sign<float,2>(v);
}

float3 sign(float3 v)
{
    return sign<float,3>(v);
}

float4 sign(float4 v)
{
    return sign<float,4>(v);
}

double sign(double v)
{
    return sign<double>(v);
}

double1 sign(double1 v)
{
    return sign<double,1>(v);
}

double2 sign(double2 v)
{
    return sign<double,2>(v);
}

double3 sign(double3 v)
{
    return sign<double,3>(v);
}

double4 sign(double4 v)
{
    return sign<double,4>(v);
}

half sign(half v)
{
    return sign<half>(v);
}

half1 sign(half1 v)
{
    return sign<half,1>(v);
}

half2 sign(half2 v)
{
    return sign<half,2>(v);
}

half3 sign(half3 v)
{
    return sign<half,3>(v);
}

half4 sign(half4 v)
{
    return sign<half,4>(v);
}

fixed sign(fixed v)
{
    return sign<fixed>(v);
}

fixed1 sign(fixed1 v)
{
    return sign<fixed,1>(v);
}

fixed2 sign(fixed2 v)
{
    return sign<fixed,2>(v);
}

fixed3 sign(fixed3 v)
{
    return sign<fixed,3>(v);
}

fixed4 sign(fixed4 v)
{
    return sign<fixed,4>(v);
}

} // namespace Cg

