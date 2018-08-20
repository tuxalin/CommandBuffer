/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/inout.hpp>
#include <Cg/frexp.hpp>

#include <cmath> // for std::exp

namespace Cg {

float1 frexp(float1 x, Out<float1> expOut)
{
    return frexp<float,1>(x, expOut);
}

float2 frexp(float2 x, Out<float2> expOut)
{
    return frexp<float,2>(x, expOut);
}

float3 frexp(float3 x, Out<float3> expOut)
{
    return frexp<float,3>(x, expOut);
}

float4 frexp(float4 x, Out<float4> expOut)
{
    return frexp<float,4>(x, expOut);
}

double1 frexp(double1 x, Out<double1> expOut)
{
    return frexp<double,1>(x, expOut);
}

double2 frexp(double2 x, Out<double2> expOut)
{
    return frexp<double,2>(x, expOut);
}

double3 frexp(double3 x, Out<double3> expOut)
{
    return frexp<double,3>(x, expOut);
}

double4 frexp(double4 x, Out<double4> expOut)
{
    return frexp<double,4>(x, expOut);
}

half1 frexp(half1 x, Out<half1> expOut)
{
    return frexp<half,1>(x, expOut);
}

half2 frexp(half2 x, Out<half2> expOut)
{
    return frexp<half,2>(x, expOut);
}

half3 frexp(half3 x, Out<half3> expOut)
{
    return frexp<half,3>(x, expOut);
}

half4 frexp(half4 x, Out<half4> expOut)
{
    return frexp<half,4>(x, expOut);
}

fixed1 frexp(fixed1 x, Out<fixed1> expOut)
{
    return frexp<fixed,1>(x, expOut);
}

fixed2 frexp(fixed2 x, Out<fixed2> expOut)
{
    return frexp<fixed,2>(x, expOut);
}

fixed3 frexp(fixed3 x, Out<fixed3> expOut)
{
    return frexp<fixed,3>(x, expOut);
}

fixed4 frexp(fixed4 x, Out<fixed4> expOut)
{
    return frexp<fixed,4>(x, expOut);
}

} // namespace Cg

