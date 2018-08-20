/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/acos.hpp>

namespace Cg {

float acos(float v)
{
    return acos<float>(v);
}

float1 acos(float1 v)
{
    return acos<float,1>(v);
}

float2 acos(float2 v)
{
    return acos<float,2>(v);
}

float3 acos(float3 v)
{
    return acos<float,3>(v);
}

float4 acos(float4 v)
{
    return acos<float,4>(v);
}

double acos(double v)
{
    return acos<double>(v);
}

double1 acos(double1 v)
{
    return acos<double,1>(v);
}

double2 acos(double2 v)
{
    return acos<double,2>(v);
}

double3 acos(double3 v)
{
    return acos<double,3>(v);
}

double4 acos(double4 v)
{
    return acos<double,4>(v);
}

half acos(half v)
{
    return acos<half>(v);
}

half1 acos(half1 v)
{
    return acos<half,1>(v);
}

half2 acos(half2 v)
{
    return acos<half,2>(v);
}

half3 acos(half3 v)
{
    return acos<half,3>(v);
}

half4 acos(half4 v)
{
    return acos<half,4>(v);
}

fixed acos(fixed v)
{
    return acos<fixed>(v);
}

fixed1 acos(fixed1 v)
{
    return acos<fixed,1>(v);
}

fixed2 acos(fixed2 v)
{
    return acos<fixed,2>(v);
}

fixed3 acos(fixed3 v)
{
    return acos<fixed,3>(v);
}

fixed4 acos(fixed4 v)
{
    return acos<fixed,4>(v);
}

} // namespace Cg
