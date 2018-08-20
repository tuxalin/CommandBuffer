/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_determinant_hpp__
#define __Cg_determinant_hpp__

#ifdef __Cg_stdlib_hpp__
#pragma message("error: include this header file (" __FILE__ ") before <Cg/stdlib.hpp>")
#endif

#include <Cg/matrix.hpp>

namespace Cg {

extern float1 determinant(float1x1 A);
extern float1 determinant(float2x2 A);
extern float1 determinant(float3x3 A);
extern float1 determinant(float4x4 A);
#ifdef __Cg_double_hpp__
extern double1 determinant(double1x1 A);
extern double1 determinant(double2x2 A);
extern double1 determinant(double3x3 A);
extern double1 determinant(double4x4 A);
#endif

} // namespace Cg

#endif // __Cg_determinant_hpp__
