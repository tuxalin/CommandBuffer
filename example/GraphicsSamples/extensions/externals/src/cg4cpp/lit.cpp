/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/vector/xyzw.hpp>
#include <Cg/stdlib.hpp>

#include <cmath>  // for std::pow

namespace Cg {

template <typename T>
static inline __CGvector<T,4> lit(const __CGvector<T,1> & NdotL, const __CGvector<T,1> & NdotH, const __CGvector<T,1> & m) // XXXmjk should be vectors??
{
    __CGvector<T,4> rv;
    rv.x = T(1);
    if (NdotL < 0) {
        rv.y = 0;
        rv.z = 0;
    } else {
        rv.y = NdotL;
        if (NdotH < 0) {
            rv.z = 0;
        } else {
            rv.z = std::pow(float1(NdotH), float1(m));  // revisit for double
        }
    }
    rv.w = 1;
    return rv;
}

float4 lit(float1 NdotL, float1 NdotH, float1 m)
{
  return lit<float>(NdotL, NdotH, m);
}

half4 lit(half1 NdotL, half1 NdotH, half1 m)
{
  return lit<half>(NdotL, NdotH, m);
}

fixed4 lit(fixed1 NdotL, fixed1 NdotH, fixed1 m)
{
  return lit<fixed>(NdotL, NdotH, m);
}

} // namespace Cg

