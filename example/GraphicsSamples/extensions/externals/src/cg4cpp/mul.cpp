/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/double.hpp>
#include <Cg/fixed.hpp>
#include <Cg/half.hpp>
#include <Cg/mul.hpp>

namespace Cg {

float1 mul(float1x1 m, float1 v)
{
    return mul<float,float,1,1>(m, v);
}
int1 mul(int1x1 m, int1 v)
{
    return mul<int,int,1,1>(m, v);
}
double1 mul(double1x1 m, double1 v)
{
    return mul<double,double,1,1>(m, v);
}
fixed1 mul(fixed1x1 m, fixed1 v)
{
    return mul<fixed,fixed,1,1>(m, v);
}
half1 mul(half1x1 m, half1 v)
{
    return mul<half,half,1,1>(m, v);
}

float2 mul(float2x1 m, float1 v)
{
    return mul<float,float,2,1>(m, v);
}
int2 mul(int2x1 m, int1 v)
{
    return mul<int,int,2,1>(m, v);
}
double2 mul(double2x1 m, double1 v)
{
    return mul<double,double,2,1>(m, v);
}
fixed2 mul(fixed2x1 m, fixed1 v)
{
    return mul<fixed,fixed,2,1>(m, v);
}
half2 mul(half2x1 m, half1 v)
{
    return mul<half,half,2,1>(m, v);
}

float3 mul(float3x1 m, float1 v)
{
    return mul<float,float,3,1>(m, v);
}
int3 mul(int3x1 m, int1 v)
{
    return mul<int,int,3,1>(m, v);
}
double3 mul(double3x1 m, double1 v)
{
    return mul<double,double,3,1>(m, v);
}
fixed3 mul(fixed3x1 m, fixed1 v)
{
    return mul<fixed,fixed,3,1>(m, v);
}
half3 mul(half3x1 m, half1 v)
{
    return mul<half,half,3,1>(m, v);
}

float4 mul(float4x1 m, float1 v)
{
    return mul<float,float,4,1>(m, v);
}
int4 mul(int4x1 m, int1 v)
{
    return mul<int,int,4,1>(m, v);
}
double4 mul(double4x1 m, double1 v)
{
    return mul<double,double,4,1>(m, v);
}
fixed4 mul(fixed4x1 m, fixed1 v)
{
    return mul<fixed,fixed,4,1>(m, v);
}
half4 mul(half4x1 m, half1 v)
{
    return mul<half,half,4,1>(m, v);
}

float1 mul(float1x2 m, float2 v)
{
    return mul<float,float,1,2>(m, v);
}
int1 mul(int1x2 m, int2 v)
{
    return mul<int,int,1,2>(m, v);
}
double1 mul(double1x2 m, double2 v)
{
    return mul<double,double,1,2>(m, v);
}
fixed1 mul(fixed1x2 m, fixed2 v)
{
    return mul<fixed,fixed,1,2>(m, v);
}
half1 mul(half1x2 m, half2 v)
{
    return mul<half,half,1,2>(m, v);
}

float2 mul(float2x2 m, float2 v)
{
    return mul<float,float,2,2>(m, v);
}
int2 mul(int2x2 m, int2 v)
{
    return mul<int,int,2,2>(m, v);
}
double2 mul(double2x2 m, double2 v)
{
    return mul<double,double,2,2>(m, v);
}
fixed2 mul(fixed2x2 m, fixed2 v)
{
    return mul<fixed,fixed,2,2>(m, v);
}
half2 mul(half2x2 m, half2 v)
{
    return mul<half,half,2,2>(m, v);
}

float3 mul(float3x2 m, float2 v)
{
    return mul<float,float,3,2>(m, v);
}
int3 mul(int3x2 m, int2 v)
{
    return mul<int,int,3,2>(m, v);
}
double3 mul(double3x2 m, double2 v)
{
    return mul<double,double,3,2>(m, v);
}
fixed3 mul(fixed3x2 m, fixed2 v)
{
    return mul<fixed,fixed,3,2>(m, v);
}
half3 mul(half3x2 m, half2 v)
{
    return mul<half,half,3,2>(m, v);
}

float4 mul(float4x2 m, float2 v)
{
    return mul<float,float,4,2>(m, v);
}
int4 mul(int4x2 m, int2 v)
{
    return mul<int,int,4,2>(m, v);
}
double4 mul(double4x2 m, double2 v)
{
    return mul<double,double,4,2>(m, v);
}
fixed4 mul(fixed4x2 m, fixed2 v)
{
    return mul<fixed,fixed,4,2>(m, v);
}
half4 mul(half4x2 m, half2 v)
{
    return mul<half,half,4,2>(m, v);
}

float1 mul(float1x3 m, float3 v)
{
    return mul<float,float,1,3>(m, v);
}
int1 mul(int1x3 m, int3 v)
{
    return mul<int,int,1,3>(m, v);
}
double1 mul(double1x3 m, double3 v)
{
    return mul<double,double,1,3>(m, v);
}
fixed1 mul(fixed1x3 m, fixed3 v)
{
    return mul<fixed,fixed,1,3>(m, v);
}
half1 mul(half1x3 m, half3 v)
{
    return mul<half,half,1,3>(m, v);
}

float2 mul(float2x3 m, float3 v)
{
    return mul<float,float,2,3>(m, v);
}
int2 mul(int2x3 m, int3 v)
{
    return mul<int,int,2,3>(m, v);
}
double2 mul(double2x3 m, double3 v)
{
    return mul<double,double,2,3>(m, v);
}
fixed2 mul(fixed2x3 m, fixed3 v)
{
    return mul<fixed,fixed,2,3>(m, v);
}
half2 mul(half2x3 m, half3 v)
{
    return mul<half,half,2,3>(m, v);
}

float3 mul(float3x3 m, float3 v)
{
    return mul<float,float,3,3>(m, v);
}
int3 mul(int3x3 m, int3 v)
{
    return mul<int,int,3,3>(m, v);
}
double3 mul(double3x3 m, double3 v)
{
    return mul<double,double,3,3>(m, v);
}
fixed3 mul(fixed3x3 m, fixed3 v)
{
    return mul<fixed,fixed,3,3>(m, v);
}
half3 mul(half3x3 m, half3 v)
{
    return mul<half,half,3,3>(m, v);
}

float4 mul(float4x3 m, float3 v)
{
    return mul<float,float,4,3>(m, v);
}
int4 mul(int4x3 m, int3 v)
{
    return mul<int,int,4,3>(m, v);
}
double4 mul(double4x3 m, double3 v)
{
    return mul<double,double,4,3>(m, v);
}
fixed4 mul(fixed4x3 m, fixed3 v)
{
    return mul<fixed,fixed,4,3>(m, v);
}
half4 mul(half4x3 m, half3 v)
{
    return mul<half,half,4,3>(m, v);
}

float1 mul(float1x4 m, float4 v)
{
    return mul<float,float,1,4>(m, v);
}
int1 mul(int1x4 m, int4 v)
{
    return mul<int,int,1,4>(m, v);
}
double1 mul(double1x4 m, double4 v)
{
    return mul<double,double,1,4>(m, v);
}
fixed1 mul(fixed1x4 m, fixed4 v)
{
    return mul<fixed,fixed,1,4>(m, v);
}
half1 mul(half1x4 m, half4 v)
{
    return mul<half,half,1,4>(m, v);
}

float2 mul(float2x4 m, float4 v)
{
    return mul<float,float,2,4>(m, v);
}
int2 mul(int2x4 m, int4 v)
{
    return mul<int,int,2,4>(m, v);
}
double2 mul(double2x4 m, double4 v)
{
    return mul<double,double,2,4>(m, v);
}
fixed2 mul(fixed2x4 m, fixed4 v)
{
    return mul<fixed,fixed,2,4>(m, v);
}
half2 mul(half2x4 m, half4 v)
{
    return mul<half,half,2,4>(m, v);
}

float3 mul(float3x4 m, float4 v)
{
    return mul<float,float,3,4>(m, v);
}
int3 mul(int3x4 m, int4 v)
{
    return mul<int,int,3,4>(m, v);
}
double3 mul(double3x4 m, double4 v)
{
    return mul<double,double,3,4>(m, v);
}
fixed3 mul(fixed3x4 m, fixed4 v)
{
    return mul<fixed,fixed,3,4>(m, v);
}
half3 mul(half3x4 m, half4 v)
{
    return mul<half,half,3,4>(m, v);
}

float4 mul(float4x4 m, float4 v)
{
    return mul<float,float,4,4>(m, v);
}
int4 mul(int4x4 m, int4 v)
{
    return mul<int,int,4,4>(m, v);
}
double4 mul(double4x4 m, double4 v)
{
    return mul<double,double,4,4>(m, v);
}
fixed4 mul(fixed4x4 m, fixed4 v)
{
    return mul<fixed,fixed,4,4>(m, v);
}
half4 mul(half4x4 m, half4 v)
{
    return mul<half,half,4,4>(m, v);
}

float1 mul(float1 v, float1x1 m)
{
    return mul<float,float,1,1>(v, m);
}
int1 mul(int1 v, int1x1 m)
{
    return mul<int,int,1,1>(v, m);
}
double1 mul(double1 v, double1x1 m)
{
    return mul<double,double,1,1>(v, m);
}
fixed1 mul(fixed1 v, fixed1x1 m)
{
    return mul<fixed,fixed,1,1>(v, m);
}
half1 mul(half1 v, half1x1 m)
{
    return mul<half,half,1,1>(v, m);
}

float2 mul(float1 v, float1x2 m)
{
    return mul<float,float,1,2>(v, m);
}
int2 mul(int1 v, int1x2 m)
{
    return mul<int,int,1,2>(v, m);
}
double2 mul(double1 v, double1x2 m)
{
    return mul<double,double,1,2>(v, m);
}
fixed2 mul(fixed1 v, fixed1x2 m)
{
    return mul<fixed,fixed,1,2>(v, m);
}
half2 mul(half1 v, half1x2 m)
{
    return mul<half,half,1,2>(v, m);
}

float3 mul(float1 v, float1x3 m)
{
    return mul<float,float,1,3>(v, m);
}
int3 mul(int1 v, int1x3 m)
{
    return mul<int,int,1,3>(v, m);
}
double3 mul(double1 v, double1x3 m)
{
    return mul<double,double,1,3>(v, m);
}
fixed3 mul(fixed1 v, fixed1x3 m)
{
    return mul<fixed,fixed,1,3>(v, m);
}
half3 mul(half1 v, half1x3 m)
{
    return mul<half,half,1,3>(v, m);
}

float4 mul(float1 v, float1x4 m)
{
    return mul<float,float,1,4>(v, m);
}
int4 mul(int1 v, int1x4 m)
{
    return mul<int,int,1,4>(v, m);
}
double4 mul(double1 v, double1x4 m)
{
    return mul<double,double,1,4>(v, m);
}
fixed4 mul(fixed1 v, fixed1x4 m)
{
    return mul<fixed,fixed,1,4>(v, m);
}
half4 mul(half1 v, half1x4 m)
{
    return mul<half,half,1,4>(v, m);
}

float1 mul(float2 v, float2x1 m)
{
    return mul<float,float,2,1>(v, m);
}
int1 mul(int2 v, int2x1 m)
{
    return mul<int,int,2,1>(v, m);
}
double1 mul(double2 v, double2x1 m)
{
    return mul<double,double,2,1>(v, m);
}
fixed1 mul(fixed2 v, fixed2x1 m)
{
    return mul<fixed,fixed,2,1>(v, m);
}
half1 mul(half2 v, half2x1 m)
{
    return mul<half,half,2,1>(v, m);
}

float2 mul(float2 v, float2x2 m)
{
    return mul<float,float,2,2>(v, m);
}
int2 mul(int2 v, int2x2 m)
{
    return mul<int,int,2,2>(v, m);
}
double2 mul(double2 v, double2x2 m)
{
    return mul<double,double,2,2>(v, m);
}
fixed2 mul(fixed2 v, fixed2x2 m)
{
    return mul<fixed,fixed,2,2>(v, m);
}
half2 mul(half2 v, half2x2 m)
{
    return mul<half,half,2,2>(v, m);
}

float3 mul(float2 v, float2x3 m)
{
    return mul<float,float,2,3>(v, m);
}
int3 mul(int2 v, int2x3 m)
{
    return mul<int,int,2,3>(v, m);
}
double3 mul(double2 v, double2x3 m)
{
    return mul<double,double,2,3>(v, m);
}
fixed3 mul(fixed2 v, fixed2x3 m)
{
    return mul<fixed,fixed,2,3>(v, m);
}
half3 mul(half2 v, half2x3 m)
{
    return mul<half,half,2,3>(v, m);
}

float4 mul(float2 v, float2x4 m)
{
    return mul<float,float,2,4>(v, m);
}
int4 mul(int2 v, int2x4 m)
{
    return mul<int,int,2,4>(v, m);
}
double4 mul(double2 v, double2x4 m)
{
    return mul<double,double,2,4>(v, m);
}
fixed4 mul(fixed2 v, fixed2x4 m)
{
    return mul<fixed,fixed,2,4>(v, m);
}
half4 mul(half2 v, half2x4 m)
{
    return mul<half,half,2,4>(v, m);
}

float1 mul(float3 v, float3x1 m)
{
    return mul<float,float,3,1>(v, m);
}
int1 mul(int3 v, int3x1 m)
{
    return mul<int,int,3,1>(v, m);
}
double1 mul(double3 v, double3x1 m)
{
    return mul<double,double,3,1>(v, m);
}
fixed1 mul(fixed3 v, fixed3x1 m)
{
    return mul<fixed,fixed,3,1>(v, m);
}
half1 mul(half3 v, half3x1 m)
{
    return mul<half,half,3,1>(v, m);
}

float2 mul(float3 v, float3x2 m)
{
    return mul<float,float,3,2>(v, m);
}
int2 mul(int3 v, int3x2 m)
{
    return mul<int,int,3,2>(v, m);
}
double2 mul(double3 v, double3x2 m)
{
    return mul<double,double,3,2>(v, m);
}
fixed2 mul(fixed3 v, fixed3x2 m)
{
    return mul<fixed,fixed,3,2>(v, m);
}
half2 mul(half3 v, half3x2 m)
{
    return mul<half,half,3,2>(v, m);
}

float3 mul(float3 v, float3x3 m)
{
    return mul<float,float,3,3>(v, m);
}
int3 mul(int3 v, int3x3 m)
{
    return mul<int,int,3,3>(v, m);
}
double3 mul(double3 v, double3x3 m)
{
    return mul<double,double,3,3>(v, m);
}
fixed3 mul(fixed3 v, fixed3x3 m)
{
    return mul<fixed,fixed,3,3>(v, m);
}
half3 mul(half3 v, half3x3 m)
{
    return mul<half,half,3,3>(v, m);
}

float4 mul(float3 v, float3x4 m)
{
    return mul<float,float,3,4>(v, m);
}
int4 mul(int3 v, int3x4 m)
{
    return mul<int,int,3,4>(v, m);
}
double4 mul(double3 v, double3x4 m)
{
    return mul<double,double,3,4>(v, m);
}
fixed4 mul(fixed3 v, fixed3x4 m)
{
    return mul<fixed,fixed,3,4>(v, m);
}
half4 mul(half3 v, half3x4 m)
{
    return mul<half,half,3,4>(v, m);
}

float1 mul(float4 v, float4x1 m)
{
    return mul<float,float,4,1>(v, m);
}
int1 mul(int4 v, int4x1 m)
{
    return mul<int,int,4,1>(v, m);
}
double1 mul(double4 v, double4x1 m)
{
    return mul<double,double,4,1>(v, m);
}
fixed1 mul(fixed4 v, fixed4x1 m)
{
    return mul<fixed,fixed,4,1>(v, m);
}
half1 mul(half4 v, half4x1 m)
{
    return mul<half,half,4,1>(v, m);
}

float2 mul(float4 v, float4x2 m)
{
    return mul<float,float,4,2>(v, m);
}
int2 mul(int4 v, int4x2 m)
{
    return mul<int,int,4,2>(v, m);
}
double2 mul(double4 v, double4x2 m)
{
    return mul<double,double,4,2>(v, m);
}
fixed2 mul(fixed4 v, fixed4x2 m)
{
    return mul<fixed,fixed,4,2>(v, m);
}
half2 mul(half4 v, half4x2 m)
{
    return mul<half,half,4,2>(v, m);
}

float3 mul(float4 v, float4x3 m)
{
    return mul<float,float,4,3>(v, m);
}
int3 mul(int4 v, int4x3 m)
{
    return mul<int,int,4,3>(v, m);
}
double3 mul(double4 v, double4x3 m)
{
    return mul<double,double,4,3>(v, m);
}
fixed3 mul(fixed4 v, fixed4x3 m)
{
    return mul<fixed,fixed,4,3>(v, m);
}
half3 mul(half4 v, half4x3 m)
{
    return mul<half,half,4,3>(v, m);
}

float4 mul(float4 v, float4x4 m)
{
    return mul<float,float,4,4>(v, m);
}
int4 mul(int4 v, int4x4 m)
{
    return mul<int,int,4,4>(v, m);
}
double4 mul(double4 v, double4x4 m)
{
    return mul<double,double,4,4>(v, m);
}
fixed4 mul(fixed4 v, fixed4x4 m)
{
    return mul<fixed,fixed,4,4>(v, m);
}
half4 mul(half4 v, half4x4 m)
{
    return mul<half,half,4,4>(v, m);
}

float1x1 mul(float1x1 m1, float1x1 m2)
{
    return mul<float,float,1,1,1>(m1, m2);
}
int1x1 mul(int1x1 m1, int1x1 m2)
{
    return mul<int,int,1,1,1>(m1, m2);
}
double1x1 mul(double1x1 m1, double1x1 m2)
{
    return mul<double,double,1,1,1>(m1, m2);
}
fixed1x1 mul(fixed1x1 m1, fixed1x1 m2)
{
    return mul<fixed,fixed,1,1,1>(m1, m2);
}
half1x1 mul(half1x1 m1, half1x1 m2)
{
    return mul<half,half,1,1,1>(m1, m2);
}

float1x2 mul(float1x1 m1, float1x2 m2)
{
    return mul<float,float,1,1,2>(m1, m2);
}
int1x2 mul(int1x1 m1, int1x2 m2)
{
    return mul<int,int,1,1,2>(m1, m2);
}
double1x2 mul(double1x1 m1, double1x2 m2)
{
    return mul<double,double,1,1,2>(m1, m2);
}
fixed1x2 mul(fixed1x1 m1, fixed1x2 m2)
{
    return mul<fixed,fixed,1,1,2>(m1, m2);
}
half1x2 mul(half1x1 m1, half1x2 m2)
{
    return mul<half,half,1,1,2>(m1, m2);
}

float1x3 mul(float1x1 m1, float1x3 m2)
{
    return mul<float,float,1,1,3>(m1, m2);
}
int1x3 mul(int1x1 m1, int1x3 m2)
{
    return mul<int,int,1,1,3>(m1, m2);
}
double1x3 mul(double1x1 m1, double1x3 m2)
{
    return mul<double,double,1,1,3>(m1, m2);
}
fixed1x3 mul(fixed1x1 m1, fixed1x3 m2)
{
    return mul<fixed,fixed,1,1,3>(m1, m2);
}
half1x3 mul(half1x1 m1, half1x3 m2)
{
    return mul<half,half,1,1,3>(m1, m2);
}

float1x4 mul(float1x1 m1, float1x4 m2)
{
    return mul<float,float,1,1,4>(m1, m2);
}
int1x4 mul(int1x1 m1, int1x4 m2)
{
    return mul<int,int,1,1,4>(m1, m2);
}
double1x4 mul(double1x1 m1, double1x4 m2)
{
    return mul<double,double,1,1,4>(m1, m2);
}
fixed1x4 mul(fixed1x1 m1, fixed1x4 m2)
{
    return mul<fixed,fixed,1,1,4>(m1, m2);
}
half1x4 mul(half1x1 m1, half1x4 m2)
{
    return mul<half,half,1,1,4>(m1, m2);
}

float1x1 mul(float1x2 m1, float2x1 m2)
{
    return mul<float,float,1,2,1>(m1, m2);
}
int1x1 mul(int1x2 m1, int2x1 m2)
{
    return mul<int,int,1,2,1>(m1, m2);
}
double1x1 mul(double1x2 m1, double2x1 m2)
{
    return mul<double,double,1,2,1>(m1, m2);
}
fixed1x1 mul(fixed1x2 m1, fixed2x1 m2)
{
    return mul<fixed,fixed,1,2,1>(m1, m2);
}
half1x1 mul(half1x2 m1, half2x1 m2)
{
    return mul<half,half,1,2,1>(m1, m2);
}

float1x2 mul(float1x2 m1, float2x2 m2)
{
    return mul<float,float,1,2,2>(m1, m2);
}
int1x2 mul(int1x2 m1, int2x2 m2)
{
    return mul<int,int,1,2,2>(m1, m2);
}
double1x2 mul(double1x2 m1, double2x2 m2)
{
    return mul<double,double,1,2,2>(m1, m2);
}
fixed1x2 mul(fixed1x2 m1, fixed2x2 m2)
{
    return mul<fixed,fixed,1,2,2>(m1, m2);
}
half1x2 mul(half1x2 m1, half2x2 m2)
{
    return mul<half,half,1,2,2>(m1, m2);
}

float1x3 mul(float1x2 m1, float2x3 m2)
{
    return mul<float,float,1,2,3>(m1, m2);
}
int1x3 mul(int1x2 m1, int2x3 m2)
{
    return mul<int,int,1,2,3>(m1, m2);
}
double1x3 mul(double1x2 m1, double2x3 m2)
{
    return mul<double,double,1,2,3>(m1, m2);
}
fixed1x3 mul(fixed1x2 m1, fixed2x3 m2)
{
    return mul<fixed,fixed,1,2,3>(m1, m2);
}
half1x3 mul(half1x2 m1, half2x3 m2)
{
    return mul<half,half,1,2,3>(m1, m2);
}

float1x4 mul(float1x2 m1, float2x4 m2)
{
    return mul<float,float,1,2,4>(m1, m2);
}
int1x4 mul(int1x2 m1, int2x4 m2)
{
    return mul<int,int,1,2,4>(m1, m2);
}
double1x4 mul(double1x2 m1, double2x4 m2)
{
    return mul<double,double,1,2,4>(m1, m2);
}
fixed1x4 mul(fixed1x2 m1, fixed2x4 m2)
{
    return mul<fixed,fixed,1,2,4>(m1, m2);
}
half1x4 mul(half1x2 m1, half2x4 m2)
{
    return mul<half,half,1,2,4>(m1, m2);
}

float1x1 mul(float1x3 m1, float3x1 m2)
{
    return mul<float,float,1,3,1>(m1, m2);
}
int1x1 mul(int1x3 m1, int3x1 m2)
{
    return mul<int,int,1,3,1>(m1, m2);
}
double1x1 mul(double1x3 m1, double3x1 m2)
{
    return mul<double,double,1,3,1>(m1, m2);
}
fixed1x1 mul(fixed1x3 m1, fixed3x1 m2)
{
    return mul<fixed,fixed,1,3,1>(m1, m2);
}
half1x1 mul(half1x3 m1, half3x1 m2)
{
    return mul<half,half,1,3,1>(m1, m2);
}

float1x2 mul(float1x3 m1, float3x2 m2)
{
    return mul<float,float,1,3,2>(m1, m2);
}
int1x2 mul(int1x3 m1, int3x2 m2)
{
    return mul<int,int,1,3,2>(m1, m2);
}
double1x2 mul(double1x3 m1, double3x2 m2)
{
    return mul<double,double,1,3,2>(m1, m2);
}
fixed1x2 mul(fixed1x3 m1, fixed3x2 m2)
{
    return mul<fixed,fixed,1,3,2>(m1, m2);
}
half1x2 mul(half1x3 m1, half3x2 m2)
{
    return mul<half,half,1,3,2>(m1, m2);
}

float1x3 mul(float1x3 m1, float3x3 m2)
{
    return mul<float,float,1,3,3>(m1, m2);
}
int1x3 mul(int1x3 m1, int3x3 m2)
{
    return mul<int,int,1,3,3>(m1, m2);
}
double1x3 mul(double1x3 m1, double3x3 m2)
{
    return mul<double,double,1,3,3>(m1, m2);
}
fixed1x3 mul(fixed1x3 m1, fixed3x3 m2)
{
    return mul<fixed,fixed,1,3,3>(m1, m2);
}
half1x3 mul(half1x3 m1, half3x3 m2)
{
    return mul<half,half,1,3,3>(m1, m2);
}

float1x4 mul(float1x3 m1, float3x4 m2)
{
    return mul<float,float,1,3,4>(m1, m2);
}
int1x4 mul(int1x3 m1, int3x4 m2)
{
    return mul<int,int,1,3,4>(m1, m2);
}
double1x4 mul(double1x3 m1, double3x4 m2)
{
    return mul<double,double,1,3,4>(m1, m2);
}
fixed1x4 mul(fixed1x3 m1, fixed3x4 m2)
{
    return mul<fixed,fixed,1,3,4>(m1, m2);
}
half1x4 mul(half1x3 m1, half3x4 m2)
{
    return mul<half,half,1,3,4>(m1, m2);
}

float1x1 mul(float1x4 m1, float4x1 m2)
{
    return mul<float,float,1,4,1>(m1, m2);
}
int1x1 mul(int1x4 m1, int4x1 m2)
{
    return mul<int,int,1,4,1>(m1, m2);
}
double1x1 mul(double1x4 m1, double4x1 m2)
{
    return mul<double,double,1,4,1>(m1, m2);
}
fixed1x1 mul(fixed1x4 m1, fixed4x1 m2)
{
    return mul<fixed,fixed,1,4,1>(m1, m2);
}
half1x1 mul(half1x4 m1, half4x1 m2)
{
    return mul<half,half,1,4,1>(m1, m2);
}

float1x2 mul(float1x4 m1, float4x2 m2)
{
    return mul<float,float,1,4,2>(m1, m2);
}
int1x2 mul(int1x4 m1, int4x2 m2)
{
    return mul<int,int,1,4,2>(m1, m2);
}
double1x2 mul(double1x4 m1, double4x2 m2)
{
    return mul<double,double,1,4,2>(m1, m2);
}
fixed1x2 mul(fixed1x4 m1, fixed4x2 m2)
{
    return mul<fixed,fixed,1,4,2>(m1, m2);
}
half1x2 mul(half1x4 m1, half4x2 m2)
{
    return mul<half,half,1,4,2>(m1, m2);
}

float1x3 mul(float1x4 m1, float4x3 m2)
{
    return mul<float,float,1,4,3>(m1, m2);
}
int1x3 mul(int1x4 m1, int4x3 m2)
{
    return mul<int,int,1,4,3>(m1, m2);
}
double1x3 mul(double1x4 m1, double4x3 m2)
{
    return mul<double,double,1,4,3>(m1, m2);
}
fixed1x3 mul(fixed1x4 m1, fixed4x3 m2)
{
    return mul<fixed,fixed,1,4,3>(m1, m2);
}
half1x3 mul(half1x4 m1, half4x3 m2)
{
    return mul<half,half,1,4,3>(m1, m2);
}

float1x4 mul(float1x4 m1, float4x4 m2)
{
    return mul<float,float,1,4,4>(m1, m2);
}
int1x4 mul(int1x4 m1, int4x4 m2)
{
    return mul<int,int,1,4,4>(m1, m2);
}
double1x4 mul(double1x4 m1, double4x4 m2)
{
    return mul<double,double,1,4,4>(m1, m2);
}
fixed1x4 mul(fixed1x4 m1, fixed4x4 m2)
{
    return mul<fixed,fixed,1,4,4>(m1, m2);
}
half1x4 mul(half1x4 m1, half4x4 m2)
{
    return mul<half,half,1,4,4>(m1, m2);
}

float2x1 mul(float2x1 m1, float1x1 m2)
{
    return mul<float,float,2,1,1>(m1, m2);
}
int2x1 mul(int2x1 m1, int1x1 m2)
{
    return mul<int,int,2,1,1>(m1, m2);
}
double2x1 mul(double2x1 m1, double1x1 m2)
{
    return mul<double,double,2,1,1>(m1, m2);
}
fixed2x1 mul(fixed2x1 m1, fixed1x1 m2)
{
    return mul<fixed,fixed,2,1,1>(m1, m2);
}
half2x1 mul(half2x1 m1, half1x1 m2)
{
    return mul<half,half,2,1,1>(m1, m2);
}

float2x2 mul(float2x1 m1, float1x2 m2)
{
    return mul<float,float,2,1,2>(m1, m2);
}
int2x2 mul(int2x1 m1, int1x2 m2)
{
    return mul<int,int,2,1,2>(m1, m2);
}
double2x2 mul(double2x1 m1, double1x2 m2)
{
    return mul<double,double,2,1,2>(m1, m2);
}
fixed2x2 mul(fixed2x1 m1, fixed1x2 m2)
{
    return mul<fixed,fixed,2,1,2>(m1, m2);
}
half2x2 mul(half2x1 m1, half1x2 m2)
{
    return mul<half,half,2,1,2>(m1, m2);
}

float2x3 mul(float2x1 m1, float1x3 m2)
{
    return mul<float,float,2,1,3>(m1, m2);
}
int2x3 mul(int2x1 m1, int1x3 m2)
{
    return mul<int,int,2,1,3>(m1, m2);
}
double2x3 mul(double2x1 m1, double1x3 m2)
{
    return mul<double,double,2,1,3>(m1, m2);
}
fixed2x3 mul(fixed2x1 m1, fixed1x3 m2)
{
    return mul<fixed,fixed,2,1,3>(m1, m2);
}
half2x3 mul(half2x1 m1, half1x3 m2)
{
    return mul<half,half,2,1,3>(m1, m2);
}

float2x4 mul(float2x1 m1, float1x4 m2)
{
    return mul<float,float,2,1,4>(m1, m2);
}
int2x4 mul(int2x1 m1, int1x4 m2)
{
    return mul<int,int,2,1,4>(m1, m2);
}
double2x4 mul(double2x1 m1, double1x4 m2)
{
    return mul<double,double,2,1,4>(m1, m2);
}
fixed2x4 mul(fixed2x1 m1, fixed1x4 m2)
{
    return mul<fixed,fixed,2,1,4>(m1, m2);
}
half2x4 mul(half2x1 m1, half1x4 m2)
{
    return mul<half,half,2,1,4>(m1, m2);
}

float2x1 mul(float2x2 m1, float2x1 m2)
{
    return mul<float,float,2,2,1>(m1, m2);
}
int2x1 mul(int2x2 m1, int2x1 m2)
{
    return mul<int,int,2,2,1>(m1, m2);
}
double2x1 mul(double2x2 m1, double2x1 m2)
{
    return mul<double,double,2,2,1>(m1, m2);
}
fixed2x1 mul(fixed2x2 m1, fixed2x1 m2)
{
    return mul<fixed,fixed,2,2,1>(m1, m2);
}
half2x1 mul(half2x2 m1, half2x1 m2)
{
    return mul<half,half,2,2,1>(m1, m2);
}

float2x2 mul(float2x2 m1, float2x2 m2)
{
    return mul<float,float,2,2,2>(m1, m2);
}
int2x2 mul(int2x2 m1, int2x2 m2)
{
    return mul<int,int,2,2,2>(m1, m2);
}
double2x2 mul(double2x2 m1, double2x2 m2)
{
    return mul<double,double,2,2,2>(m1, m2);
}
fixed2x2 mul(fixed2x2 m1, fixed2x2 m2)
{
    return mul<fixed,fixed,2,2,2>(m1, m2);
}
half2x2 mul(half2x2 m1, half2x2 m2)
{
    return mul<half,half,2,2,2>(m1, m2);
}

float2x3 mul(float2x2 m1, float2x3 m2)
{
    return mul<float,float,2,2,3>(m1, m2);
}
int2x3 mul(int2x2 m1, int2x3 m2)
{
    return mul<int,int,2,2,3>(m1, m2);
}
double2x3 mul(double2x2 m1, double2x3 m2)
{
    return mul<double,double,2,2,3>(m1, m2);
}
fixed2x3 mul(fixed2x2 m1, fixed2x3 m2)
{
    return mul<fixed,fixed,2,2,3>(m1, m2);
}
half2x3 mul(half2x2 m1, half2x3 m2)
{
    return mul<half,half,2,2,3>(m1, m2);
}

float2x4 mul(float2x2 m1, float2x4 m2)
{
    return mul<float,float,2,2,4>(m1, m2);
}
int2x4 mul(int2x2 m1, int2x4 m2)
{
    return mul<int,int,2,2,4>(m1, m2);
}
double2x4 mul(double2x2 m1, double2x4 m2)
{
    return mul<double,double,2,2,4>(m1, m2);
}
fixed2x4 mul(fixed2x2 m1, fixed2x4 m2)
{
    return mul<fixed,fixed,2,2,4>(m1, m2);
}
half2x4 mul(half2x2 m1, half2x4 m2)
{
    return mul<half,half,2,2,4>(m1, m2);
}

float2x1 mul(float2x3 m1, float3x1 m2)
{
    return mul<float,float,2,3,1>(m1, m2);
}
int2x1 mul(int2x3 m1, int3x1 m2)
{
    return mul<int,int,2,3,1>(m1, m2);
}
double2x1 mul(double2x3 m1, double3x1 m2)
{
    return mul<double,double,2,3,1>(m1, m2);
}
fixed2x1 mul(fixed2x3 m1, fixed3x1 m2)
{
    return mul<fixed,fixed,2,3,1>(m1, m2);
}
half2x1 mul(half2x3 m1, half3x1 m2)
{
    return mul<half,half,2,3,1>(m1, m2);
}

float2x2 mul(float2x3 m1, float3x2 m2)
{
    return mul<float,float,2,3,2>(m1, m2);
}
int2x2 mul(int2x3 m1, int3x2 m2)
{
    return mul<int,int,2,3,2>(m1, m2);
}
double2x2 mul(double2x3 m1, double3x2 m2)
{
    return mul<double,double,2,3,2>(m1, m2);
}
fixed2x2 mul(fixed2x3 m1, fixed3x2 m2)
{
    return mul<fixed,fixed,2,3,2>(m1, m2);
}
half2x2 mul(half2x3 m1, half3x2 m2)
{
    return mul<half,half,2,3,2>(m1, m2);
}

float2x3 mul(float2x3 m1, float3x3 m2)
{
    return mul<float,float,2,3,3>(m1, m2);
}
int2x3 mul(int2x3 m1, int3x3 m2)
{
    return mul<int,int,2,3,3>(m1, m2);
}
double2x3 mul(double2x3 m1, double3x3 m2)
{
    return mul<double,double,2,3,3>(m1, m2);
}
fixed2x3 mul(fixed2x3 m1, fixed3x3 m2)
{
    return mul<fixed,fixed,2,3,3>(m1, m2);
}
half2x3 mul(half2x3 m1, half3x3 m2)
{
    return mul<half,half,2,3,3>(m1, m2);
}

float2x4 mul(float2x3 m1, float3x4 m2)
{
    return mul<float,float,2,3,4>(m1, m2);
}
int2x4 mul(int2x3 m1, int3x4 m2)
{
    return mul<int,int,2,3,4>(m1, m2);
}
double2x4 mul(double2x3 m1, double3x4 m2)
{
    return mul<double,double,2,3,4>(m1, m2);
}
fixed2x4 mul(fixed2x3 m1, fixed3x4 m2)
{
    return mul<fixed,fixed,2,3,4>(m1, m2);
}
half2x4 mul(half2x3 m1, half3x4 m2)
{
    return mul<half,half,2,3,4>(m1, m2);
}

float2x1 mul(float2x4 m1, float4x1 m2)
{
    return mul<float,float,2,4,1>(m1, m2);
}
int2x1 mul(int2x4 m1, int4x1 m2)
{
    return mul<int,int,2,4,1>(m1, m2);
}
double2x1 mul(double2x4 m1, double4x1 m2)
{
    return mul<double,double,2,4,1>(m1, m2);
}
fixed2x1 mul(fixed2x4 m1, fixed4x1 m2)
{
    return mul<fixed,fixed,2,4,1>(m1, m2);
}
half2x1 mul(half2x4 m1, half4x1 m2)
{
    return mul<half,half,2,4,1>(m1, m2);
}

float2x2 mul(float2x4 m1, float4x2 m2)
{
    return mul<float,float,2,4,2>(m1, m2);
}
int2x2 mul(int2x4 m1, int4x2 m2)
{
    return mul<int,int,2,4,2>(m1, m2);
}
double2x2 mul(double2x4 m1, double4x2 m2)
{
    return mul<double,double,2,4,2>(m1, m2);
}
fixed2x2 mul(fixed2x4 m1, fixed4x2 m2)
{
    return mul<fixed,fixed,2,4,2>(m1, m2);
}
half2x2 mul(half2x4 m1, half4x2 m2)
{
    return mul<half,half,2,4,2>(m1, m2);
}

float2x3 mul(float2x4 m1, float4x3 m2)
{
    return mul<float,float,2,4,3>(m1, m2);
}
int2x3 mul(int2x4 m1, int4x3 m2)
{
    return mul<int,int,2,4,3>(m1, m2);
}
double2x3 mul(double2x4 m1, double4x3 m2)
{
    return mul<double,double,2,4,3>(m1, m2);
}
fixed2x3 mul(fixed2x4 m1, fixed4x3 m2)
{
    return mul<fixed,fixed,2,4,3>(m1, m2);
}
half2x3 mul(half2x4 m1, half4x3 m2)
{
    return mul<half,half,2,4,3>(m1, m2);
}

float2x4 mul(float2x4 m1, float4x4 m2)
{
    return mul<float,float,2,4,4>(m1, m2);
}
int2x4 mul(int2x4 m1, int4x4 m2)
{
    return mul<int,int,2,4,4>(m1, m2);
}
double2x4 mul(double2x4 m1, double4x4 m2)
{
    return mul<double,double,2,4,4>(m1, m2);
}
fixed2x4 mul(fixed2x4 m1, fixed4x4 m2)
{
    return mul<fixed,fixed,2,4,4>(m1, m2);
}
half2x4 mul(half2x4 m1, half4x4 m2)
{
    return mul<half,half,2,4,4>(m1, m2);
}

float3x1 mul(float3x1 m1, float1x1 m2)
{
    return mul<float,float,3,1,1>(m1, m2);
}
int3x1 mul(int3x1 m1, int1x1 m2)
{
    return mul<int,int,3,1,1>(m1, m2);
}
double3x1 mul(double3x1 m1, double1x1 m2)
{
    return mul<double,double,3,1,1>(m1, m2);
}
fixed3x1 mul(fixed3x1 m1, fixed1x1 m2)
{
    return mul<fixed,fixed,3,1,1>(m1, m2);
}
half3x1 mul(half3x1 m1, half1x1 m2)
{
    return mul<half,half,3,1,1>(m1, m2);
}

float3x2 mul(float3x1 m1, float1x2 m2)
{
    return mul<float,float,3,1,2>(m1, m2);
}
int3x2 mul(int3x1 m1, int1x2 m2)
{
    return mul<int,int,3,1,2>(m1, m2);
}
double3x2 mul(double3x1 m1, double1x2 m2)
{
    return mul<double,double,3,1,2>(m1, m2);
}
fixed3x2 mul(fixed3x1 m1, fixed1x2 m2)
{
    return mul<fixed,fixed,3,1,2>(m1, m2);
}
half3x2 mul(half3x1 m1, half1x2 m2)
{
    return mul<half,half,3,1,2>(m1, m2);
}

float3x3 mul(float3x1 m1, float1x3 m2)
{
    return mul<float,float,3,1,3>(m1, m2);
}
int3x3 mul(int3x1 m1, int1x3 m2)
{
    return mul<int,int,3,1,3>(m1, m2);
}
double3x3 mul(double3x1 m1, double1x3 m2)
{
    return mul<double,double,3,1,3>(m1, m2);
}
fixed3x3 mul(fixed3x1 m1, fixed1x3 m2)
{
    return mul<fixed,fixed,3,1,3>(m1, m2);
}
half3x3 mul(half3x1 m1, half1x3 m2)
{
    return mul<half,half,3,1,3>(m1, m2);
}

float3x4 mul(float3x1 m1, float1x4 m2)
{
    return mul<float,float,3,1,4>(m1, m2);
}
int3x4 mul(int3x1 m1, int1x4 m2)
{
    return mul<int,int,3,1,4>(m1, m2);
}
double3x4 mul(double3x1 m1, double1x4 m2)
{
    return mul<double,double,3,1,4>(m1, m2);
}
fixed3x4 mul(fixed3x1 m1, fixed1x4 m2)
{
    return mul<fixed,fixed,3,1,4>(m1, m2);
}
half3x4 mul(half3x1 m1, half1x4 m2)
{
    return mul<half,half,3,1,4>(m1, m2);
}

float3x1 mul(float3x2 m1, float2x1 m2)
{
    return mul<float,float,3,2,1>(m1, m2);
}
int3x1 mul(int3x2 m1, int2x1 m2)
{
    return mul<int,int,3,2,1>(m1, m2);
}
double3x1 mul(double3x2 m1, double2x1 m2)
{
    return mul<double,double,3,2,1>(m1, m2);
}
fixed3x1 mul(fixed3x2 m1, fixed2x1 m2)
{
    return mul<fixed,fixed,3,2,1>(m1, m2);
}
half3x1 mul(half3x2 m1, half2x1 m2)
{
    return mul<half,half,3,2,1>(m1, m2);
}

float3x2 mul(float3x2 m1, float2x2 m2)
{
    return mul<float,float,3,2,2>(m1, m2);
}
int3x2 mul(int3x2 m1, int2x2 m2)
{
    return mul<int,int,3,2,2>(m1, m2);
}
double3x2 mul(double3x2 m1, double2x2 m2)
{
    return mul<double,double,3,2,2>(m1, m2);
}
fixed3x2 mul(fixed3x2 m1, fixed2x2 m2)
{
    return mul<fixed,fixed,3,2,2>(m1, m2);
}
half3x2 mul(half3x2 m1, half2x2 m2)
{
    return mul<half,half,3,2,2>(m1, m2);
}

float3x3 mul(float3x2 m1, float2x3 m2)
{
    return mul<float,float,3,2,3>(m1, m2);
}
int3x3 mul(int3x2 m1, int2x3 m2)
{
    return mul<int,int,3,2,3>(m1, m2);
}
double3x3 mul(double3x2 m1, double2x3 m2)
{
    return mul<double,double,3,2,3>(m1, m2);
}
fixed3x3 mul(fixed3x2 m1, fixed2x3 m2)
{
    return mul<fixed,fixed,3,2,3>(m1, m2);
}
half3x3 mul(half3x2 m1, half2x3 m2)
{
    return mul<half,half,3,2,3>(m1, m2);
}

float3x4 mul(float3x2 m1, float2x4 m2)
{
    return mul<float,float,3,2,4>(m1, m2);
}
int3x4 mul(int3x2 m1, int2x4 m2)
{
    return mul<int,int,3,2,4>(m1, m2);
}
double3x4 mul(double3x2 m1, double2x4 m2)
{
    return mul<double,double,3,2,4>(m1, m2);
}
fixed3x4 mul(fixed3x2 m1, fixed2x4 m2)
{
    return mul<fixed,fixed,3,2,4>(m1, m2);
}
half3x4 mul(half3x2 m1, half2x4 m2)
{
    return mul<half,half,3,2,4>(m1, m2);
}

float3x1 mul(float3x3 m1, float3x1 m2)
{
    return mul<float,float,3,3,1>(m1, m2);
}
int3x1 mul(int3x3 m1, int3x1 m2)
{
    return mul<int,int,3,3,1>(m1, m2);
}
double3x1 mul(double3x3 m1, double3x1 m2)
{
    return mul<double,double,3,3,1>(m1, m2);
}
fixed3x1 mul(fixed3x3 m1, fixed3x1 m2)
{
    return mul<fixed,fixed,3,3,1>(m1, m2);
}
half3x1 mul(half3x3 m1, half3x1 m2)
{
    return mul<half,half,3,3,1>(m1, m2);
}

float3x2 mul(float3x3 m1, float3x2 m2)
{
    return mul<float,float,3,3,2>(m1, m2);
}
int3x2 mul(int3x3 m1, int3x2 m2)
{
    return mul<int,int,3,3,2>(m1, m2);
}
double3x2 mul(double3x3 m1, double3x2 m2)
{
    return mul<double,double,3,3,2>(m1, m2);
}
fixed3x2 mul(fixed3x3 m1, fixed3x2 m2)
{
    return mul<fixed,fixed,3,3,2>(m1, m2);
}
half3x2 mul(half3x3 m1, half3x2 m2)
{
    return mul<half,half,3,3,2>(m1, m2);
}

float3x3 mul(float3x3 m1, float3x3 m2)
{
    return mul<float,float,3,3,3>(m1, m2);
}
int3x3 mul(int3x3 m1, int3x3 m2)
{
    return mul<int,int,3,3,3>(m1, m2);
}
double3x3 mul(double3x3 m1, double3x3 m2)
{
    return mul<double,double,3,3,3>(m1, m2);
}
fixed3x3 mul(fixed3x3 m1, fixed3x3 m2)
{
    return mul<fixed,fixed,3,3,3>(m1, m2);
}
half3x3 mul(half3x3 m1, half3x3 m2)
{
    return mul<half,half,3,3,3>(m1, m2);
}

float3x4 mul(float3x3 m1, float3x4 m2)
{
    return mul<float,float,3,3,4>(m1, m2);
}
int3x4 mul(int3x3 m1, int3x4 m2)
{
    return mul<int,int,3,3,4>(m1, m2);
}
double3x4 mul(double3x3 m1, double3x4 m2)
{
    return mul<double,double,3,3,4>(m1, m2);
}
fixed3x4 mul(fixed3x3 m1, fixed3x4 m2)
{
    return mul<fixed,fixed,3,3,4>(m1, m2);
}
half3x4 mul(half3x3 m1, half3x4 m2)
{
    return mul<half,half,3,3,4>(m1, m2);
}

float3x1 mul(float3x4 m1, float4x1 m2)
{
    return mul<float,float,3,4,1>(m1, m2);
}
int3x1 mul(int3x4 m1, int4x1 m2)
{
    return mul<int,int,3,4,1>(m1, m2);
}
double3x1 mul(double3x4 m1, double4x1 m2)
{
    return mul<double,double,3,4,1>(m1, m2);
}
fixed3x1 mul(fixed3x4 m1, fixed4x1 m2)
{
    return mul<fixed,fixed,3,4,1>(m1, m2);
}
half3x1 mul(half3x4 m1, half4x1 m2)
{
    return mul<half,half,3,4,1>(m1, m2);
}

float3x2 mul(float3x4 m1, float4x2 m2)
{
    return mul<float,float,3,4,2>(m1, m2);
}
int3x2 mul(int3x4 m1, int4x2 m2)
{
    return mul<int,int,3,4,2>(m1, m2);
}
double3x2 mul(double3x4 m1, double4x2 m2)
{
    return mul<double,double,3,4,2>(m1, m2);
}
fixed3x2 mul(fixed3x4 m1, fixed4x2 m2)
{
    return mul<fixed,fixed,3,4,2>(m1, m2);
}
half3x2 mul(half3x4 m1, half4x2 m2)
{
    return mul<half,half,3,4,2>(m1, m2);
}

float3x3 mul(float3x4 m1, float4x3 m2)
{
    return mul<float,float,3,4,3>(m1, m2);
}
int3x3 mul(int3x4 m1, int4x3 m2)
{
    return mul<int,int,3,4,3>(m1, m2);
}
double3x3 mul(double3x4 m1, double4x3 m2)
{
    return mul<double,double,3,4,3>(m1, m2);
}
fixed3x3 mul(fixed3x4 m1, fixed4x3 m2)
{
    return mul<fixed,fixed,3,4,3>(m1, m2);
}
half3x3 mul(half3x4 m1, half4x3 m2)
{
    return mul<half,half,3,4,3>(m1, m2);
}

float3x4 mul(float3x4 m1, float4x4 m2)
{
    return mul<float,float,3,4,4>(m1, m2);
}
int3x4 mul(int3x4 m1, int4x4 m2)
{
    return mul<int,int,3,4,4>(m1, m2);
}
double3x4 mul(double3x4 m1, double4x4 m2)
{
    return mul<double,double,3,4,4>(m1, m2);
}
fixed3x4 mul(fixed3x4 m1, fixed4x4 m2)
{
    return mul<fixed,fixed,3,4,4>(m1, m2);
}
half3x4 mul(half3x4 m1, half4x4 m2)
{
    return mul<half,half,3,4,4>(m1, m2);
}

float4x1 mul(float4x1 m1, float1x1 m2)
{
    return mul<float,float,4,1,1>(m1, m2);
}
int4x1 mul(int4x1 m1, int1x1 m2)
{
    return mul<int,int,4,1,1>(m1, m2);
}
double4x1 mul(double4x1 m1, double1x1 m2)
{
    return mul<double,double,4,1,1>(m1, m2);
}
fixed4x1 mul(fixed4x1 m1, fixed1x1 m2)
{
    return mul<fixed,fixed,4,1,1>(m1, m2);
}
half4x1 mul(half4x1 m1, half1x1 m2)
{
    return mul<half,half,4,1,1>(m1, m2);
}

float4x2 mul(float4x1 m1, float1x2 m2)
{
    return mul<float,float,4,1,2>(m1, m2);
}
int4x2 mul(int4x1 m1, int1x2 m2)
{
    return mul<int,int,4,1,2>(m1, m2);
}
double4x2 mul(double4x1 m1, double1x2 m2)
{
    return mul<double,double,4,1,2>(m1, m2);
}
fixed4x2 mul(fixed4x1 m1, fixed1x2 m2)
{
    return mul<fixed,fixed,4,1,2>(m1, m2);
}
half4x2 mul(half4x1 m1, half1x2 m2)
{
    return mul<half,half,4,1,2>(m1, m2);
}

float4x3 mul(float4x1 m1, float1x3 m2)
{
    return mul<float,float,4,1,3>(m1, m2);
}
int4x3 mul(int4x1 m1, int1x3 m2)
{
    return mul<int,int,4,1,3>(m1, m2);
}
double4x3 mul(double4x1 m1, double1x3 m2)
{
    return mul<double,double,4,1,3>(m1, m2);
}
fixed4x3 mul(fixed4x1 m1, fixed1x3 m2)
{
    return mul<fixed,fixed,4,1,3>(m1, m2);
}
half4x3 mul(half4x1 m1, half1x3 m2)
{
    return mul<half,half,4,1,3>(m1, m2);
}

float4x4 mul(float4x1 m1, float1x4 m2)
{
    return mul<float,float,4,1,4>(m1, m2);
}
int4x4 mul(int4x1 m1, int1x4 m2)
{
    return mul<int,int,4,1,4>(m1, m2);
}
double4x4 mul(double4x1 m1, double1x4 m2)
{
    return mul<double,double,4,1,4>(m1, m2);
}
fixed4x4 mul(fixed4x1 m1, fixed1x4 m2)
{
    return mul<fixed,fixed,4,1,4>(m1, m2);
}
half4x4 mul(half4x1 m1, half1x4 m2)
{
    return mul<half,half,4,1,4>(m1, m2);
}

float4x1 mul(float4x2 m1, float2x1 m2)
{
    return mul<float,float,4,2,1>(m1, m2);
}
int4x1 mul(int4x2 m1, int2x1 m2)
{
    return mul<int,int,4,2,1>(m1, m2);
}
double4x1 mul(double4x2 m1, double2x1 m2)
{
    return mul<double,double,4,2,1>(m1, m2);
}
fixed4x1 mul(fixed4x2 m1, fixed2x1 m2)
{
    return mul<fixed,fixed,4,2,1>(m1, m2);
}
half4x1 mul(half4x2 m1, half2x1 m2)
{
    return mul<half,half,4,2,1>(m1, m2);
}

float4x2 mul(float4x2 m1, float2x2 m2)
{
    return mul<float,float,4,2,2>(m1, m2);
}
int4x2 mul(int4x2 m1, int2x2 m2)
{
    return mul<int,int,4,2,2>(m1, m2);
}
double4x2 mul(double4x2 m1, double2x2 m2)
{
    return mul<double,double,4,2,2>(m1, m2);
}
fixed4x2 mul(fixed4x2 m1, fixed2x2 m2)
{
    return mul<fixed,fixed,4,2,2>(m1, m2);
}
half4x2 mul(half4x2 m1, half2x2 m2)
{
    return mul<half,half,4,2,2>(m1, m2);
}

float4x3 mul(float4x2 m1, float2x3 m2)
{
    return mul<float,float,4,2,3>(m1, m2);
}
int4x3 mul(int4x2 m1, int2x3 m2)
{
    return mul<int,int,4,2,3>(m1, m2);
}
double4x3 mul(double4x2 m1, double2x3 m2)
{
    return mul<double,double,4,2,3>(m1, m2);
}
fixed4x3 mul(fixed4x2 m1, fixed2x3 m2)
{
    return mul<fixed,fixed,4,2,3>(m1, m2);
}
half4x3 mul(half4x2 m1, half2x3 m2)
{
    return mul<half,half,4,2,3>(m1, m2);
}

float4x4 mul(float4x2 m1, float2x4 m2)
{
    return mul<float,float,4,2,4>(m1, m2);
}
int4x4 mul(int4x2 m1, int2x4 m2)
{
    return mul<int,int,4,2,4>(m1, m2);
}
double4x4 mul(double4x2 m1, double2x4 m2)
{
    return mul<double,double,4,2,4>(m1, m2);
}
fixed4x4 mul(fixed4x2 m1, fixed2x4 m2)
{
    return mul<fixed,fixed,4,2,4>(m1, m2);
}
half4x4 mul(half4x2 m1, half2x4 m2)
{
    return mul<half,half,4,2,4>(m1, m2);
}

float4x1 mul(float4x3 m1, float3x1 m2)
{
    return mul<float,float,4,3,1>(m1, m2);
}
int4x1 mul(int4x3 m1, int3x1 m2)
{
    return mul<int,int,4,3,1>(m1, m2);
}
double4x1 mul(double4x3 m1, double3x1 m2)
{
    return mul<double,double,4,3,1>(m1, m2);
}
fixed4x1 mul(fixed4x3 m1, fixed3x1 m2)
{
    return mul<fixed,fixed,4,3,1>(m1, m2);
}
half4x1 mul(half4x3 m1, half3x1 m2)
{
    return mul<half,half,4,3,1>(m1, m2);
}

float4x2 mul(float4x3 m1, float3x2 m2)
{
    return mul<float,float,4,3,2>(m1, m2);
}
int4x2 mul(int4x3 m1, int3x2 m2)
{
    return mul<int,int,4,3,2>(m1, m2);
}
double4x2 mul(double4x3 m1, double3x2 m2)
{
    return mul<double,double,4,3,2>(m1, m2);
}
fixed4x2 mul(fixed4x3 m1, fixed3x2 m2)
{
    return mul<fixed,fixed,4,3,2>(m1, m2);
}
half4x2 mul(half4x3 m1, half3x2 m2)
{
    return mul<half,half,4,3,2>(m1, m2);
}

float4x3 mul(float4x3 m1, float3x3 m2)
{
    return mul<float,float,4,3,3>(m1, m2);
}
int4x3 mul(int4x3 m1, int3x3 m2)
{
    return mul<int,int,4,3,3>(m1, m2);
}
double4x3 mul(double4x3 m1, double3x3 m2)
{
    return mul<double,double,4,3,3>(m1, m2);
}
fixed4x3 mul(fixed4x3 m1, fixed3x3 m2)
{
    return mul<fixed,fixed,4,3,3>(m1, m2);
}
half4x3 mul(half4x3 m1, half3x3 m2)
{
    return mul<half,half,4,3,3>(m1, m2);
}

float4x4 mul(float4x3 m1, float3x4 m2)
{
    return mul<float,float,4,3,4>(m1, m2);
}
int4x4 mul(int4x3 m1, int3x4 m2)
{
    return mul<int,int,4,3,4>(m1, m2);
}
double4x4 mul(double4x3 m1, double3x4 m2)
{
    return mul<double,double,4,3,4>(m1, m2);
}
fixed4x4 mul(fixed4x3 m1, fixed3x4 m2)
{
    return mul<fixed,fixed,4,3,4>(m1, m2);
}
half4x4 mul(half4x3 m1, half3x4 m2)
{
    return mul<half,half,4,3,4>(m1, m2);
}

float4x1 mul(float4x4 m1, float4x1 m2)
{
    return mul<float,float,4,4,1>(m1, m2);
}
int4x1 mul(int4x4 m1, int4x1 m2)
{
    return mul<int,int,4,4,1>(m1, m2);
}
double4x1 mul(double4x4 m1, double4x1 m2)
{
    return mul<double,double,4,4,1>(m1, m2);
}
fixed4x1 mul(fixed4x4 m1, fixed4x1 m2)
{
    return mul<fixed,fixed,4,4,1>(m1, m2);
}
half4x1 mul(half4x4 m1, half4x1 m2)
{
    return mul<half,half,4,4,1>(m1, m2);
}

float4x2 mul(float4x4 m1, float4x2 m2)
{
    return mul<float,float,4,4,2>(m1, m2);
}
int4x2 mul(int4x4 m1, int4x2 m2)
{
    return mul<int,int,4,4,2>(m1, m2);
}
double4x2 mul(double4x4 m1, double4x2 m2)
{
    return mul<double,double,4,4,2>(m1, m2);
}
fixed4x2 mul(fixed4x4 m1, fixed4x2 m2)
{
    return mul<fixed,fixed,4,4,2>(m1, m2);
}
half4x2 mul(half4x4 m1, half4x2 m2)
{
    return mul<half,half,4,4,2>(m1, m2);
}

float4x3 mul(float4x4 m1, float4x3 m2)
{
    return mul<float,float,4,4,3>(m1, m2);
}
int4x3 mul(int4x4 m1, int4x3 m2)
{
    return mul<int,int,4,4,3>(m1, m2);
}
double4x3 mul(double4x4 m1, double4x3 m2)
{
    return mul<double,double,4,4,3>(m1, m2);
}
fixed4x3 mul(fixed4x4 m1, fixed4x3 m2)
{
    return mul<fixed,fixed,4,4,3>(m1, m2);
}
half4x3 mul(half4x4 m1, half4x3 m2)
{
    return mul<half,half,4,4,3>(m1, m2);
}

float4x4 mul(float4x4 m1, float4x4 m2)
{
    return mul<float,float,4,4,4>(m1, m2);
}
int4x4 mul(int4x4 m1, int4x4 m2)
{
    return mul<int,int,4,4,4>(m1, m2);
}
double4x4 mul(double4x4 m1, double4x4 m2)
{
    return mul<double,double,4,4,4>(m1, m2);
}
fixed4x4 mul(fixed4x4 m1, fixed4x4 m2)
{
    return mul<fixed,fixed,4,4,4>(m1, m2);
}
half4x4 mul(half4x4 m1, half4x4 m2)
{
    return mul<half,half,4,4,4>(m1, m2);
}

} // namespace Cg

