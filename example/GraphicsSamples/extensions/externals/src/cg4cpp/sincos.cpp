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

#include <cmath> // for std::sin & std::cos

namespace Cg {

template <typename T, int N>
static inline void sincos(__CGvector<T,N> v, __CGvector<T,N> &s, __CGvector<T,N> &c)
{
    for (int i=0; i<N; i++) {
        s[i] = std::sin(v[i]);
        c[i] = std::cos(v[i]);
    }
}

void sincos(float1 v, Out<float1> s, Out<float1> c)
{
    sincos<float,1>(v, s, c);
}

void sincos(float2 v, Out<float2> s, Out<float2> c)
{
    sincos<float,2>(v, s, c);
}

void sincos(float3 v, Out<float3> s, Out<float3> c)
{
    sincos<float,3>(v, s, c);
}

void sincos(float4 v, Out<float4> s, Out<float4> c)
{
    sincos<float,4>(v, s, c);
}

void sincos(double1 v, Out<double1> s, Out<double1> c)
{
    sincos<double,1>(v, s, c);
}

void sincos(double2 v, Out<double2> s, Out<double2> c)
{
    sincos<double,2>(v, s, c);
}

void sincos(double3 v, Out<double3> s, Out<double3> c)
{
    sincos<double,3>(v, s, c);
}

void sincos(double4 v, Out<double4> s, Out<double4> c)
{
    sincos<double,4>(v, s, c);
}

void sincos(half1 v, Out<half1> s, Out<half1> c)
{
    sincos<half,1>(v, s, c);
}

void sincos(half2 v, Out<half2> s, Out<half2> c)
{
    sincos<half,2>(v, s, c);
}

void sincos(half3 v, Out<half3> s, Out<half3> c)
{
    sincos<half,3>(v, s, c);
}

void sincos(half4 v, Out<half4> s, Out<half4> c)
{
    sincos<half,4>(v, s, c);
}

void sincos(fixed1 v, Out<fixed1> s, Out<fixed1> c)
{
    sincos<fixed,1>(v, s, c);
}

void sincos(fixed2 v, Out<fixed2> s, Out<fixed2> c)
{
    sincos<fixed,2>(v, s, c);
}

void sincos(fixed3 v, Out<fixed3> s, Out<fixed3> c)
{
    sincos<fixed,3>(v, s, c);
}

void sincos(fixed4 v, Out<fixed4> s, Out<fixed4> c)
{
    sincos<fixed,4>(v, s, c);
}

} // namespace Cg

