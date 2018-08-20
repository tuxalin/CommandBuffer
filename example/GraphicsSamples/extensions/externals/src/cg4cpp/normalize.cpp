/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/normalize.hpp>

namespace Cg {

float1 normalize(float1 v)
{
    return normalize<float,1>(v);
}

float2 normalize(float2 v)
{
    return normalize<float,2>(v);
}

float3 normalize(float3 v)
{
    return normalize<float,3>(v);
}

float4 normalize(float4 v)
{
    return normalize<float,4>(v);
}

double1 normalize(double1 v)
{
    return normalize<double,1>(v);
}

double2 normalize(double2 v)
{
    return normalize<double,2>(v);
}

double3 normalize(double3 v)
{
    return normalize<double,3>(v);
}

double4 normalize(double4 v)
{
    return normalize<double,4>(v);
}

half1 normalize(half1 v)
{
    return normalize<half,1>(v);
}

half2 normalize(half2 v)
{
    return normalize<half,2>(v);
}

half3 normalize(half3 v)
{
    return normalize<half,3>(v);
}

half4 normalize(half4 v)
{
    return normalize<half,4>(v);
}

fixed1 normalize(fixed1 v)
{
    return normalize<fixed,1>(v);
}

fixed2 normalize(fixed2 v)
{
    return normalize<fixed,2>(v);
}

fixed3 normalize(fixed3 v)
{
    return normalize<fixed,3>(v);
}

fixed4 normalize(fixed4 v)
{
    return normalize<fixed,4>(v);
}

} // namespace Cg

