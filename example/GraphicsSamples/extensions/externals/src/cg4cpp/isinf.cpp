/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/isinf.hpp>

namespace Cg {

bool1 isinf(float1 v)
{
    return isinf<float,1>(v);
}

bool2 isinf(float2 v)
{
    return isinf<float,2>(v);
}

bool3 isinf(float3 v)
{
    return isinf<float,3>(v);
}

bool4 isinf(float4 v)
{
    return isinf<float,4>(v);
}

bool1 isinf(double1 v)
{
    return isinf<double,1>(v);
}

bool2 isinf(double2 v)
{
    return isinf<double,2>(v);
}

bool3 isinf(double3 v)
{
    return isinf<double,3>(v);
}

bool4 isinf(double4 v)
{
    return isinf<double,4>(v);
}

bool1 isinf(half1 v)
{
    return isinf<half,1>(v);
}

bool2 isinf(half2 v)
{
    return isinf<half,2>(v);
}

bool3 isinf(half3 v)
{
    return isinf<half,3>(v);
}

bool4 isinf(half4 v)
{
    return isinf<half,4>(v);
}

bool1 isinf(fixed1 v)
{
    return isinf<fixed,1>(v);
}

bool2 isinf(fixed2 v)
{
    return isinf<fixed,2>(v);
}

bool3 isinf(fixed3 v)
{
    return isinf<fixed,3>(v);
}

bool4 isinf(fixed4 v)
{
    return isinf<fixed,4>(v);
}

} // namespace Cg

