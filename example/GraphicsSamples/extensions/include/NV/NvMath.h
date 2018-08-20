//----------------------------------------------------------------------------------
// File:        NV/NvMath.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//----------------------------------------------------------------------------------

//
// Template math library for common 3D functionality
//
// This code is in part deriver from glh, a cross platform glut helper library.
// The copyright for glh follows this notice.
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

/*
    Copyright (c) 2000 Cass Everitt
    Copyright (c) 2000 NVIDIA Corporation
    All rights reserved.

    Redistribution and use in source and binary forms, with or
    without modification, are permitted provided that the following
    conditions are met:

     * Redistributions of source code must retain the above
       copyright notice, this list of conditions and the following
       disclaimer.

     * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials
       provided with the distribution.

     * The names of contributors to this software may not be used
       to endorse or promote products derived from this software
       without specific prior written permission.

       THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
       ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
       LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
       FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
       REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
       INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
       BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
       LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
       CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
       LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
       ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
       POSSIBILITY OF SUCH DAMAGE.


    Cass Everitt - cass@r3.nu
*/

#ifndef NV_MATH_H
#define NV_MATH_H

#include <NvSimpleTypes.h>


#include <math.h>

#include "NvVector.h"
#include "NvMatrix.h"
#include "NvQuaternion.h"

/// \file
/// Overall math class header

#define NV_PI   float(3.1415926535897932384626433832795)

namespace nv {

typedef vec2<float> vec2f; ///< float 2-vectors
typedef vec2<int32_t> vec2i; ///< signed integer 2-vectors
typedef vec2<uint32_t> vec2ui; ///< unsigned integer 2-vectors
typedef vec3<float> vec3f; ///< float 3-vectors
typedef vec3<int32_t> vec3i; ///< signed integer 3-vectors
typedef vec3<uint32_t> vec3ui; ///< unsigned integer 4-vectors
typedef vec4<float> vec4f; ///< float 4-vectors
typedef vec4<int32_t> vec4i; ///< signed integer 4-vectors
typedef vec4<uint32_t> vec4ui; ///< unsigned integer 4-vectors
typedef matrix4<float> matrix4f; ///< float 4x4 matrices
typedef quaternion<float> quaternionf; ///< float quaternions

};

#endif
