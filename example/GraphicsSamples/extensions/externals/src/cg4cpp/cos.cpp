/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/cos.hpp>

namespace Cg {

float cos(float v)
{
    return cos<float>(v);
}

float1 cos(float1 v)
{
    return cos<float,1>(v);
}

float2 cos(float2 v)
{
    return cos<float,2>(v);
}

float3 cos(float3 v)
{
    return cos<float,3>(v);
}

float4 cos(float4 v)
{
    return cos<float,4>(v);
}

double cos(double v)
{
    return cos<double>(v);
}

double1 cos(double1 v)
{
    return cos<double,1>(v);
}

double2 cos(double2 v)
{
    return cos<double,2>(v);
}

double3 cos(double3 v)
{
    return cos<double,3>(v);
}

double4 cos(double4 v)
{
    return cos<double,4>(v);
}

half cos(half v)
{
    return cos<half>(v);
}

half1 cos(half1 v)
{
    return cos<half,1>(v);
}

half2 cos(half2 v)
{
    return cos<half,2>(v);
}

half3 cos(half3 v)
{
    return cos<half,3>(v);
}

half4 cos(half4 v)
{
    return cos<half,4>(v);
}

fixed cos(fixed v)
{
    return cos<fixed>(v);
}

fixed1 cos(fixed1 v)
{
    return cos<fixed,1>(v);
}

fixed2 cos(fixed2 v)
{
    return cos<fixed,2>(v);
}

fixed3 cos(fixed3 v)
{
    return cos<fixed,3>(v);
}

fixed4 cos(fixed4 v)
{
    return cos<fixed,4>(v);
}

} // namespace Cg
