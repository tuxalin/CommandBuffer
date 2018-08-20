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

#ifndef __sampler_state_hpp__
#define __sampler_state_hpp__

#include <assert.h>

#include <Cg/vector/xyzw.hpp>
#include <Cg/vector/rgba.hpp>
#include <Cg/vector/stpq.hpp>
#include <Cg/vector.hpp>
#include <Cg/inout.hpp>
#include <Cg/floor.hpp>
#include <Cg/max.hpp>
#include <Cg/pow.hpp>
#include <Cg/frac.hpp>
//#include <Cg/clamp.hpp>
//#include <Cg/lerp.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/sampler.hpp>

// All this cruft just to include the Windows <GL/gl.h> without including <windows.h>
#ifdef _WIN32
#  ifndef APIENTRY
#   define __CG_APIENTRY_DEFINED
    /* Cygwin and MingW32 are two free GNU-based Open Source compilation
       environments for Win32.  Note that __CYGWIN32__ is deprecated
       in favor of simply __CYGWIN__. */
#   if defined(__MINGW32__) || defined(__CYGWIN32__) || defined(__CYGWIN__)
#    ifdef i386
#     define APIENTRY __attribute__ ((stdcall)) 
#    else
#     define APIENTRY
#    endif
#   else
#    if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#     define APIENTRY __stdcall
#    else
#     define APIENTRY
#    endif
#   endif
#  endif
   /* This is from Win32's <wingdi.h> and <winnt.h> */
#  ifndef WINGDIAPI
#   define __CG_WINGDIAPI_DEFINED
#   if defined(__MINGW32__) || defined(__CYGWIN32__) || defined(__CYGWIN__)
#    define WINGDIAPI
#   else
#    define WINGDIAPI __declspec(dllimport)
#   endif
#  endif
#endif
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
#else
# include <GL/gl.h>
# include <GL/glext.h>
#endif

#ifdef __CG_APIENTRY_DEFINED
# undef __CG_APIENTRY_DEFINED
# undef APIENTRY
#endif

#ifdef __CG_WINGDIAPI_DEFINED
# undef __CG_WINGDIAPI_DEFINED
# undef WINGDIAPI
#endif

// OpenGL tokens
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_CLAMP_TO_BORDER                0x812D
#define GL_MIRRORED_REPEAT                0x8370
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_COMPARE_R_TO_TEXTURE           0x884E
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_MIRROR_CLAMP_EXT               0x8742
#define GL_MIRROR_CLAMP_TO_EDGE_EXT       0x8743
#define GL_MIRROR_CLAMP_TO_BORDER_EXT     0x8912
#define GL_SIGNED_RGBA_NV                 0x86FB
#define GL_SIGNED_RGBA8_NV                0x86FC
#define GL_SIGNED_RGB_NV                  0x86FE
#define GL_SIGNED_RGB8_NV                 0x86FF
#define GL_SIGNED_LUMINANCE_NV            0x8701
#define GL_SIGNED_LUMINANCE8_NV           0x8702
#define GL_SIGNED_LUMINANCE_ALPHA_NV      0x8703
#define GL_SIGNED_LUMINANCE8_ALPHA8_NV    0x8704
#define GL_SIGNED_ALPHA_NV                0x8705
#define GL_SIGNED_ALPHA8_NV               0x8706
#define GL_SIGNED_INTENSITY_NV            0x8707
#define GL_SIGNED_INTENSITY8_NV           0x8708
#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV   0x870C
#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV 0x870D
#define GL_TEXTURE_1D_ARRAY_EXT           0x8C18
#define GL_TEXTURE_2D_ARRAY_EXT           0x8C1A
#define GL_TEXTURE_BUFFER_EXT             0x8C2A
#define GL_TEXTURE_BINDING_BUFFER_EXT     0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT 0x8C2D
#define GL_TEXTURE_BUFFER_FORMAT_EXT      0x8C2E

