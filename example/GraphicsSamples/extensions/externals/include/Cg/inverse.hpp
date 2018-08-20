
/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_inverse_hpp__
#define __Cg_inveres_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

extern float1x1 inverse(float1x1 a);
extern float2x2 inverse(float2x2 a);
extern float3x3 inverse(float3x3 a);
extern float4x4 inverse(float4x4 a);

#ifdef __Cg_double_hpp__
extern double1x1 inverse(double1x1 a);
extern double2x2 inverse(double2x2 a);
extern double3x3 inverse(double3x3 a);
extern double4x4 inverse(double4x4 a);
#endif

} // namespace Cg

#endif // __Cg_inverse_hpp__
