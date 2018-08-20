/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/sinh.hpp>

namespace Cg {

float sinh(float v)
{
    return sinh<float>(v);
}
float1 sinh(float1 v)
{
    return sinh<float,1>(v);
}

float2 sinh(float2 v)
{
    return sinh<float,2>(v);
}

float3 sinh(float3 v)
{
    return sinh<float,3>(v);
}

float4 sinh(float4 v)
{
    return sinh<float,4>(v);
}

double1 sinh(double1 v)
{
    return sinh<double,1>(v);
}

double2 sinh(double2 v)
{
    return sinh<double,2>(v);
}

double3 sinh(double3 v)
{
    return sinh<double,3>(v);
}

double4 sinh(double4 v)
{
    return sinh<double,4>(v);
}

half sinh(half v)
{
    return sinh<half>(v);
}
half1 sinh(half1 v)
{
    return sinh<half,1>(v);
}

half2 sinh(half2 v)
{
    return sinh<half,2>(v);
}

half3 sinh(half3 v)
{
    return sinh<half,3>(v);
}

half4 sinh(half4 v)
{
    return sinh<half,4>(v);
}

fixed1 sinh(fixed1 v)
{
    return sinh<fixed,1>(v);
}

fixed2 sinh(fixed2 v)
{
    return sinh<fixed,2>(v);
}

fixed3 sinh(fixed3 v)
{
    return sinh<fixed,3>(v);
}

fixed4 sinh(fixed4 v)
{
    return sinh<fixed,4>(v);
}

} // namespace Cg
