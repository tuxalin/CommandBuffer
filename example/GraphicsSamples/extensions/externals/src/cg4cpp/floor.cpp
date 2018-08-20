/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/floor.hpp>

namespace Cg {

float floor(float v)
{
    return floor<float,1>(v);
}

float1 floor(float1 v)
{
    return floor<float,1>(v);
}

float2 floor(float2 v)
{
    return floor<float,2>(v);
}

float3 floor(float3 v)
{
    return floor<float,3>(v);
}

float4 floor(float4 v)
{
    return floor<float,4>(v);
}

double floor(double v)
{
    return floor<double,1>(v);
}

double1 floor(double1 v)
{
    return floor<double,1>(v);
}

double2 floor(double2 v)
{
    return floor<double,2>(v);
}

double3 floor(double3 v)
{
    return floor<double,3>(v);
}

double4 floor(double4 v)
{
    return floor<double,4>(v);
}

half floor(half v)
{
    return floor<half,1>(v);
}

half1 floor(half1 v)
{
    return floor<half,1>(v);
}

half2 floor(half2 v)
{
    return floor<half,2>(v);
}

half3 floor(half3 v)
{
    return floor<half,3>(v);
}

half4 floor(half4 v)
{
    return floor<half,4>(v);
}

fixed floor(fixed v)
{
    return floor<fixed,1>(v);
}

fixed1 floor(fixed1 v)
{
    return floor<fixed,1>(v);
}

fixed2 floor(fixed2 v)
{
    return floor<fixed,2>(v);
}

fixed3 floor(fixed3 v)
{
    return floor<fixed,3>(v);
}

fixed4 floor(fixed4 v)
{
    return floor<fixed,4>(v);
}

} // namespace Cg
