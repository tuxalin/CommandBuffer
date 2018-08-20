/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/exp.hpp>

namespace Cg {

float exp(float v)
{
    return exp<float,1>(v);
}

float1 exp(float1 v)
{
    return exp<float,1>(v);
}

float2 exp(float2 v)
{
    return exp<float,2>(v);
}

float3 exp(float3 v)
{
    return exp<float,3>(v);
}

float4 exp(float4 v)
{
    return exp<float,4>(v);
}

double exp(double v)
{
    return exp<double,1>(v);
}

double1 exp(double1 v)
{
    return exp<double,1>(v);
}

double2 exp(double2 v)
{
    return exp<double,2>(v);
}

double3 exp(double3 v)
{
    return exp<double,3>(v);
}

double4 exp(double4 v)
{
    return exp<double,4>(v);
}

half exp(half v)
{
    return exp<half,1>(v);
}

half1 exp(half1 v)
{
    return exp<half,1>(v);
}

half2 exp(half2 v)
{
    return exp<half,2>(v);
}

half3 exp(half3 v)
{
    return exp<half,3>(v);
}

half4 exp(half4 v)
{
    return exp<half,4>(v);
}

fixed exp(fixed v)
{
    return exp<fixed,1>(v);
}

fixed1 exp(fixed1 v)
{
    return exp<fixed,1>(v);
}

fixed2 exp(fixed2 v)
{
    return exp<fixed,2>(v);
}

fixed3 exp(fixed3 v)
{
    return exp<fixed,3>(v);
}

fixed4 exp(fixed4 v)
{
    return exp<fixed,4>(v);
}

} // namespace Cg
