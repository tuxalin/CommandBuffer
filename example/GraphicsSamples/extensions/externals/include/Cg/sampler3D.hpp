/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __sampler3D_hpp__
#define __sampler3D_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsampler3D_state;

class sampler3D {
    __CGsampler3D_state *state;

public:
    sampler3D();
    sampler3D(int texUnit);
    sampler3D(const sampler3D &src);
    ~sampler3D();
    sampler3D & operator = (const sampler3D &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 tex3D(sampler3D, float3 str);
float4 tex3D(sampler3D, float3 str, float3 dx, float3 dy);
float4 tex3Dproj(sampler3D, float4 strq);

} // namespace Cg

#endif // __sampler3D_hpp__
