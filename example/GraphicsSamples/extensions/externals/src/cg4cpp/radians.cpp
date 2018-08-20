/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/radians.hpp>

namespace Cg {

float radians(float v)
{
    return radians<float,1>(v);
}

float1 radians(float1 v)
{
    return radians<float,1>(v);
}

float2 radians(float2 v)
{
    return radians<float,2>(v);
}

float3 radians(float3 v)
{
    return radians<float,3>(v);
}

float4 radians(float4 v)
{
    return radians<float,4>(v);
}

double radians(double v)
{
    return radians<double,1>(v);
}

double1 radians(double1 v)
{
    return radians<double,1>(v);
}

double2 radians(double2 v)
{
    return radians<double,2>(v);
}

double3 radians(double3 v)
{
    return radians<double,3>(v);
}

double4 radians(double4 v)
{
    return radians<double,4>(v);
}

half radians(half v)
{
    return radians<half,1>(v);
}

half1 radians(half1 v)
{
    return radians<half,1>(v);
}

half2 radians(half2 v)
{
    return radians<half,2>(v);
}

half3 radians(half3 v)
{
    return radians<half,3>(v);
}

half4 radians(half4 v)
{
    return radians<half,4>(v);
}

fixed radians(fixed v)
{
    return radians<fixed,1>(v);
}

fixed1 radians(fixed1 v)
{
    return radians<fixed,1>(v);
}

fixed2 radians(fixed2 v)
{
    return radians<fixed,2>(v);
}

fixed3 radians(fixed3 v)
{
    return radians<fixed,3>(v);
}

fixed4 radians(fixed4 v)
{
    return radians<fixed,4>(v);
}

} // namespace Cg

