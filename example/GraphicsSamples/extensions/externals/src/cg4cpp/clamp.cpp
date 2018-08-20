/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/clamp.hpp>

namespace Cg {

int1 clamp(int1 a, int1 b, int1 x)
{
  return clamp<int,1>(a, b, x);
}
int2 clamp(int2 a, int2 b, int2 x)
{
  return clamp<int,2>(a, b, x);
}
int3 clamp(int3 a, int3 b, int3 x)
{
  return clamp<int,3>(a, b, x);
}
int4 clamp(int4 a, int4 b, int4 x)
{
  return clamp<int,4>(a, b, x);
}

float1 clamp(float1 a, float1 b, float1 x)
{
  return clamp<float,1>(a, b, x);
}
float2 clamp(float2 a, float2 b, float2 x)
{
  return clamp<float,2>(a, b, x);
}
float3 clamp(float3 a, float3 b, float3 x)
{
  return clamp<float,3>(a, b, x);
}
float4 clamp(float4 a, float4 b, float4 x)
{
  return clamp<float,4>(a, b, x);
}

half1 clamp(half1 a, half1 b, half1 x)
{
  return clamp<half,1>(a, b, x);
}
half2 clamp(half2 a, half2 b, half2 x)
{
  return clamp<half,2>(a, b, x);
}
half3 clamp(half3 a, half3 b, half3 x)
{
  return clamp<half,3>(a, b, x);
}
half4 clamp(half4 a, half4 b, half4 x)
{
  return clamp<half,4>(a, b, x);
}

fixed1 clamp(fixed1 a, fixed1 b, fixed1 x)
{
  return clamp<fixed,1>(a, b, x);
}
fixed2 clamp(fixed2 a, fixed2 b, fixed2 x)
{
  return clamp<fixed,2>(a, b, x);
}
fixed3 clamp(fixed3 a, fixed3 b, fixed3 x)
{
  return clamp<fixed,3>(a, b, x);
}
fixed4 clamp(fixed4 a, fixed4 b, fixed4 x)
{
  return clamp<fixed,4>(a, b, x);
}

double1 clamp(double1 a, double1 b, double1 x)
{
  return clamp<double,1>(a, b, x);
}
double2 clamp(double2 a, double2 b, double2 x)
{
  return clamp<double,2>(a, b, x);
}
double3 clamp(double3 a, double3 b, double3 x)
{
  return clamp<double,3>(a, b, x);
}
double4 clamp(double4 a, double4 b, double4 x)
{
  return clamp<double,4>(a, b, x);
}

} // namespace Cg

