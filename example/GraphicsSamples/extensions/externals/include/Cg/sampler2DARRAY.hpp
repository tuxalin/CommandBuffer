/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __sampler2DARRAY_hpp__
#define __sampler2DARRAY_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsampler2DARRAY_state;

class sampler2DARRAY {
    __CGsampler2DARRAY_state *state;

public:
    sampler2DARRAY();
    sampler2DARRAY(int texUnit);
    sampler2DARRAY(const sampler2DARRAY &src);
    ~sampler2DARRAY();
    sampler2DARRAY & operator = (const sampler2DARRAY &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 tex2DARRAY(sampler2DARRAY, float3 str);
float4 tex2DARRAY(sampler2DARRAY, float4 strq);
float4 tex2DARRAY(sampler2DARRAY, float3 str, float3 dx, float3 dy);
float4 tex2DARRAY(sampler2DARRAY, float4 strq, float4 dx, float4 dy);
float4 tex2DARRAYproj(sampler2DARRAY, float4 strq);

} // namespace Cg

#endif // __sampler2DARRAY_hpp__
