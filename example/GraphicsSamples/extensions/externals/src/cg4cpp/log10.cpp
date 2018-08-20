/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/log10.hpp>

namespace Cg {

float log10(float v)
{
    return log10<float>(v);
}

float1 log10(float1 v)
{
    return log10<float,1>(v);
}

float2 log10(float2 v)
{
    return log10<float,2>(v);
}

float3 log10(float3 v)
{
    return log10<float,3>(v);
}

float4 log10(float4 v)
{
    return log10<float,4>(v);
}

double log10(double v)
{
    return log10<double>(v);
}

double1 log10(double1 v)
{
    return log10<double,1>(v);
}

double2 log10(double2 v)
{
    return log10<double,2>(v);
}

double3 log10(double3 v)
{
    return log10<double,3>(v);
}

double4 log10(double4 v)
{
    return log10<double,4>(v);
}

half log10(half v)
{
    return log10<half>(v);
}

half1 log10(half1 v)
{
    return log10<half,1>(v);
}

half2 log10(half2 v)
{
    return log10<half,2>(v);
}

half3 log10(half3 v)
{
    return log10<half,3>(v);
}

half4 log10(half4 v)
{
    return log10<half,4>(v);
}

fixed log10(fixed v)
{
    return log10<fixed>(v);
}

fixed1 log10(fixed1 v)
{
    return log10<fixed,1>(v);
}

fixed2 log10(fixed2 v)
{
    return log10<fixed,2>(v);
}

fixed3 log10(fixed3 v)
{
    return log10<fixed,3>(v);
}

fixed4 log10(fixed4 v)
{
    return log10<fixed,4>(v);
}

} // namespace Cg

