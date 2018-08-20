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
#include <Cg/stdlib.hpp>

#include <cmath> // for std::exp

namespace Cg {

template <typename T, int N>
static inline __CGvector<T,N> modf(__CGvector<T,N> x, __CGvector<T,N>& ipOut)
{
    __CGvector<T,N> rv;

    for (int i=0; i<N; i++) {
        typename __CGtype_trait<T>::dotType ipVal;  // float except double when T is double
        rv[i] = T(std::modf(x[i], &ipVal));
        ipOut[i] = T(ipVal);
    }
    return rv;
}

float1 modf(float1 x, Out<float1> ipOut)
{
    return modf<float,1>(x, ipOut);
}

float2 modf(float2 x, Out<float2> ipOut)
{
    return modf<float,2>(x, ipOut);
}

float3 modf(float3 x, Out<float3> ipOut)
{
    return modf<float,3>(x, ipOut);
}

float4 modf(float4 x, Out<float4> ipOut)
{
    return modf<float,4>(x, ipOut);
}

double1 modf(double1 x, Out<double1> ipOut)
{
    return modf<double,1>(x, ipOut);
}

double2 modf(double2 x, Out<double2> ipOut)
{
    return modf<double,2>(x, ipOut);
}

double3 modf(double3 x, Out<double3> ipOut)
{
    return modf<double,3>(x, ipOut);
}

double4 modf(double4 x, Out<double4> ipOut)
{
    return modf<double,4>(x, ipOut);
}

half1 modf(half1 x, Out<half1> ipOut)
{
    return modf<half,1>(x, ipOut);
}

half2 modf(half2 x, Out<half2> ipOut)
{
    return modf<half,2>(x, ipOut);
}

half3 modf(half3 x, Out<half3> ipOut)
{
    return modf<half,3>(x, ipOut);
}

half4 modf(half4 x, Out<half4> ipOut)
{
    return modf<half,4>(x, ipOut);
}

fixed1 modf(fixed1 x, Out<fixed1> ipOut)
{
    return modf<fixed,1>(x, ipOut);
}

fixed2 modf(fixed2 x, Out<fixed2> ipOut)
{
    return modf<fixed,2>(x, ipOut);
}

fixed3 modf(fixed3 x, Out<fixed3> ipOut)
{
    return modf<fixed,3>(x, ipOut);
}

fixed4 modf(fixed4 x, Out<fixed4> ipOut)
{
    return modf<fixed,4>(x, ipOut);
}

} // namespace Cg

