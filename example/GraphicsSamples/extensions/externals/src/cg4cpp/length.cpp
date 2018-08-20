/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/length.hpp>

namespace Cg {

float1 length(float1 v)
{
    return length<float,1>(v);
}

float1 length(float2 v)
{
    return length<float,2>(v);
}

float1 length(float3 v)
{
    return length<float,3>(v);
}

float1 length(float4 v)
{
    return length<float,4>(v);
}

double1 length(double1 v)
{
    return v;
}

double1 length(double2 v)
{
    return length<double,2>(v);
}

double1 length(double3 v)
{
    return length<double,3>(v);
}

double1 length(double4 v)
{
    return length<double,4>(v);
}

half1 length(half1 v)
{
    return v;
}

half1 length(half2 v)
{
    return length<half,2>(v);
}

half1 length(half3 v)
{
    return length<half,3>(v);
}

half1 length(half4 v)
{
    return length<half,4>(v);
}

fixed1 length(fixed1 v)
{
    return v;
}

fixed1 length(fixed2 v)
{
    return length<fixed,2>(v);
}

fixed1 length(fixed3 v)
{
    return length<fixed,3>(v);
}

fixed1 length(fixed4 v)
{
    return length<fixed,4>(v);
}

} // namespace Cg

