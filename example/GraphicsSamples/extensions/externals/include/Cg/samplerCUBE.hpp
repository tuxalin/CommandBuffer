/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __samplerCUBE_hpp__
#define __samplerCUBE_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsamplerCUBE_state;

class samplerCUBE {
    __CGsamplerCUBE_state *state;

public:
    samplerCUBE();
    samplerCUBE(int texUnit);
    samplerCUBE(const samplerCUBE &src);
    ~samplerCUBE();
    samplerCUBE & operator = (const samplerCUBE &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 texCUBE(samplerCUBE, float3 str);
float4 texCUBE(samplerCUBE, float3 str, float3 dx, float3 dy);
float4 texCUBEproj(samplerCUBE, float4 strq);

} // namespace Cg

#endif // __samplerCUBE_hpp__
