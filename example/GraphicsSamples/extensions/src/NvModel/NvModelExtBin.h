//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtBin.h
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
#ifndef _NVMODELEXTBIN_H_
#define _NVMODELEXTBIN_H_

#include "NvModel/NvModelMaterial.h"
#include "NvModelSubMeshBin.h"

#include "NV/NvMath.h"
#include <vector>
#include <string>

namespace Nv
{
	class NvModelExtBin : public NvModelExt
	{
	public:
		virtual ~NvModelExtBin();

		static NvModelExtBin* Create(const char* pFileName);

		/// Returns the number of meshes contained in the model
		/// \return Number of meshes contained in the model
		virtual uint32_t GetMeshCount() const { return m_meshCount; }

		/// Returns the mesh contained in the model with the given ID
		/// \param[in] subMeshID ID, or index, of the mesh to retrieve
		/// \return A pointer to the mesh with the given ID.  Null if
		///         no mesh exists with that ID.
		virtual SubMesh* GetSubMesh(uint32_t subMeshID) {
			return m_subMeshes + subMeshID;
		}

		/// Returns the number of materials defined by the model
		/// \return Number of materials defined by the model
		virtual uint32_t GetMaterialCount() const { return m_materialCount; }

		/// Returns the material in the model with the given ID
		/// \param[in] materialID ID, or index, of the material to retrieve
		/// \return A pointer to the material with the given ID.  Null if
		///         no material exists with that ID.
		virtual Material* GetMaterial(uint32_t materialID) {
			return m_materials + materialID;
		}

        /// Returns the number of textures used by the model
        /// \return Number of textures used by the model
        uint32_t GetTextureCount() const { return m_textureCount; }

        /// Returns the name of the texture in the model with the given ID
        /// \param[in] texture ID, or index, of the texture to retrieve
        /// \return A string containing the file name of the texture with
        ///         the given ID.  Null if no texture exists with that ID.
        virtual std::string GetTextureName(uint32_t textureID) const
        {
            if (textureID >= m_textureCount)
            {
                return std::string("");
            }
            return m_textures[textureID];
        }

	protected:
		NvModelExtBin();

        bool LoadFromPreprocessed(uint8_t* data);
        bool LoadFromPreprocessed_v1(uint8_t* data);
        bool LoadFromPreprocessed_v2(uint8_t* data);
        bool LoadFromPreprocessed_v3(uint8_t* data);
        bool LoadFromPreprocessed_v4(uint8_t* data);

        uint8_t* ReadTextureBlock(uint8_t* pTextureBlock);
        uint8_t* ReadSkeletonBlock(uint8_t* pSkeletonBlock);
        uint8_t* ReadMaterials(uint8_t* pMaterials);
        uint8_t* ReadMeshes_v3(uint8_t* pMeshes);
        uint8_t* ReadMeshes(uint8_t* pMeshes);


		Material* m_materials;
		uint32_t m_materialCount;

        // Array of all textures defined by all materials in the mesh
        std::vector<std::string> m_textures;
		uint32_t m_textureCount;
		
		SubMeshBin* m_subMeshes;
		uint32_t m_meshCount;
	};
}
#endif // _NVMODELEXT_H_
