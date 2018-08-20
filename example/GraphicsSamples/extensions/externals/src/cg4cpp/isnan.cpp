/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/isnan.hpp>

namespace Cg {

bool isnan(float v)
{
    return isnan<float>(v);
}

bool1 isnan(float1 v)
{
    return isnan<float,1>(v);
}

bool2 isnan(float2 v)
{
    return isnan<float,2>(v);
}

bool3 isnan(float3 v)
{
    return isnan<float,3>(v);
}

bool4 isnan(float4 v)
{
    return isnan<float,4>(v);
}

bool isnan(double v)
{
    return isnan<double>(v);
}

bool1 isnan(double1 v)
{
    return isnan<double,1>(v);
}

bool2 isnan(double2 v)
{
    return isnan<double,2>(v);
}

bool3 isnan(double3 v)
{
    return isnan<double,3>(v);
}

bool4 isnan(double4 v)
{
    return isnan<double,4>(v);
}

bool isnan(half v)
{
    return isnan<half>(v);
}

bool1 isnan(half1 v)
{
    return isnan<half,1>(v);
}

bool2 isnan(half2 v)
{
    return isnan<half,2>(v);
}

bool3 isnan(half3 v)
{
    return isnan<half,3>(v);
}

bool4 isnan(half4 v)
{
    return isnan<half,4>(v);
}

bool isnan(fixed v)
{
    return isnan<fixed>(v);
}

bool1 isnan(fixed1 v)
{
    return isnan<fixed,1>(v);
}

bool2 isnan(fixed2 v)
{
    return isnan<fixed,2>(v);
}

bool3 isnan(fixed3 v)
{
    return isnan<fixed,3>(v);
}

bool4 isnan(fixed4 v)
{
    return isnan<fixed,4>(v);
}

} // namespace Cg

