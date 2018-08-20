/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/intBitsToFloat.hpp>

namespace Cg {

float intBitsToFloat(int v)
{
    return intBitsToFloat<int,1>(v);
}

float1 intBitsToFloat(int1 v)
{
    return intBitsToFloat<int,1>(v);
}

float2 intBitsToFloat(int2 v)
{
    return intBitsToFloat<int,2>(v);
}

float3 intBitsToFloat(int3 v)
{
    return intBitsToFloat<int,3>(v);
}

float4 intBitsToFloat(int4 v)
{
    return intBitsToFloat<int,4>(v);
}

} // namespace Cg
