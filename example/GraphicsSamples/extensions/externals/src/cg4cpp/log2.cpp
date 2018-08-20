/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/log2.hpp>

namespace Cg {

float log2(float v)
{
    return log2<float>(v);
}

float1 log2(float1 v)
{
    return log2<float,1>(v);
}

float2 log2(float2 v)
{
    return log2<float,2>(v);
}

float3 log2(float3 v)
{
    return log2<float,3>(v);
}

float4 log2(float4 v)
{
    return log2<float,4>(v);
}

double log2(double v)
{
    return log2<double>(v);
}

double1 log2(double1 v)
{
    return log2<double,1>(v);
}

double2 log2(double2 v)
{
    return log2<double,2>(v);
}

double3 log2(double3 v)
{
    return log2<double,3>(v);
}

double4 log2(double4 v)
{
    return log2<double,4>(v);
}

half log2(half v)
{
    return log2<half>(v);
}

half1 log2(half1 v)
{
    return log2<half,1>(v);
}

half2 log2(half2 v)
{
    return log2<half,2>(v);
}

half3 log2(half3 v)
{
    return log2<half,3>(v);
}

half4 log2(half4 v)
{
    return log2<half,4>(v);
}

fixed log2(fixed v)
{
    return log2<fixed>(v);
}

fixed1 log2(fixed1 v)
{
    return log2<fixed,1>(v);
}

fixed2 log2(fixed2 v)
{
    return log2<fixed,2>(v);
}

fixed3 log2(fixed3 v)
{
    return log2<fixed,3>(v);
}

fixed4 log2(fixed4 v)
{
    return log2<fixed,4>(v);
}

} // namespace Cg

