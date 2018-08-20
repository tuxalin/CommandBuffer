/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __sampler2D_hpp__
#define __sampler2D_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsampler2D_state;

class __CGsampler2D_factory {
public:
    virtual __CGsampler2D_state *construct() = 0;
};

class sampler2D {
    __CGsampler2D_state *state;

public:
    sampler2D();
    sampler2D(const sampler2D &src);
    sampler2D(__CGsampler2D_factory &src);
    ~sampler2D();
    sampler2D & operator = (const sampler2D &rhs);
    float4 sample(float4 strq, float lod);
    void update();
};

// Cg Standard Library texture functions

float4 tex2D(sampler2D, float2 st);
float4 tex2D(sampler2D, float2 st, float2 dx, float2 dy);
float4 tex2D(sampler2D, float3 str);
float4 tex2D(sampler2D, float3 str, float3 dx, float3 dy);
float4 tex2Dproj(sampler2D, float3 str);
float4 tex2Dproj(sampler2D, float4 strq);

} // namespace Cg

#endif // __sampler2D_hpp__
