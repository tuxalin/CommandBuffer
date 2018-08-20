/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/tan.hpp>

namespace Cg {

float tan(float v)
{
    return tan<float,1>(v);
}

float1 tan(float1 v)
{
    return tan<float,1>(v);
}

float2 tan(float2 v)
{
    return tan<float,2>(v);
}

float3 tan(float3 v)
{
    return tan<float,3>(v);
}

float4 tan(float4 v)
{
    return tan<float,4>(v);
}

double tan(double v)
{
    return tan<double,1>(v);
}

double1 tan(double1 v)
{
    return tan<double,1>(v);
}

double2 tan(double2 v)
{
    return tan<double,2>(v);
}

double3 tan(double3 v)
{
    return tan<double,3>(v);
}

double4 tan(double4 v)
{
    return tan<double,4>(v);
}

half tan(half v)
{
    return tan<half,1>(v);
}

half1 tan(half1 v)
{
    return tan<half,1>(v);
}

half2 tan(half2 v)
{
    return tan<half,2>(v);
}

half3 tan(half3 v)
{
    return tan<half,3>(v);
}

half4 tan(half4 v)
{
    return tan<half,4>(v);
}

fixed tan(fixed v)
{
    return tan<fixed,1>(v);
}

fixed1 tan(fixed1 v)
{
    return tan<fixed,1>(v);
}

fixed2 tan(fixed2 v)
{
    return tan<fixed,2>(v);
}

fixed3 tan(fixed3 v)
{
    return tan<fixed,3>(v);
}

fixed4 tan(fixed4 v)
{
    return tan<fixed,4>(v);
}

} // namespace Cg
