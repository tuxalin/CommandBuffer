/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __sampler1D_hpp__
#define __sampler1D_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsampler1D_state;

class sampler1D {
    __CGsampler1D_state *state;

public:
    sampler1D();
    sampler1D(int texUnit);
    sampler1D(const sampler1D &src);
    ~sampler1D();
    sampler1D & operator = (const sampler1D &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 tex1D(sampler1D, float1 s);
float4 tex1D(sampler1D, float1 s, float1 dx, float1 dy);
float4 tex1D(sampler1D, float2 st);
float4 tex1D(sampler1D, float2 st, float1 dx, float1 dy);
float4 tex1Dproj(sampler1D, float2 sq);
float4 tex1Dproj(sampler1D, float3 srq);

} // namespace Cg

#endif // __sampler1D_hpp__
