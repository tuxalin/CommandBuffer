/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __samplerRECT_hpp__
#define __samplerRECT_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsamplerRECT_state;

class samplerRECT {
    __CGsamplerRECT_state *state;

public:
    samplerRECT();
    samplerRECT(int texUnit);
    samplerRECT(const samplerRECT &src);
    ~samplerRECT();
    samplerRECT & operator = (const samplerRECT &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 texRECT(samplerRECT, float2 st);
float4 texRECT(samplerRECT, float2 st, float2 dx, float2 dy);
float4 texRECT(samplerRECT, float3 str);
float4 texRECT(samplerRECT, float3 str, float3 dx, float3 dy);
float4 texRECTproj(samplerRECT, float4 strq);

} // namespace Cg

#endif // __samplerRECT_hpp__
