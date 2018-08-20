/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __samplerBUF_hpp__
#define __samplerBUF_hpp__

#include <Cg/vector.hpp>

namespace Cg {

class __CGsamplerBUF_state;

class samplerBUF {
    __CGsamplerBUF_state *state;

public:
    samplerBUF();
    samplerBUF(int texUnit);
    samplerBUF(const samplerBUF &src);
    ~samplerBUF();
    samplerBUF & operator = (const samplerBUF &rhs);
    float4 sample(float4 strq, float lod);
};

// Cg Standard Library texture functions

float4 texBUF(samplerBUF, float1 s);

} // namespace Cg

#endif // __samplerBUF_hpp__
