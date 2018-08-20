/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/stdlib.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGvector<T,N> ldexp(const __CGvector<T,N> & x, const __CGvector<T,N> & n)
{
    // XXX better optimize; std::ldexp not appropriate
    return x * exp2(n);
}

float1 ldexp(float1 x, float1 n)
{
    return ldexp<float,1>(x, n);;
}

float2 ldexp(float2 x, float2 n)
{
    return ldexp<float,2>(x, n);
}

float3 ldexp(float3 x, float3 n)
{
    return ldexp<float,3>(x, n);
}

float4 ldexp(float4 x, float4 n)
{
    return ldexp<float,4>(x, n);
}

double1 ldexp(double1 x, double1 n)
{
    return ldexp<double,1>(x, n);
}

double2 ldexp(double2 x, double2 n)
{
    return ldexp<double,2>(x, n);
}

double3 ldexp(double3 x, double3 n)
{
    return ldexp<double,3>(x, n);
}

double4 ldexp(double4 x, double4 n)
{
    return ldexp<double,4>(x, n);
}

half1 ldexp(half1 x, half1 n)
{
    return ldexp<half,1>(x, n);
}

half2 ldexp(half2 x, half2 n)
{
    return ldexp<half,2>(x, n);
}

half3 ldexp(half3 x, half3 n)
{
    return ldexp<half,3>(x, n);
}

half4 ldexp(half4 x, half4 n)
{
    return ldexp<half,4>(x, n);
}

fixed1 ldexp(fixed1 x, fixed1 n)
{
    return ldexp<fixed,1>(x, n);
}

fixed2 ldexp(fixed2 x, fixed2 n)
{
    return ldexp<fixed,2>(x, n);
}

fixed3 ldexp(fixed3 x, fixed3 n)
{
    return ldexp<fixed,3>(x, n);
}

fixed4 ldexp(fixed4 x, fixed4 n)
{
    return ldexp<fixed,4>(x, n);
}

} // namespace Cg

