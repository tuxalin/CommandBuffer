/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/floatToIntBits.hpp>

namespace Cg {

int floatToIntBits(float v)
{
    return floatToIntBits<float,1>(v);
}

int1 floatToIntBits(float1 v)
{
    return floatToIntBits<float,1>(v);
}

int2 floatToIntBits(float2 v)
{
    return floatToIntBits<float,2>(v);
}

int3 floatToIntBits(float3 v)
{
    return floatToIntBits<float,3>(v);
}

int4 floatToIntBits(float4 v)
{
    return floatToIntBits<float,4>(v);
}

} // namespace Cg
