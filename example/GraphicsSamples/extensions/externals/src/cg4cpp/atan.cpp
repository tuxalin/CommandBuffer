/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/atan.hpp>

namespace Cg {

float atan(float v)
{
    return atan<float>(v);
}

float1 atan(float1 v)
{
    return atan<float,1>(v);
}

float2 atan(float2 v)
{
    return atan<float,2>(v);
}

float3 atan(float3 v)
{
    return atan<float,3>(v);
}

float4 atan(float4 v)
{
    return atan<float,4>(v);
}

double atan(double v)
{
    return atan<double>(v);
}

double1 atan(double1 v)
{
    return atan<double,1>(v);
}

double2 atan(double2 v)
{
    return atan<double,2>(v);
}

double3 atan(double3 v)
{
    return atan<double,3>(v);
}

double4 atan(double4 v)
{
    return atan<double,4>(v);
}

half atan(half v)
{
    return atan<half>(v);
}

half1 atan(half1 v)
{
    return atan<half,1>(v);
}

half2 atan(half2 v)
{
    return atan<half,2>(v);
}

half3 atan(half3 v)
{
    return atan<half,3>(v);
}

half4 atan(half4 v)
{
    return atan<half,4>(v);
}

fixed atan(fixed v)
{
    return atan<fixed>(v);
}

fixed1 atan(fixed1 v)
{
    return atan<fixed,1>(v);
}

fixed2 atan(fixed2 v)
{
    return atan<fixed,2>(v);
}

fixed3 atan(fixed3 v)
{
    return atan<fixed,3>(v);
}

fixed4 atan(fixed4 v)
{
    return atan<fixed,4>(v);
}

} // namespace Cg
