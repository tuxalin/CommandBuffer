/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/sin.hpp>

namespace Cg {

float sin(float v)
{
    return sin<float,1>(v);
}

float1 sin(float1 v)
{
    return sin<float,1>(v);
}

float2 sin(float2 v)
{
    return sin<float,2>(v);
}

float3 sin(float3 v)
{
    return sin<float,3>(v);
}

float4 sin(float4 v)
{
    return sin<float,4>(v);
}

double sin(double v)
{
    return sin<double,1>(v);
}

double1 sin(double1 v)
{
    return sin<double,1>(v);
}

double2 sin(double2 v)
{
    return sin<double,2>(v);
}

double3 sin(double3 v)
{
    return sin<double,3>(v);
}

double4 sin(double4 v)
{
    return sin<double,4>(v);
}

half sin(half v)
{
    return sin<half,1>(v);
}

half1 sin(half1 v)
{
    return sin<half,1>(v);
}

half2 sin(half2 v)
{
    return sin<half,2>(v);
}

half3 sin(half3 v)
{
    return sin<half,3>(v);
}

half4 sin(half4 v)
{
    return sin<half,4>(v);
}

fixed sin(fixed v)
{
    return sin<fixed,1>(v);
}

fixed1 sin(fixed1 v)
{
    return sin<fixed,1>(v);
}

fixed2 sin(fixed2 v)
{
    return sin<fixed,2>(v);
}

fixed3 sin(fixed3 v)
{
    return sin<fixed,3>(v);
}

fixed4 sin(fixed4 v)
{
    return sin<fixed,4>(v);
}

} // namespace Cg
