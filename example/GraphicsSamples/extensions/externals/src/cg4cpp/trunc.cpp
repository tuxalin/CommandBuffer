/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/trunc.hpp>

namespace Cg {

float trunc(float v)
{
    return trunc<float>(v);
}

float1 trunc(float1 v)
{
    return trunc<float,1>(v);
}

float2 trunc(float2 v)
{
    return trunc<float,2>(v);
}

float3 trunc(float3 v)
{
    return trunc<float,3>(v);
}

float4 trunc(float4 v)
{
    return trunc<float,4>(v);
}

double trunc(double v)
{
    return trunc<double>(v);
}

double1 trunc(double1 v)
{
    return trunc<double,1>(v);
}

double2 trunc(double2 v)
{
    return trunc<double,2>(v);
}

double3 trunc(double3 v)
{
    return trunc<double,3>(v);
}

double4 trunc(double4 v)
{
    return trunc<double,4>(v);
}

half trunc(half v)
{
    return trunc<half>(v);
}

half1 trunc(half1 v)
{
    return trunc<half,1>(v);
}

half2 trunc(half2 v)
{
    return trunc<half,2>(v);
}

half3 trunc(half3 v)
{
    return trunc<half,3>(v);
}

half4 trunc(half4 v)
{
    return trunc<half,4>(v);
}

fixed trunc(fixed v)
{
    return trunc<fixed>(v);
}

fixed1 trunc(fixed1 v)
{
    return trunc<fixed,1>(v);
}

fixed2 trunc(fixed2 v)
{
    return trunc<fixed,2>(v);
}

fixed3 trunc(fixed3 v)
{
    return trunc<fixed,3>(v);
}

fixed4 trunc(fixed4 v)
{
    return trunc<fixed,4>(v);
}

} // namespace Cg