namespace Cg {

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(push)
#pragma warning(disable:4512)  // assignment operator could not be generated
#endif

class __CGimage {
    friend class __CGsampler1D_state;
    friend class __CGsampler1DARRAY_state;  // EXT_texture_array
    friend class __CGsampler2D_state;
    friend class __CGsampler2DARRAY_state;  // EXT_texture_array
    friend class __CGsampler3D_state;
    friend class __CGsamplerCUBE_state;
    friend class __CGsamplerRECT_state;  // ARB_texture_rectangle
    friend class __CGsamplerBUF_state;   // EXT_texture_buffer_object

    __CGimage() : data(NULL) { }  // private constructor; I only play with my friends!
    ~__CGimage() {
        delete [] data;
    }

    // API state
    GLenum internalFormat;
    GLint width, height, depth;
    GLint border;

    // Texture image data (floats returned by glGetTexImage)
    float *data;

    void deriveFormatBasedState();

    // API-derived state
    GLenum format;  // Base format
    int components;  // Number of components (1, 2, 3, or 4)
    bool clamped;  // Should the border values be clamped?
    float4 clampMin, clampMax;  // Range to clamp border values to if clamped is true
    int3 borderSize;  // Size of border in each dimension
    int3 borderlessSize;  // Size of image without border in each dimension
    float3 borderlessSizeF;  // float version of borderlessSize

    void initImage(GLenum target, GLint level, int3 targetBorderSupport);

    float4 fetch(int1 u, int1 v, int1 p, const float4 &borderValues) const;
};

class __CGsampler_state {

    friend class __CGsampler_GL_factory;

    typedef struct {
        GLint swapbytes, rowlength, imageheight;
        GLint skiprows, skippixels, skipimages, alignment;
    } PixelStoreState;

    int refcnt;

protected:
    __CGsampler_state() : refcnt(1) { }

    static const int maxLevels = 13;  // 2^(13-1) = 4096 max base level dimension size

    GLenum texTarget;
    GLenum texUnit;

    // API state
    GLint baseLevel, maxLevel;
    GLfloat minLod, maxLod;
    GLenum magFilter, minFilter;
    GLenum wrapS, wrapT, wrapR;
    GLfloat borderValues[4];
    GLfloat lodBias;
    GLenum compareMode, compareFunc, depthTextureMode; // Just for depth textures

    // API-derived state
    float magnifyTransition;
    float clampedLodBias;
    int trueBaseLevel;
    int effectiveMaxLevel;
    float4 clampedBorderValues;
    GLenum prefilterMode;

    // Static helper methods for initialization
    static void initExtensionFuncs();
    static void savePackPixelStoreState(PixelStoreState &state);
    static void restorePackPixelStoreState(PixelStoreState &state);
    // Static helper methods for wrap modes
    static int modulo(int numer, int denom);
    static int1 mirrorNearest(int numer, int denom);
    static int2 mirrorLinear(int numer, int denom);
    static int1 wrapNearest(GLenum wrapMode, int1 size, float1 coord);
    static int2 wrapLinear(GLenum wrapMode, int1 size, float1 coord, float &wrappedCoord);
    // Static helper methods for filtering
    static float4 linear1D(float4 tex[2], float1 weight);
    static float4 linear2D(float4 tex[2][2], float2 weight);
    static float4 linear3D(float4 tex[2][2][2], float3 weight);

    // Constructor-related methods
    virtual void initSampler();
    virtual void initDerivedSampler(GLenum texTarget);
    virtual void initDerivedSampler(GLenum texTarget, int unit);

    virtual void initImages() = 0;

    virtual float4 prefilter(const float4 &texel, float1 r);

    virtual float4 minify(int ndx, float4 strq, float1 lod);
    virtual float4 magnify(int ndx, float4 strq);

    virtual float4 linearFilter(int level, float4 strq) = 0;
    virtual float4 nearestFilter(int level, float4 strq) = 0;

    virtual float4 nearest(const __CGimage &x, float4 strq) = 0;
    virtual float4 linear(const __CGimage &x, float4 strq) = 0;

    virtual ~__CGsampler_state() { };

public:
    void ref() {
        refcnt++;
    };
    void unref() {
        refcnt--;
        if (0 == refcnt) {
            delete this;
        }
    }
};

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(pop)
#endif

} // namespace Cg

#endif // __sampler_state_hpp__
