/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/round.hpp>

namespace Cg {

float round(float v)
{
    return round<float>(v);
}

float1 round(float1 v)
{
    return round<float,1>(v);
}

float2 round(float2 v)
{
    return round<float,2>(v);
}

float3 round(float3 v)
{
    return round<float,3>(v);
}

float4 round(float4 v)
{
    return round<float,4>(v);
}

double round(double v)
{
    return round<double>(v);
}

double1 round(double1 v)
{
    return round<double,1>(v);
}

double2 round(double2 v)
{
    return round<double,2>(v);
}

double3 round(double3 v)
{
    return round<double,3>(v);
}

double4 round(double4 v)
{
    return round<double,4>(v);
}

half round(half v)
{
    return round<half>(v);
}

half1 round(half1 v)
{
    return round<half,1>(v);
}

half2 round(half2 v)
{
    return round<half,2>(v);
}

half3 round(half3 v)
{
    return round<half,3>(v);
}

half4 round(half4 v)
{
    return round<half,4>(v);
}

fixed round(fixed v)
{
    return round<fixed>(v);
}

fixed1 round(fixed1 v)
{
    return round<fixed,1>(v);
}

fixed2 round(fixed2 v)
{
    return round<fixed,2>(v);
}

fixed3 round(fixed3 v)
{
    return round<fixed,3>(v);
}

fixed4 round(fixed4 v)
{
    return round<fixed,4>(v);
}

} // namespace Cg

