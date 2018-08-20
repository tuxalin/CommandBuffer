//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvMaterialGL.h
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
#ifndef NVGLMATERIAL_H_
#define NVGLMATERIAL_H_
#pragma once

#include "NV/NvPlatformGL.h"
#include "NV/NvMath.h"

#include <string>

namespace Nv
{
    class NvModelExt;

    // Simple class to hold values for a material, which can be 
    // used when rendering an NvGLMeshExt
    class NvMaterialGL
    {
    public:
        NvMaterialGL(){}

        /// Initializes material attributes from the material in the given
        /// model at the given index.
        /// \param[in] pModel Pointer to the source model that contains the
        ///             material definitions.
        /// \param[in] materialIndex Index of the material to get properties from
        /// \return True if the material could be initialized from the requested
        ///         model and material index.  False if there was a problem and
        ///         the material could not be initialized.
        bool InitFromMaterial(NvModelExt* pModel, uint32_t materialIndex);

        // Material Name
        std::string m_name;

        // Ambient Color
        nv::vec3f m_ambient;

        // Diffuse Color
        nv::vec3f m_diffuse;

        // Emissive Color
        nv::vec3f m_emissive;

        // Specular Color
        nv::vec3f m_specular;

        // Specular Power
        uint32_t m_shininess;

        // Translucency
        float m_alpha;

        // Diffuse Texture Map
        GLuint m_diffuseTexture;

        // Bump (Normal) Texture Map
        GLuint m_bumpMapTexture;
    };
}

#endif