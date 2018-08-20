/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/floatToRawIntBits.hpp>

namespace Cg {

int floatToRawIntBits(float v)
{
    return floatToRawIntBits<float,1>(v);
}

int1 floatToRawIntBits(float1 v)
{
    return floatToRawIntBits<float,1>(v);
}

int2 floatToRawIntBits(float2 v)
{
    return floatToRawIntBits<float,2>(v);
}

int3 floatToRawIntBits(float3 v)
{
    return floatToRawIntBits<float,3>(v);
}

int4 floatToRawIntBits(float4 v)
{
    return floatToRawIntBits<float,4>(v);
}

} // namespace Cg
