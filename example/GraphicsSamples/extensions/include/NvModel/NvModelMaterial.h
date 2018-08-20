//----------------------------------------------------------------------------------
// File:        NvModel/NvModelMaterial.h
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
#ifndef _NVMODELMATERIAL_H_
#define _NVMODELMATERIAL_H_
#include "NV/NvMath.h"
#include <vector>

namespace Nv
{
    // Structure to hold a texture index with associated sampling and addressing parameters
    struct TextureDesc
    {
        enum MapMode
        {
            MapMode_Wrap = 0x0, /// Wrapping
            MapMode_Clamp = 0x1, /// Clamp addressing
            MapMode_Mirror = 0x2, /// Mirror-wrapped addressing
            MapMode_Force32Bit = 0x7FFFFFFF

        };

        enum FilterMode
        {
            FilterMode_Nearest = 0x0, /// Nearest filtering
            FilterMode_Linear = 0x1, /// Lerp filtering
            FilterMode_Force32Bit = 0x7FFFFFFF

        };

        int32_t m_textureIndex;
        int32_t m_UVIndex;
        MapMode m_mapModes[3];
        FilterMode m_minFilter;
    };
    typedef std::vector<TextureDesc> TextureDescArray;

    // Simple class to hold values for a material, which are usually
    // read in from an OBJ .mtl file.  
    class Material
    {
    public:
        Material() :
            m_ambient(1.0f, 1.0f, 1.0f),
            m_diffuse(1.0f, 1.0f, 1.0f),
            m_emissive(0.0f, 0.0f, 0.0f),
            m_alpha(1.0f),
            m_specular(1.0f, 1.0f, 1.0f),
            m_shininess(1),
            m_opticalDensity(1.0f),
            m_transmissionFilter(1.0f, 1.0f, 1.0f),
            m_illumModel(HighlightOn)
        {}

        /// Ambient surface color
        nv::vec3f m_ambient;

        /// Diffuse surface color
        nv::vec3f m_diffuse;

        /// Emissive surface color
        nv::vec3f m_emissive;

        /// Object translucency
        float m_alpha;

        /// Specular surface color
        nv::vec3f m_specular;

        /// Surface specular power
        uint32_t m_shininess;

        /// Surface optical density/refraction index
        float m_opticalDensity;

        /// Surface color transmission filter
        nv::vec3f m_transmissionFilter;

        /// Index of ambient texture
        TextureDescArray m_ambientTextures;

        /// Index of diffuse texture
        TextureDescArray m_diffuseTextures;

        /// Index of specular texture
        TextureDescArray m_specularTextures;

        /// Index of bump/normal map texture
        TextureDescArray m_bumpMapTextures;

        /// Index of reflection texture
        TextureDescArray m_reflectionTextures;

        /// Index of displacement map texture
        TextureDescArray m_displacementMapTextures;

        /// Index of specular power texture
        TextureDescArray m_specularPowerTextures;

        /// Index of translucency map texture
        TextureDescArray m_alphaMapTextures;

        /// Index of decal texture
        TextureDescArray m_decalTextures;

        enum IlluminationModel
        {
            ColorOn_AmbientOff = 0,
            ColorOn_AmbientOn,
            HighlightOn,    // Specular highlights, so ambient+diffuse+specular
            ReflectionOn_RayTraceOn,
            GlassOn_RayTraceOn,
            FresnelOn_RayTraceOn,
            RefractionOn_FresnelOff_RayTraceOn,
            RefractionOn_FresnelOn_RayTraceOn,
            ReflectionOn_RayTraceOff,
            GlassOn_RayTraceOff,
            CastShadowsOntoInvisibleSurfaces,
            Force32Bit = 0x7FFFFFFF
        };
        IlluminationModel m_illumModel;
    };
};
#endif
