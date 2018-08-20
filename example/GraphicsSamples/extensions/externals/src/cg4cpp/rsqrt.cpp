/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/rsqrt.hpp>

namespace Cg {

float rsqrt(float v)
{
    return rsqrt<float,1>(v);
}

float1 rsqrt(float1 v)
{
    return rsqrt<float,1>(v);
}

float2 rsqrt(float2 v)
{
    return rsqrt<float,2>(v);
}

float3 rsqrt(float3 v)
{
    return rsqrt<float,3>(v);
}

float4 rsqrt(float4 v)
{
    return rsqrt<float,4>(v);
}

double rsqrt(double v)
{
    return rsqrt<double,1>(v);
}

double1 rsqrt(double1 v)
{
    return rsqrt<double,1>(v);
}

double2 rsqrt(double2 v)
{
    return rsqrt<double,2>(v);
}

double3 rsqrt(double3 v)
{
    return rsqrt<double,3>(v);
}

double4 rsqrt(double4 v)
{
    return rsqrt<double,4>(v);
}

half rsqrt(half v)
{
    return rsqrt<half,1>(v);
}

half1 rsqrt(half1 v)
{
    return rsqrt<half,1>(v);
}

half2 rsqrt(half2 v)
{
    return rsqrt<half,2>(v);
}

half3 rsqrt(half3 v)
{
    return rsqrt<half,3>(v);
}

half4 rsqrt(half4 v)
{
    return rsqrt<half,4>(v);
}

fixed rsqrt(fixed v)
{
    return rsqrt<fixed,1>(v);
}

fixed1 rsqrt(fixed1 v)
{
    return rsqrt<fixed,1>(v);
}

fixed2 rsqrt(fixed2 v)
{
    return rsqrt<fixed,2>(v);
}

fixed3 rsqrt(fixed3 v)
{
    return rsqrt<fixed,3>(v);
}

fixed4 rsqrt(fixed4 v)
{
    return rsqrt<fixed,4>(v);
}

} // namespace Cg
