/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/double.hpp>
#include <Cg/transpose.hpp>

namespace Cg {

float4x4 transpose(float4x4 a)
{
    return transpose<float,4,4>(a);
}
float3x4 transpose(float4x3 a)
{
    return transpose<float,4,3>(a);
}
float2x4 transpose(float4x2 a)
{
    return transpose<float,4,2>(a);
}
float1x4 transpose(float4x1 a)
{
    return transpose<float,4,1>(a);
}
float4x3 transpose(float3x4 a)
{
    return transpose<float,3,4>(a);
}
float3x3 transpose(float3x3 a)
{
    return transpose<float,3,3>(a);
}
float2x3 transpose(float3x2 a)
{
    return transpose<float,3,2>(a);
}
float1x3 transpose(float3x1 a)
{
    return transpose<float,3,1>(a);
}
float4x2 transpose(float2x4 a)
{
    return transpose<float,2,4>(a);
}
float3x2 transpose(float2x3 a)
{
    return transpose<float,2,3>(a);
}
float2x2 transpose(float2x2 a)
{
    return transpose<float,2,2>(a);
}
float1x2 transpose(float2x1 a)
{
    return transpose<float,2,1>(a);
}
float4x1 transpose(float1x4 a)
{
    return transpose<float,1,4>(a);
}
float3x1 transpose(float1x3 a)
{
    return transpose<float,1,3>(a);
}
float2x1 transpose(float1x2 a)
{
    return transpose<float,1,2>(a);
}
float1x1 transpose(float1x1 a)
{
    return transpose<float,1,1>(a);
}

double4x4 transpose(double4x4 a)
{
    return transpose<double,4,4>(a);
}
double3x4 transpose(double4x3 a)
{
    return transpose<double,4,3>(a);
}
double2x4 transpose(double4x2 a)
{
    return transpose<double,4,2>(a);
}
double1x4 transpose(double4x1 a)
{
    return transpose<double,4,1>(a);
}
double4x3 transpose(double3x4 a)
{
    return transpose<double,3,4>(a);
}
double3x3 transpose(double3x3 a)
{
    return transpose<double,3,3>(a);
}
double2x3 transpose(double3x2 a)
{
    return transpose<double,3,2>(a);
}
double1x3 transpose(double3x1 a)
{
    return transpose<double,3,1>(a);
}
double4x2 transpose(double2x4 a)
{
    return transpose<double,2,4>(a);
}
double3x2 transpose(double2x3 a)
{
    return transpose<double,2,3>(a);
}
double2x2 transpose(double2x2 a)
{
    return transpose<double,2,2>(a);
}
double1x2 transpose(double2x1 a)
{
    return transpose<double,2,1>(a);
}
double4x1 transpose(double1x4 a)
{
    return transpose<double,1,4>(a);
}
double3x1 transpose(double1x3 a)
{
    return transpose<double,1,3>(a);
}
double2x1 transpose(double1x2 a)
{
    return transpose<double,1,2>(a);
}
double1x1 transpose(double1x1 a)
{
    return transpose<double,1,1>(a);
}

} // namespace Cg

