/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/faceforward.hpp>

namespace Cg {

float1 faceforward(float1 n, float1 i, float1 ng)
{
  return faceforward<float,1>(n, i, ng);
}
float2 faceforward(float2 n, float2 i, float2 ng)
{
  return faceforward<float,2>(n, i, ng);
}
float3 faceforward(float3 n, float3 i, float3 ng)
{
  return faceforward<float,3>(n, i, ng);
}
float4 faceforward(float4 n, float4 i, float4 ng)
{
  return faceforward<float,4>(n, i, ng);
}

half1 faceforward(half1 n, half1 i, half1 ng)
{
  return faceforward<half,1>(n, i, ng);
}
half2 faceforward(half2 n, half2 i, half2 ng)
{
  return faceforward<half,2>(n, i, ng);
}
half3 faceforward(half3 n, half3 i, half3 ng)
{
  return faceforward<half,3>(n, i, ng);
}
half4 faceforward(half4 n, half4 i, half4 ng)
{
  return faceforward<half,4>(n, i, ng);
}

fixed1 faceforward(fixed1 n, fixed1 i, fixed1 ng)
{
  return faceforward<fixed,1>(n, i, ng);
}
fixed2 faceforward(fixed2 n, fixed2 i, fixed2 ng)
{
  return faceforward<fixed,2>(n, i, ng);
}
fixed3 faceforward(fixed3 n, fixed3 i, fixed3 ng)
{
  return faceforward<fixed,3>(n, i, ng);
}
fixed4 faceforward(fixed4 n, fixed4 i, fixed4 ng)
{
  return faceforward<fixed,4>(n, i, ng);
}

double1 faceforward(double1 n, double1 i, double1 ng)
{
  return faceforward<double,1>(n, i, ng);
}
double2 faceforward(double2 n, double2 i, double2 ng)
{
  return faceforward<double,2>(n, i, ng);
}
double3 faceforward(double3 n, double3 i, double3 ng)
{
  return faceforward<double,3>(n, i, ng);
}
double4 faceforward(double4 n, double4 i, double4 ng)
{
  return faceforward<double,4>(n, i, ng);
}

} // namespace Cg

