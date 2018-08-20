/* 
 * Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/double.hpp>
#include <Cg/upper4x3.hpp>

namespace Cg {

float4x3 upper4x3(float1x1 m)
{
    return upper4x3<float,1,1>(m);
}

float4x3 upper4x3(float1x2 m)
{
    return upper4x3<float,1,2>(m);
}

float4x3 upper4x3(float1x3 m)
{
    return upper4x3<float,1,3>(m);
}

float4x3 upper4x3(float1x4 m)
{
    return upper4x3<float,1,4>(m);
}

float4x3 upper4x3(float2x1 m)
{
    return upper4x3<float,2,1>(m);
}

float4x3 upper4x3(float2x2 m)
{
    return upper4x3<float,2,2>(m);
}

float4x3 upper4x3(float2x3 m)
{
    return upper4x3<float,2,3>(m);
}

float4x3 upper4x3(float2x4 m)
{
    return upper4x3<float,2,4>(m);
}

float4x3 upper4x3(float3x1 m)
{
    return upper4x3<float,3,1>(m);
}

float4x3 upper4x3(float3x2 m)
{
    return upper4x3<float,3,2>(m);
}

float4x3 upper4x3(float3x3 m)
{
    return upper4x3<float,3,3>(m);
}

float4x3 upper4x3(float3x4 m)
{
    return upper4x3<float,3,4>(m);
}

float4x3 upper4x3(float4x1 m)
{
    return upper4x3<float,4,1>(m);
}

float4x3 upper4x3(float4x2 m)
{
    return upper4x3<float,4,2>(m);
}

float4x3 upper4x3(float4x3 m)
{
    return upper4x3<float,4,3>(m);
}

float4x3 upper4x3(float4x4 m)
{
    return upper4x3<float,4,4>(m);
}

double4x3 upper4x3(double1x1 m)
{
    return upper4x3<double,1,1>(m);
}

double4x3 upper4x3(double1x2 m)
{
    return upper4x3<double,1,2>(m);
}

double4x3 upper4x3(double1x3 m)
{
    return upper4x3<double,1,3>(m);
}

double4x3 upper4x3(double1x4 m)
{
    return upper4x3<double,1,4>(m);
}

double4x3 upper4x3(double2x1 m)
{
    return upper4x3<double,2,1>(m);
}

double4x3 upper4x3(double2x2 m)
{
    return upper4x3<double,2,2>(m);
}

double4x3 upper4x3(double2x3 m)
{
    return upper4x3<double,2,3>(m);
}

double4x3 upper4x3(double2x4 m)
{
    return upper4x3<double,2,4>(m);
}

double4x3 upper4x3(double3x1 m)
{
    return upper4x3<double,3,1>(m);
}

double4x3 upper4x3(double3x2 m)
{
    return upper4x3<double,3,2>(m);
}

double4x3 upper4x3(double3x3 m)
{
    return upper4x3<double,3,3>(m);
}

double4x3 upper4x3(double3x4 m)
{
    return upper4x3<double,3,4>(m);
}

double4x3 upper4x3(double4x1 m)
{
    return upper4x3<double,4,1>(m);
}

double4x3 upper4x3(double4x2 m)
{
    return upper4x3<double,4,2>(m);
}

double4x3 upper4x3(double4x3 m)
{
    return upper4x3<double,4,3>(m);
}

double4x3 upper4x3(double4x4 m)
{
    return upper4x3<double,4,4>(m);
}

} // namespace Cg

