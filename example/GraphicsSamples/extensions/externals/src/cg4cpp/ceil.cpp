/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/ceil.hpp>

namespace Cg {

float ceil(float v)
{
    return ceil<float>(v);
}

float1 ceil(float1 v)
{
    return ceil<float,1>(v);
}

float2 ceil(float2 v)
{
    return ceil<float,2>(v);
}

float3 ceil(float3 v)
{
    return ceil<float,3>(v);
}

float4 ceil(float4 v)
{
    return ceil<float,4>(v);
}

double ceil(double v)
{
    return ceil<double>(v);
}

double1 ceil(double1 v)
{
    return ceil<double,1>(v);
}

double2 ceil(double2 v)
{
    return ceil<double,2>(v);
}

double3 ceil(double3 v)
{
    return ceil<double,3>(v);
}

double4 ceil(double4 v)
{
    return ceil<double,4>(v);
}

half ceil(half v)
{
    return ceil<half>(v);
}

half1 ceil(half1 v)
{
    return ceil<half,1>(v);
}

half2 ceil(half2 v)
{
    return ceil<half,2>(v);
}

half3 ceil(half3 v)
{
    return ceil<half,3>(v);
}

half4 ceil(half4 v)
{
    return ceil<half,4>(v);
}

fixed ceil(fixed v)
{
    return ceil<fixed>(v);
}

fixed1 ceil(fixed1 v)
{
    return ceil<fixed,1>(v);
}

fixed2 ceil(fixed2 v)
{
    return ceil<fixed,2>(v);
}

fixed3 ceil(fixed3 v)
{
    return ceil<fixed,3>(v);
}

fixed4 ceil(fixed4 v)
{
    return ceil<fixed,4>(v);
}

} // namespace Cg
