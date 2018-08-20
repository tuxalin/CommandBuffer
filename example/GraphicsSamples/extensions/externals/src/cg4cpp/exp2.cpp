/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/exp2.hpp>

namespace Cg {

float exp2(float v)
{
    return exp2<float,1>(v);
}

float1 exp2(float1 v)
{
    return exp2<float,1>(v);
}

float2 exp2(float2 v)
{
    return exp2<float,2>(v);
}

float3 exp2(float3 v)
{
    return exp2<float,3>(v);
}

float4 exp2(float4 v)
{
    return exp2<float,4>(v);
}

double exp2(double v)
{
    return exp2<double,1>(v);
}

double1 exp2(double1 v)
{
    return exp2<double,1>(v);
}

double2 exp2(double2 v)
{
    return exp2<double,2>(v);
}

double3 exp2(double3 v)
{
    return exp2<double,3>(v);
}

double4 exp2(double4 v)
{
    return exp2<double,4>(v);
}

half exp2(half v)
{
    return exp2<half,1>(v);
}

half1 exp2(half1 v)
{
    return exp2<half,1>(v);
}

half2 exp2(half2 v)
{
    return exp2<half,2>(v);
}

half3 exp2(half3 v)
{
    return exp2<half,3>(v);
}

half4 exp2(half4 v)
{
    return exp2<half,4>(v);
}

fixed exp2(fixed v)
{
    return exp2<fixed,1>(v);
}

fixed1 exp2(fixed1 v)
{
    return exp2<fixed,1>(v);
}

fixed2 exp2(fixed2 v)
{
    return exp2<fixed,2>(v);
}

fixed3 exp2(fixed3 v)
{
    return exp2<fixed,3>(v);
}

fixed4 exp2(fixed4 v)
{
    return exp2<fixed,4>(v);
}

} // namespace Cg
