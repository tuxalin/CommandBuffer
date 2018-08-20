/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/frac.hpp>

namespace Cg {

float frac(float v)
{
    return frac<float,1>(v);
}

float1 frac(float1 v)
{
    return frac<float,1>(v);
}

float2 frac(float2 v)
{
    return frac<float,2>(v);
}

float3 frac(float3 v)
{
    return frac<float,3>(v);
}

float4 frac(float4 v)
{
    return frac<float,4>(v);
}

double frac(double v)
{
    return frac<double,1>(v);
}

double1 frac(double1 v)
{
    return frac<double,1>(v);
}

double2 frac(double2 v)
{
    return frac<double,2>(v);
}

double3 frac(double3 v)
{
    return frac<double,3>(v);
}

double4 frac(double4 v)
{
    return frac<double,4>(v);
}

half frac(half v)
{
    return frac<half,1>(v);
}

half1 frac(half1 v)
{
    return frac<half,1>(v);
}

half2 frac(half2 v)
{
    return frac<half,2>(v);
}

half3 frac(half3 v)
{
    return frac<half,3>(v);
}

half4 frac(half4 v)
{
    return frac<half,4>(v);
}

fixed frac(fixed v)
{
    return frac<fixed,1>(v);
}

fixed1 frac(fixed1 v)
{
    return frac<fixed,1>(v);
}

fixed2 frac(fixed2 v)
{
    return frac<fixed,2>(v);
}

fixed3 frac(fixed3 v)
{
    return frac<fixed,3>(v);
}

fixed4 frac(fixed4 v)
{
    return frac<fixed,4>(v);
}

} // namespace Cg
