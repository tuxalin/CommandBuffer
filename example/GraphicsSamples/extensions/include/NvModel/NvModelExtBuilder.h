//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtBuilder.h
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
#ifndef _NVMODELEXTBUILDER_H_
#define _NVMODELEXTBUILDER_H_

#include "NvModel/NvModelExt.h"
#include "NvModel/NvModelMaterial.h"
#include "NvModel/NvModelSubMeshBuilder.h"

#include "NV/NvMath.h"
#include <vector>
#include <string>

namespace Nv
{
	class NvModelExtBuilder : public NvModelExt
	{
	public:
        virtual ~NvModelExtBuilder();

		/// Create a builder object
		/// \return an allocated build object
        static NvModelExtBuilder* Create();

		/// Returns the number of meshes contained in the model
		/// \return Number of meshes contained in the model
        virtual uint32_t GetMeshCount() const { return m_subMeshes.size(); }

		/// Returns the mesh contained in the model with the given ID
		/// \param[in] subMeshID ID, or index, of the mesh to retrieve
		/// \return A pointer to the mesh with the given ID.  Null if
		///         no mesh exists with that ID.
		virtual SubMesh* GetSubMesh(uint32_t subMeshID)
        {
            if (subMeshID >= m_subMeshes.size())
            {
                return NULL;
            }

            return m_subMeshes[subMeshID]; 
        }

		/// Returns the number of materials defined by the model
		/// \return Number of materials defined by the model
		virtual uint32_t GetMaterialCount() const { return m_materials.size(); }

		/// Returns the material in the model with the given ID
		/// \param[in] materialID ID, or index, of the material to retrieve
		/// \return A pointer to the material with the given ID.  Null if
		///         no material exists with that ID.
		virtual Material* GetMaterial(uint32_t materialID) 
        { 
            if (materialID >= m_materials.size())
            {
                return NULL;
            }
            return m_materials[materialID]; 
        }

        /// Returns the number of textures used by the model
        /// \return Number of textures used by the model
        virtual uint32_t GetTextureCount() const { return m_textures.size(); }

        /// Returns the name of the texture in the model with the given ID
        /// \param[in] texture ID, or index, of the texture to retrieve
        /// \return A string containing the file name of the texture with
        ///         the given ID.  Null if no texture exists with that ID.
        virtual std::string GetTextureName(uint32_t textureID) const
        {
            if (textureID >= m_textures.size())
            {
                return std::string("");
            }
            return m_textures[textureID];
        }

        /// Many source meshes will come in referring to their textures with a path
        /// that is absolute or otherwise does not match its location on the runtime
        /// system.  We provide this option to strip any path information from the 
        /// texture names contained in the mesh, leaving only a filename and extension.
        void StripTextureNamePaths()
        {
            std::vector<std::string>::iterator textureIt = m_textures.begin();
            std::vector<std::string>::iterator textureEnd = m_textures.end();
            for (; textureIt != textureEnd; ++textureIt)
            {
                std::string& filename = *textureIt;
                int delimiterPos = filename.find_last_of("/");
                if (delimiterPos != std::string::npos)
                {
                    filename = filename.substr(delimiterPos + 1, std::string::npos);
                }
                delimiterPos = filename.find_last_of("\\");
                if (delimiterPos != std::string::npos)
                {
                    filename = filename.substr(delimiterPos + 1, std::string::npos);
                }
                delimiterPos = filename.find_last_of(":");
                if (delimiterPos != std::string::npos)
                {
                    filename = filename.substr(delimiterPos + 1, std::string::npos);
                }
            }
        }

        /// Since we only load .dds files at runtime, we need to be able to re-target
        /// textures referred to by materials to their .dds counterparts.
        void ConvertTextureNamesToDDS()
        {
            std::vector<std::string>::iterator textureIt = m_textures.begin();
            std::vector<std::string>::iterator textureEnd = m_textures.end();
            for (; textureIt != textureEnd; ++textureIt)
            {
                std::string& filename = *textureIt;
                int extPos = filename.find_last_of(".");
                if (extPos != std::string::npos)
                {
                    filename = filename.substr(0, extPos);
                }
                filename += ".dds";
            }
        }

        /// NvAssetLoader insists that all assets must be a direct child of an "assets"
        /// directory.  In order to keep all of the assets from being a jumbled mess,
        /// subdirectories are often created in that "assets" directory.  If the texture
        /// name does not contain the relative path from its containing assets directory,
        /// the loader will never find it.  Thus this method which allows you to prepend
        /// a string, such as "textures/", to the name of each texture used by the model.
        /// \param prefix Prefix string to be appended to each texture name used by the model.
        ///               If the intent is to prepend a path to the texture, make sure to 
        ///               include the trailing "/" character.
        void PrependToTextureNames(const std::string& prefix)
        {
            std::vector<std::string>::iterator textureIt = m_textures.begin();
            std::vector<std::string>::iterator textureEnd = m_textures.end();
            for (; textureIt != textureEnd; ++textureIt)
            {
                std::string& filename = *textureIt;
                filename = prefix + filename;
            }
        }

        int32_t GetTextureId(const std::string& name, bool bAdd = true);

        void UpdateBoundingBox();

        void SetSkeleton(NvSkeleton* pSkeleton) { m_pSkeleton = pSkeleton; }
		std::vector<Material*> m_materials;
        std::vector<std::string> m_textures;
		std::vector<SubMeshBuilder*> m_subMeshes;

	protected:
        NvModelExtBuilder();
	};
}
#endif // _NVMODELEXTBUILDER_H_
