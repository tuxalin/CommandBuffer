/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __sampler1DARRAY_hpp__
#define __sampler1DARRAY_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsampler1DARRAY_state;

class sampler1DARRAY {
    __CGsampler1DARRAY_state *state;

public:
    sampler1DARRAY();
    sampler1DARRAY(const sampler1DARRAY &src);
    ~sampler1DARRAY();
    sampler1DARRAY & operator = (const sampler1DARRAY &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 tex1DARRAY(sampler1DARRAY, float2 st);
float4 tex1DARRAY(sampler1DARRAY, float2 st, float2 dx, float2 dy);
float4 tex1DARRAY(sampler1DARRAY, float3 str);
float4 tex1DARRAY(sampler1DARRAY, float3 str, float3 dx, float3 dy);
float4 tex1DARRAYproj(sampler1DARRAY, float3 str);
float4 tex1DARRAYproj(sampler1DARRAY, float4 strq);

} // namespace Cg

#endif // __sampler1DARRAY_hpp__
