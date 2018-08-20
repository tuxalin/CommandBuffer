/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/isfinite.hpp>

namespace Cg {

bool isfinite(float v)
{
    return isfinite<float>(v);
}

bool1 isfinite(float1 v)
{
    return isfinite<float,1>(v);
}

bool2 isfinite(float2 v)
{
    return isfinite<float,2>(v);
}

bool3 isfinite(float3 v)
{
    return isfinite<float,3>(v);
}

bool4 isfinite(float4 v)
{
    return isfinite<float,4>(v);
}

bool isfinite(double v)
{
    return isfinite<double>(v);
}

bool1 isfinite(double1 v)
{
    return isfinite<double,1>(v);
}

bool2 isfinite(double2 v)
{
    return isfinite<double,2>(v);
}

bool3 isfinite(double3 v)
{
    return isfinite<double,3>(v);
}

bool4 isfinite(double4 v)
{
    return isfinite<double,4>(v);
}

bool isfinite(half v)
{
    return isfinite<half>(v);
}

bool1 isfinite(half1 v)
{
    return isfinite<half,1>(v);
}

bool2 isfinite(half2 v)
{
    return isfinite<half,2>(v);
}

bool3 isfinite(half3 v)
{
    return isfinite<half,3>(v);
}

bool4 isfinite(half4 v)
{
    return isfinite<half,4>(v);
}

bool isfinite(fixed v)
{
    return isfinite<fixed>(v);
}

bool1 isfinite(fixed1 v)
{
    return isfinite<fixed,1>(v);
}

bool2 isfinite(fixed2 v)
{
    return isfinite<fixed,2>(v);
}

bool3 isfinite(fixed3 v)
{
    return isfinite<fixed,3>(v);
}

bool4 isfinite(fixed4 v)
{
    return isfinite<fixed,4>(v);
}

} // namespace Cg

