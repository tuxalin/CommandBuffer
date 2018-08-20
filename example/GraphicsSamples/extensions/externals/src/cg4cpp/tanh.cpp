/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/tanh.hpp>

namespace Cg {

float tanh(float v)
{
    return tanh<float,1>(v);
}

float1 tanh(float1 v)
{
    return tanh<float,1>(v);
}

float2 tanh(float2 v)
{
    return tanh<float,2>(v);
}

float3 tanh(float3 v)
{
    return tanh<float,3>(v);
}

float4 tanh(float4 v)
{
    return tanh<float,4>(v);
}

double tanh(double v)
{
    return tanh<double,1>(v);
}

double1 tanh(double1 v)
{
    return tanh<double,1>(v);
}

double2 tanh(double2 v)
{
    return tanh<double,2>(v);
}

double3 tanh(double3 v)
{
    return tanh<double,3>(v);
}

double4 tanh(double4 v)
{
    return tanh<double,4>(v);
}

half tanh(half v)
{
    return tanh<half,1>(v);
}

half1 tanh(half1 v)
{
    return tanh<half,1>(v);
}

half2 tanh(half2 v)
{
    return tanh<half,2>(v);
}

half3 tanh(half3 v)
{
    return tanh<half,3>(v);
}

half4 tanh(half4 v)
{
    return tanh<half,4>(v);
}

fixed tanh(fixed v)
{
    return tanh<fixed,1>(v);
}

fixed1 tanh(fixed1 v)
{
    return tanh<fixed,1>(v);
}

fixed2 tanh(fixed2 v)
{
    return tanh<fixed,2>(v);
}

fixed3 tanh(fixed3 v)
{
    return tanh<fixed,3>(v);
}

fixed4 tanh(fixed4 v)
{
    return tanh<fixed,4>(v);
}

} // namespace Cg
