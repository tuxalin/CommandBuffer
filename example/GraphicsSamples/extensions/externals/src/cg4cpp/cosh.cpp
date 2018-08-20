/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/cosh.hpp>

namespace Cg {

float cosh(float v)
{
    return cosh<float>(v);
}

float1 cosh(float1 v)
{
    return cosh<float,1>(v);
}

float2 cosh(float2 v)
{
    return cosh<float,2>(v);
}

float3 cosh(float3 v)
{
    return cosh<float,3>(v);
}

float4 cosh(float4 v)
{
    return cosh<float,4>(v);
}

double cosh(double v)
{
    return cosh<double>(v);
}

double1 cosh(double1 v)
{
    return cosh<double,1>(v);
}

double2 cosh(double2 v)
{
    return cosh<double,2>(v);
}

double3 cosh(double3 v)
{
    return cosh<double,3>(v);
}

double4 cosh(double4 v)
{
    return cosh<double,4>(v);
}

half cosh(half v)
{
    return cosh<half>(v);
}

half1 cosh(half1 v)
{
    return cosh<half,1>(v);
}

half2 cosh(half2 v)
{
    return cosh<half,2>(v);
}

half3 cosh(half3 v)
{
    return cosh<half,3>(v);
}

half4 cosh(half4 v)
{
    return cosh<half,4>(v);
}

fixed cosh(fixed v)
{
    return cosh<fixed>(v);
}

fixed1 cosh(fixed1 v)
{
    return cosh<fixed,1>(v);
}

fixed2 cosh(fixed2 v)
{
    return cosh<fixed,2>(v);
}

fixed3 cosh(fixed3 v)
{
    return cosh<fixed,3>(v);
}

fixed4 cosh(fixed4 v)
{
    return cosh<fixed,4>(v);
}

} // namespace Cg
