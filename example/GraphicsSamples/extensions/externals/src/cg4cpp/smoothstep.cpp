/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/smoothstep.hpp>

namespace Cg {

float1 smoothstep(float1 a, float1 b, float1 x)
{
  return smoothstep<float,1>(a, b, x);
}
float2 smoothstep(float2 a, float2 b, float2 x)
{
  return smoothstep<float,2>(a, b, x);
}
float3 smoothstep(float3 a, float3 b, float3 x)
{
  return smoothstep<float,3>(a, b, x);
}
float4 smoothstep(float4 a, float4 b, float4 x)
{
  return smoothstep<float,4>(a, b, x);
}

half1 smoothstep(half1 a, half1 b, half1 x)
{
  return smoothstep<half,1>(a, b, x);
}
half2 smoothstep(half2 a, half2 b, half2 x)
{
  return smoothstep<half,2>(a, b, x);
}
half3 smoothstep(half3 a, half3 b, half3 x)
{
  return smoothstep<half,3>(a, b, x);
}
half4 smoothstep(half4 a, half4 b, half4 x)
{
  return smoothstep<half,4>(a, b, x);
}

fixed1 smoothstep(fixed1 a, fixed1 b, fixed1 x)
{
  return smoothstep<fixed,1>(a, b, x);
}
fixed2 smoothstep(fixed2 a, fixed2 b, fixed2 x)
{
  return smoothstep<fixed,2>(a, b, x);
}
fixed3 smoothstep(fixed3 a, fixed3 b, fixed3 x)
{
  return smoothstep<fixed,3>(a, b, x);
}
fixed4 smoothstep(fixed4 a, fixed4 b, fixed4 x)
{
  return smoothstep<fixed,4>(a, b, x);
}

double1 smoothstep(double1 a, double1 b, double1 x)
{
  return smoothstep<double,1>(a, b, x);
}
double2 smoothstep(double2 a, double2 b, double2 x)
{
  return smoothstep<double,2>(a, b, x);
}
double3 smoothstep(double3 a, double3 b, double3 x)
{
  return smoothstep<double,3>(a, b, x);
}
double4 smoothstep(double4 a, double4 b, double4 x)
{
  return smoothstep<double,4>(a, b, x);
}

} // namespace Cg

