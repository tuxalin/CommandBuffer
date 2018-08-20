/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/lerp.hpp>

namespace Cg {

float1 lerp(float1 a, float1 b, float1 t)
{
  return lerp<float,1>(a, b, t);
}
float2 lerp(float2 a, float2 b, float2 t)
{
  return lerp<float,2>(a, b, t);
}
float3 lerp(float3 a, float3 b, float3 t)
{
  return lerp<float,3>(a, b, t);
}
float4 lerp(float4 a, float4 b, float4 t)
{
  return lerp<float,4>(a, b, t);
}

int1 lerp(int1 a, int1 b, int1 t)
{
  return lerp<int,1>(a, b, t);
}
int2 lerp(int2 a, int2 b, int2 t)
{
  return lerp<int,2>(a, b, t);
}
int3 lerp(int3 a, int3 b, int3 t)
{
  return lerp<int,3>(a, b, t);
}
int4 lerp(int4 a, int4 b, int4 t)
{
  return lerp<int,4>(a, b, t);
}

half1 lerp(half1 a, half1 b, half1 t)
{
  return lerp<half,1>(a, b, t);
}
half2 lerp(half2 a, half2 b, half2 t)
{
  return lerp<half,2>(a, b, t);
}
half3 lerp(half3 a, half3 b, half3 t)
{
  return lerp<half,3>(a, b, t);
}
half4 lerp(half4 a, half4 b, half4 t)
{
  return lerp<half,4>(a, b, t);
}

fixed1 lerp(fixed1 a, fixed1 b, fixed1 t)
{
  return lerp<fixed,1>(a, b, t);
}
fixed2 lerp(fixed2 a, fixed2 b, fixed2 t)
{
  return lerp<fixed,2>(a, b, t);
}
fixed3 lerp(fixed3 a, fixed3 b, fixed3 t)
{
  return lerp<fixed,3>(a, b, t);
}
fixed4 lerp(fixed4 a, fixed4 b, fixed4 t)
{
  return lerp<fixed,4>(a, b, t);
}

double1 lerp(double1 a, double1 b, double1 t)
{
  return lerp<double,1>(a, b, t);
}
double2 lerp(double2 a, double2 b, double2 t)
{
  return lerp<double,2>(a, b, t);
}
double3 lerp(double3 a, double3 b, double3 t)
{
  return lerp<double,3>(a, b, t);
}
double4 lerp(double4 a, double4 b, double4 t)
{
  return lerp<double,4>(a, b, t);
}

} // namespace Cg

