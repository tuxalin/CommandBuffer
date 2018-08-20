/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/log.hpp>

namespace Cg {

float log(float v)
{
    return log<float>(v);
}

float1 log(float1 v)
{
    return log<float,1>(v);
}

float2 log(float2 v)
{
    return log<float,2>(v);
}

float3 log(float3 v)
{
    return log<float,3>(v);
}

float4 log(float4 v)
{
    return log<float,4>(v);
}

double log(double v)
{
    return log<double>(v);
}

double1 log(double1 v)
{
    return log<double,1>(v);
}

double2 log(double2 v)
{
    return log<double,2>(v);
}

double3 log(double3 v)
{
    return log<double,3>(v);
}

double4 log(double4 v)
{
    return log<double,4>(v);
}

half log(half v)
{
    return log<half>(v);
}

half1 log(half1 v)
{
    return log<half,1>(v);
}

half2 log(half2 v)
{
    return log<half,2>(v);
}

half3 log(half3 v)
{
    return log<half,3>(v);
}

half4 log(half4 v)
{
    return log<half,4>(v);
}

fixed log(fixed v)
{
    return log<fixed>(v);
}

fixed1 log(fixed1 v)
{
    return log<fixed,1>(v);
}

fixed2 log(fixed2 v)
{
    return log<fixed,2>(v);
}

fixed3 log(fixed3 v)
{
    return log<fixed,3>(v);
}

fixed4 log(fixed4 v)
{
    return log<fixed,4>(v);
}

} // namespace Cg
