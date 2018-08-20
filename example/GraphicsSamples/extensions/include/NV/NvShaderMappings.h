//----------------------------------------------------------------------------------
// File:        NV/NvShaderMappings.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#ifndef NV_SHADER_MAPPINGS_H
#define NV_SHADER_MAPPINGS_H

/// \file
/// This file allows applications to use a single file to declare the uniforms in a
/// GLSL shader AND C++ code.  The idea is to declare the uniforms in one file that
/// is included in both the shader code and the C++ code.  This simplifies the use
/// of Uniform Buffer Objects, and avoids having the C++ struct fall out of sync with
/// the GLSL.  Sync failures between C++ and GLSL can lead to difficult-to-track bugs

#if defined(ANDROID) || defined(LINUX)
#define ALIGN(a)__attribute__ ((aligned (a)))
#else
#define ALIGN(a) __declspec(align(a))
#endif

namespace nv {
// Matrices, must align to 4 vector (16 bytes)
#define SDK_MAT4 ALIGN(16) nv::matrix4f

///@{
/// vectors.
/// vectors, 4-tuples and 3-tuples must align to 16 bytes
///  2-vectors must align to 8 bytes
#define SDK_VEC4 ALIGN(16) nv::vec4f
#define SDK_VEC3 ALIGN(16) nv::vec3f
#define SDK_VEC2 ALIGN(8) nv::vec2f

#define ivec4 ALIGN(16) nv::vec4i
#define ivec3 ALIGN(16) nv::vec3i
#define ivec2 ALIGN(8) nv::vec2i

#define uivec4 ALIGN(16) nv::vec4ui
#define uivec3 ALIGN(16) nv::vec3ui
#define uivec2 ALIGN(8) nv::vec2ui
///@}

///@{
/// scalars.
///  uint can be a typedef
///  bool needs special padding / alignment
#define SDK_BOOL ALIGN(4) nv::boolClass
#define uint unsigned int
///@}

/// class to make uint look like bool to make GLSL packing rules happy
struct boolClass
{
    unsigned int _rep;

    boolClass() : _rep(false) {}
    boolClass( bool b) : _rep(b) {}
    operator bool() { return _rep == 0 ? false : true; }
    boolClass& operator=( bool b) { _rep = b; return *this; }
};

};

#endif
