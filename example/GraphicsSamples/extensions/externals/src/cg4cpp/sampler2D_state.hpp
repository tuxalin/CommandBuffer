
/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */
/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include "sampler_state.hpp"

namespace Cg {

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(push)
#pragma warning(disable:4512)  // assignment operator could not be generated
#endif

class __CGsampler2D_state : public __CGsampler_state {
    __CGimage image[maxLevels];

    virtual void initImages();

    virtual ~__CGsampler2D_state() { }

    virtual float4 linearFilter(int level, float4 strq);
    virtual float4 nearestFilter(int level, float4 strq);

    virtual float4 nearest(const __CGimage &image, float4 strq);
    virtual float4 linear(const __CGimage &image, float4 strq);

public:
    __CGsampler2D_state() {
        initDerivedSampler(GL_TEXTURE_2D);
    }
    __CGsampler2D_state(int unit) {
        initDerivedSampler(GL_TEXTURE_2D, unit);
    }
    float4 sample(float4 strq, float lod);
    int3 size(int1 lod);
};

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(pop)
#endif

} // namespace Cg
