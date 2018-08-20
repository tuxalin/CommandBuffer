/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/asin.hpp>

namespace Cg {

float asin(float v)
{
    return asin<float>(v);
}

float1 asin(float1 v)
{
    return asin<float,1>(v);
}

float2 asin(float2 v)
{
    return asin<float,2>(v);
}

float3 asin(float3 v)
{
    return asin<float,3>(v);
}

float4 asin(float4 v)
{
    return asin<float,4>(v);
}

double asin(double v)
{
    return asin<double>(v);
}

double1 asin(double1 v)
{
    return asin<double,1>(v);
}

double2 asin(double2 v)
{
    return asin<double,2>(v);
}

double3 asin(double3 v)
{
    return asin<double,3>(v);
}

double4 asin(double4 v)
{
    return asin<double,4>(v);
}

half asin(half v)
{
    return asin<half>(v);
}

half1 asin(half1 v)
{
    return asin<half,1>(v);
}

half2 asin(half2 v)
{
    return asin<half,2>(v);
}

half3 asin(half3 v)
{
    return asin<half,3>(v);
}

half4 asin(half4 v)
{
    return asin<half,4>(v);
}

fixed asin(fixed v)
{
    return asin<fixed>(v);
}

fixed1 asin(fixed1 v)
{
    return asin<fixed,1>(v);
}

fixed2 asin(fixed2 v)
{
    return asin<fixed,2>(v);
}

fixed3 asin(fixed3 v)
{
    return asin<fixed,3>(v);
}

fixed4 asin(fixed4 v)
{
    return asin<fixed,4>(v);
}

} // namespace Cg

