/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/cross.hpp>

namespace Cg {

float3 cross(float3 a, float3 b)
{
    return cross<float>(a, b);
}

// int3 version not officially part of Cg runtime
int3 cross(int3 a, int3 b)
{
    return cross<int>(a, b);
}

double3 cross(double3 a, double3 b)
{
    return cross<double>(a, b);
}

half3 cross(half3 a, half3 b)
{
    return cross<half>(a, b);
}

fixed3 cross(fixed3 a, fixed3 b)
{
    return cross<fixed>(a, b);
}

} // namespace Cg

