/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/degrees.hpp>

namespace Cg {

float degrees(float v)
{
    return degrees<float,1>(v);
}

float1 degrees(float1 v)
{
    return degrees<float,1>(v);
}

float2 degrees(float2 v)
{
    return degrees<float,2>(v);
}

float3 degrees(float3 v)
{
    return degrees<float,3>(v);
}

float4 degrees(float4 v)
{
    return degrees<float,4>(v);
}

double degrees(double v)
{
    return degrees<double,1>(v);
}

double1 degrees(double1 v)
{
    return degrees<double,1>(v);
}

double2 degrees(double2 v)
{
    return degrees<double,2>(v);
}

double3 degrees(double3 v)
{
    return degrees<double,3>(v);
}

double4 degrees(double4 v)
{
    return degrees<double,4>(v);
}

half degrees(half v)
{
    return degrees<half,1>(v);
}

half1 degrees(half1 v)
{
    return degrees<half,1>(v);
}

half2 degrees(half2 v)
{
    return degrees<half,2>(v);
}

half3 degrees(half3 v)
{
    return degrees<half,3>(v);
}

half4 degrees(half4 v)
{
    return degrees<half,4>(v);
}

fixed degrees(fixed v)
{
    return degrees<fixed,1>(v);
}

fixed1 degrees(fixed1 v)
{
    return degrees<fixed,1>(v);
}

fixed2 degrees(fixed2 v)
{
    return degrees<fixed,2>(v);
}

fixed3 degrees(fixed3 v)
{
    return degrees<fixed,3>(v);
}

fixed4 degrees(fixed4 v)
{
    return degrees<fixed,4>(v);
}

} // namespace Cg

