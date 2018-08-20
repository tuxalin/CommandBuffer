//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtBin.cpp
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
#include "NvModel/NvModelExt.h"
#include "NvModelExtBin.h"
#include "NvModelExtFile.h"
#include "NvModel/NvSkeleton.h"

namespace Nv
{
	NvModelExtBin::NvModelExtBin()
	{
	}

	NvModelExtBin::~NvModelExtBin()
	{
	}

	NvModelExtBin* NvModelExtBin::Create(const char* pFileName) {
		NvModelExtBin* model = new NvModelExtBin;
		if (NULL == ms_pLoader)
		{
			return NULL;
		}

		// Use the provided loader callback to load the file into memory
		char *pData = ms_pLoader->LoadDataFromFile(pFileName);
		if (NULL == pData)
		{
			return NULL;
		}

		model->LoadFromPreprocessed((uint8_t*)pData);

		// Free the OBJ buffer
		ms_pLoader->ReleaseData(pData);

		return model;
	}

    // Helper function for reading in texture descriptions
    void ReadTextureDescs(NvModelTextureDesc* pSrcDescs, TextureDescArray& destArray, int32_t offset, int32_t count)
    {
        NvModelTextureDesc* pCurrSrcDesc = pSrcDescs + offset;

        for (int32_t i = 0; i < count; ++i, ++pCurrSrcDesc)
        {
            TextureDesc d;
            d.m_textureIndex = pCurrSrcDesc->_textureIndex;
            d.m_UVIndex = pCurrSrcDesc->_UVIndex;
            d.m_mapModes[0] = static_cast<TextureDesc::MapMode>(pCurrSrcDesc->_mapModeS);
            d.m_mapModes[1] = static_cast<TextureDesc::MapMode>(pCurrSrcDesc->_mapModeT);
            d.m_mapModes[2] = static_cast<TextureDesc::MapMode>(pCurrSrcDesc->_mapModeU);
            d.m_minFilter = static_cast<TextureDesc::FilterMode>(pCurrSrcDesc->_minFilter);
            destArray.push_back(d);
        }
    }

    bool NvModelExtBin::LoadFromPreprocessed(uint8_t* data) {
        NvModelExtFileHeader* hdr = (NvModelExtFileHeader*)data;

        if (hdr->_magic[0] != 'N' || hdr->_magic[1] != 'V' ||
            hdr->_magic[2] != 'M' || hdr->_magic[3] != 'E')
        {
            return false;
        }
        switch (hdr->_version)
        {
        case 1:
            return LoadFromPreprocessed_v1(data);
        case 2:
            return LoadFromPreprocessed_v2(data);
        case 3:
            return LoadFromPreprocessed_v3(data);
        case 4:
        default:
            return LoadFromPreprocessed_v4(data);
        }
    }

    bool NvModelExtBin::LoadFromPreprocessed_v1(uint8_t* data) {
        NvModelExtFileHeader* hdr = (NvModelExtFileHeader*)data;

        for (int i = 0; i < 3; i++) {
			m_boundingBoxMin[i] = hdr->_boundingBoxMin[i];
			m_boundingBoxMax[i] = hdr->_boundingBoxMax[i];
			m_boundingBoxCenter[i] = hdr->_boundingBoxCenter[i];
		}

		m_meshCount = hdr->_subMeshCount;
		m_materialCount = hdr->_matCount;
            m_textureCount = 0;

		m_subMeshes = new SubMeshBin[m_meshCount];
		m_materials = new Material[m_materialCount];

        NvModelSubMeshHeader_v1* pModels = (NvModelSubMeshHeader_v1*)(data + hdr->_headerSize);

		// read mesh headers
		for (uint32_t i = 0; i < m_meshCount; i++) 
        {
			SubMesh* pMesh = m_subMeshes + i;

            NvModelSubMeshHeader_v1&  mhdr = pModels[i];
			pMesh->m_vertexCount = mhdr._vertexCount;
			pMesh->m_indexCount = mhdr._indexCount;
			pMesh->m_vertSize = mhdr._vertexSize;
			pMesh->m_normalOffset = mhdr._nOffset;
			pMesh->m_texCoordOffset = mhdr._tcOffset;
            pMesh->m_texCoordCount = (mhdr._tcOffset == -1)? 0 : 1;
			pMesh->m_tangentOffset = mhdr._sTanOffset;
            // _cOffset was not initialized properly in some older files, so
            // if the value isn't sane, ignore it
            if ((mhdr._cOffset > 0) && (mhdr._cOffset < (int32_t)mhdr._vertexSize))
            {
                pMesh->m_colorOffset = mhdr._cOffset;
                pMesh->m_colorCount = 1;
            }
            else
            {
                pMesh->m_colorOffset = -1;
                pMesh->m_colorCount = 0;
            }

            pMesh->m_boneIndexOffset = -1;
            pMesh->m_boneWeightOffset = -1;
            pMesh->m_bonesPerVertex = 0;

			pMesh->m_vertices = new float[pMesh->m_vertSize * pMesh->m_vertexCount];
			memcpy(pMesh->m_vertices, data + mhdr._vertArrayBase,
				pMesh->m_vertSize * pMesh->m_vertexCount * sizeof(float));
			pMesh->m_indices = new uint32_t[pMesh->m_indexCount];
			memcpy(pMesh->m_indices, data + mhdr._indexArrayBase,
				pMesh->m_indexCount * sizeof(uint32_t));
			pMesh->m_materialId = mhdr._matIndex;
		}

		NvModelMaterialHeader_v1* pSrcMat = (NvModelMaterialHeader_v1*)
            (data + hdr->_headerSize +
            sizeof(NvModelSubMeshHeader_v1) * m_meshCount);

		// read materials
		for (uint32_t i = 0; i < m_materialCount; i++) {
			Material* pDestMat = GetMaterial(i);

            memcpy(&pDestMat->m_ambient, &(pSrcMat->_ambient), 3 * sizeof(float));
            memcpy(&pDestMat->m_diffuse, &(pSrcMat->_diffuse), 3 * sizeof(float));
            memcpy(&pDestMat->m_specular, &(pSrcMat->_specular), 3 * sizeof(float));
            memcpy(&pDestMat->m_emissive, &(pSrcMat->_emissive), 3 * sizeof(float));
            pDestMat->m_alpha = pSrcMat->_alpha;
            pDestMat->m_shininess = pSrcMat->_shininess;
            pDestMat->m_opticalDensity = pSrcMat->_opticalDensity;
            memcpy(&pDestMat->m_transmissionFilter, &(pSrcMat->_transmissionFilter), 3 * sizeof(float));

            pDestMat->m_illumModel = (Material::IlluminationModel)(pSrcMat->_illumModel);

            TextureDesc d;
            memset(&d, 0, sizeof(TextureDesc));
                
            NvModelMaterialHeader_v1* pOldHdr = (NvModelMaterialHeader_v1*)(pSrcMat);

            if (pOldHdr->_ambientTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_ambientTexture;
                pDestMat->m_ambientTextures.push_back(d);
            }

            if (pOldHdr->_diffuseTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_diffuseTexture;
                pDestMat->m_diffuseTextures.push_back(d);
            }

            if (pOldHdr->_specularTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_specularTexture;
                pDestMat->m_specularTextures.push_back(d);
            }

            if (pOldHdr->_bumpMapTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_bumpMapTexture;
                pDestMat->m_bumpMapTextures.push_back(d);
            }

            if (pOldHdr->_reflectionTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_reflectionTexture;
                pDestMat->m_reflectionTextures.push_back(d);
            }

            if (pOldHdr->_displacementMapTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_displacementMapTexture;
                pDestMat->m_displacementMapTextures.push_back(d);
            }

            if (pOldHdr->_specularPowerTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_specularPowerTexture;
                pDestMat->m_specularPowerTextures.push_back(d);
            }

            if (pOldHdr->_alphaMapTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_alphaMapTexture;
                pDestMat->m_alphaMapTextures.push_back(d);
            }

            if (pOldHdr->_decalTexture != -1)
            {
                d.m_textureIndex = pOldHdr->_decalTexture;
                pDestMat->m_decalTextures.push_back(d);
            }

            // Update pSrcMat pointer to point to the next material header
            pSrcMat = (NvModelMaterialHeader_v1*)(((char*)pSrcMat) + sizeof(NvModelMaterialHeader_v1));
        }

		return true;
	}

    bool NvModelExtBin::LoadFromPreprocessed_v2(uint8_t* data) {
        NvModelExtFileHeader_v2* hdr = (NvModelExtFileHeader_v2*)data;

        for (int i = 0; i < 3; i++) {
            m_boundingBoxMin[i] = hdr->_boundingBoxMin[i];
            m_boundingBoxMax[i] = hdr->_boundingBoxMax[i];
            m_boundingBoxCenter[i] = hdr->_boundingBoxCenter[i];
        }

        m_meshCount = hdr->_subMeshCount;
        m_materialCount = hdr->_matCount;
        m_textureCount = hdr->_textureCount;

        m_subMeshes = new SubMeshBin[m_meshCount];
        m_materials = new Material[m_materialCount];

        NvModelSubMeshHeader_v1* pModels = (NvModelSubMeshHeader_v1*)
            (data + hdr->_headerSize);

        // read mesh headers
        for (uint32_t i = 0; i < m_meshCount; i++)
        {
            SubMesh* pMesh = m_subMeshes + i;

            NvModelSubMeshHeader_v1&  mhdr = pModels[i];
            pMesh->m_vertexCount = mhdr._vertexCount;
            pMesh->m_indexCount = mhdr._indexCount;
            pMesh->m_vertSize = mhdr._vertexSize;
            pMesh->m_normalOffset = mhdr._nOffset;
            pMesh->m_texCoordOffset = mhdr._tcOffset;
            pMesh->m_texCoordCount = mhdr._tcSize;
            pMesh->m_tangentOffset = mhdr._sTanOffset;
            pMesh->m_colorOffset = mhdr._cOffset;
            pMesh->m_colorCount = (mhdr._tcOffset == -1) ? 0 : 1;;
            pMesh->m_boneIndexOffset = -1;
            pMesh->m_boneWeightOffset = -1;
            pMesh->m_bonesPerVertex = 0;

            pMesh->m_vertices = new float[pMesh->m_vertSize * pMesh->m_vertexCount];
            memcpy(pMesh->m_vertices, data + mhdr._vertArrayBase,
                pMesh->m_vertSize * pMesh->m_vertexCount * sizeof(float));
            pMesh->m_indices = new uint32_t[pMesh->m_indexCount];
            memcpy(pMesh->m_indices, data + mhdr._indexArrayBase,
                pMesh->m_indexCount * sizeof(uint32_t));
            pMesh->m_materialId = mhdr._matIndex;
        }

        NvModelMaterialHeader* pSrcMat = (NvModelMaterialHeader*)
            (data + hdr->_headerSize +
            sizeof(NvModelSubMeshHeader_v1) * m_meshCount);

        // read materials
        for (uint32_t i = 0; i < m_materialCount; i++) {
            Material* pDestMat = GetMaterial(i);

            memcpy(&pDestMat->m_ambient, &(pSrcMat->_ambient), 3 * sizeof(float));
            memcpy(&pDestMat->m_diffuse, &(pSrcMat->_diffuse), 3 * sizeof(float));
            memcpy(&pDestMat->m_specular, &(pSrcMat->_specular), 3 * sizeof(float));
            memcpy(&pDestMat->m_emissive, &(pSrcMat->_emissive), 3 * sizeof(float));
            pDestMat->m_alpha = pSrcMat->_alpha;
            pDestMat->m_shininess = pSrcMat->_shininess;
            pDestMat->m_opticalDensity = pSrcMat->_opticalDensity;
            memcpy(&pDestMat->m_transmissionFilter, &(pSrcMat->_transmissionFilter), 3 * sizeof(float));

            pDestMat->m_illumModel = (Material::IlluminationModel)(pSrcMat->_illumModel);

            // Read texture desc array by back-computing the beginning of our texture desc array from the end of our material header
            NvModelTextureDesc* pTextureDesc = (NvModelTextureDesc*)(((char*)pSrcMat) + pSrcMat->_materialBlockSize);
            uint32_t numTextures = pSrcMat->_ambientTextureCount +
                pSrcMat->_diffuseTextureCount +
                pSrcMat->_specularTextureCount +
                pSrcMat->_bumpMapTextureCount +
                pSrcMat->_reflectionTextureCount +
                pSrcMat->_displacementMapTextureCount +
                pSrcMat->_specularPowerTextureCount +
                pSrcMat->_alphaMapTextureCount +
                pSrcMat->_decalTextureCount;

            pTextureDesc -= numTextures;

            // Read in the Texture Descriptors for each of the possible types of textures
            ReadTextureDescs(pTextureDesc, pDestMat->m_ambientTextures, pSrcMat->_ambientTextureOffset, pSrcMat->_ambientTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_diffuseTextures, pSrcMat->_diffuseTextureOffset, pSrcMat->_diffuseTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_specularTextures, pSrcMat->_specularTextureOffset, pSrcMat->_specularTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_bumpMapTextures, pSrcMat->_bumpMapTextureOffset, pSrcMat->_bumpMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_reflectionTextures, pSrcMat->_reflectionTextureOffset, pSrcMat->_reflectionTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_displacementMapTextures, pSrcMat->_displacementMapTextureOffset, pSrcMat->_displacementMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_specularPowerTextures, pSrcMat->_specularPowerTextureOffset, pSrcMat->_specularPowerTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_alphaMapTextures, pSrcMat->_alphaMapTextureOffset, pSrcMat->_alphaMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_decalTextures, pSrcMat->_decalTextureOffset, pSrcMat->_decalTextureCount);

            // Update pSrcMat pointer to point to the next bit of memory after the end of the texture desc array
            pSrcMat = (NvModelMaterialHeader*)(((char*)pSrcMat) + pSrcMat->_materialBlockSize);
        }

        // Read Texture names
        m_textures.resize(m_textureCount);
        if ((hdr->_version > 1) && m_textureCount > 0)
        {
            // pSrcMat currently points just past the end of the last material header, 
            // so that's where our string table offsets array starts
            uint32_t* pOffsets = (uint32_t*)pSrcMat;
            char* pString = (char*)(pOffsets + m_textureCount);
            for (uint32_t textureIndex = 0; textureIndex < m_textureCount; ++textureIndex)
            {
                m_textures[textureIndex] = pString;
                pString += m_textures[textureIndex].length() + 1;
            }
        }

        return true;
    }

    bool NvModelExtBin::LoadFromPreprocessed_v3(uint8_t* data) {
        NvModelExtFileHeader* hdr = (NvModelExtFileHeader*)data;

        for (int i = 0; i < 3; i++) {
            m_boundingBoxMin[i] = hdr->_boundingBoxMin[i];
            m_boundingBoxMax[i] = hdr->_boundingBoxMax[i];
            m_boundingBoxCenter[i] = hdr->_boundingBoxCenter[i];
        }

        m_meshCount = hdr->_subMeshCount;
        m_materialCount = hdr->_matCount;

        m_subMeshes = new SubMeshBin[m_meshCount];
        m_materials = new Material[m_materialCount];

        uint8_t* pCurrentBufferPointer = ReadTextureBlock(data + hdr->_headerSize);
        pCurrentBufferPointer = ReadSkeletonBlock(pCurrentBufferPointer);
        pCurrentBufferPointer = ReadMaterials(pCurrentBufferPointer);
        pCurrentBufferPointer = ReadMeshes_v3(pCurrentBufferPointer);

        return true;
    }

    bool NvModelExtBin::LoadFromPreprocessed_v4(uint8_t* data) {
        NvModelExtFileHeader* hdr = (NvModelExtFileHeader*)data;

        for (int i = 0; i < 3; i++) {
            m_boundingBoxMin[i] = hdr->_boundingBoxMin[i];
            m_boundingBoxMax[i] = hdr->_boundingBoxMax[i];
            m_boundingBoxCenter[i] = hdr->_boundingBoxCenter[i];
        }

        m_meshCount = hdr->_subMeshCount;
        m_materialCount = hdr->_matCount;

        m_subMeshes = new SubMeshBin[m_meshCount];
        m_materials = new Material[m_materialCount];

        uint8_t* pCurrentBufferPointer = ReadTextureBlock(data + hdr->_headerSize);
        pCurrentBufferPointer = ReadSkeletonBlock(pCurrentBufferPointer);
        pCurrentBufferPointer = ReadMaterials(pCurrentBufferPointer);
        pCurrentBufferPointer = ReadMeshes(pCurrentBufferPointer);

        return true;
    }

    uint8_t* NvModelExtBin::ReadTextureBlock(uint8_t* pTextureBlock)
    {
        NvModelTextureBlockHeader* pTBH = reinterpret_cast<NvModelTextureBlockHeader*>(pTextureBlock);
        m_textureCount = pTBH->_textureCount;

        // Read Texture names
        m_textures.resize(m_textureCount);
        if (m_textureCount > 0)
        {
            // pSrcMat currently points just past the end of the last material header, 
            // so that's where our string table offsets array starts
            uint32_t* pOffsets = reinterpret_cast<uint32_t*>(pTextureBlock + sizeof(NvModelTextureBlockHeader));
            char* pStringTable = reinterpret_cast<char*>(pOffsets + m_textureCount);
            for (uint32_t textureIndex = 0; textureIndex < m_textureCount; ++textureIndex)
            {
                char* pString = pStringTable + pOffsets[textureIndex];
                m_textures[textureIndex] = pString;
            }
        }

        return pTextureBlock + pTBH->_textureBlockSize;
    }

    uint8_t* NvModelExtBin::ReadSkeletonBlock(uint8_t* pSkeletonBlock)
    {
        NvModelSkeletonDataBlockHeader* pSDBH = reinterpret_cast<NvModelSkeletonDataBlockHeader*>(pSkeletonBlock);
        uint32_t boneCount = pSDBH->_boneCount;
        if (boneCount > 0)
        {
            NvSkeletonNode* pDestNodes = new NvSkeletonNode[boneCount];
            NvSkeletonNode* pDestNode = pDestNodes;
            if (NULL != pDestNodes)
            {
                uint8_t* pBoneData = pSkeletonBlock + sizeof(NvModelSkeletonDataBlockHeader);
                for (uint32_t boneIndex = 0; boneIndex < boneCount; ++boneIndex, ++pDestNode)
                {
                    // pBoneData points to the bone data structure, so retain that pointer before
                    // moving it past the structure to the next element
                    NvModelBoneData* pSrcNode = reinterpret_cast<NvModelBoneData*>(pBoneData);
                    pDestNode->m_parentNode = pSrcNode->_parentIndex;
                    pDestNode->m_parentRelTransform.set_value(pSrcNode->_parentRelTransform);
                    pBoneData += sizeof(NvModelBoneData);

                    // Grab the name of the bone from the current data pointer then move it along
                    pDestNode->m_name = reinterpret_cast<char*>(pBoneData);
                    pBoneData += pSrcNode->_nameLength;
                    
                    // Get the child indices from the current bone data pointer before moving it past
                    for (int32_t childIndex = 0; childIndex < pSrcNode->_numChildren; ++childIndex)
                    {
                        pDestNode->m_childNodes.push_back(*(reinterpret_cast<int32_t*>(pBoneData)));
                        pBoneData += sizeof(int32_t);
                    }

                    // Get the mesh indices from the current bone data pointer before moving it past
                    for (int32_t meshIndex = 0; meshIndex < pSrcNode->_numMeshes; ++meshIndex)
                    {
                        pDestNode->m_meshes.push_back(*(reinterpret_cast<uint32_t*>(pBoneData)));
                        pBoneData += sizeof(uint32_t);
                    }
                }
                m_pSkeleton = new NvSkeleton(pDestNodes, boneCount);
            }
        }

        return pSkeletonBlock + pSDBH->_skeletonBlockSize;
    }

    uint8_t* NvModelExtBin::ReadMaterials(uint8_t* pMaterials)
    {
        NvModelMaterialHeader* pSrcMat = reinterpret_cast<NvModelMaterialHeader*>(pMaterials);
        for (uint32_t matIndex = 0; matIndex < m_materialCount; ++matIndex)
        {
            Material* pDestMat = GetMaterial(matIndex);
            memcpy(&pDestMat->m_ambient, &(pSrcMat->_ambient), 3 * sizeof(float));
            memcpy(&pDestMat->m_diffuse, &(pSrcMat->_diffuse), 3 * sizeof(float));
            memcpy(&pDestMat->m_specular, &(pSrcMat->_specular), 3 * sizeof(float));
            memcpy(&pDestMat->m_emissive, &(pSrcMat->_emissive), 3 * sizeof(float));
            pDestMat->m_alpha = pSrcMat->_alpha;
            pDestMat->m_shininess = pSrcMat->_shininess;
            pDestMat->m_opticalDensity = pSrcMat->_opticalDensity;
            memcpy(&pDestMat->m_transmissionFilter, &(pSrcMat->_transmissionFilter), 3 * sizeof(float));

            pDestMat->m_illumModel = (Material::IlluminationModel)(pSrcMat->_illumModel);

            NvModelTextureDesc* pTextureDesc = reinterpret_cast<NvModelTextureDesc*>(reinterpret_cast<uint8_t*>(pSrcMat) + sizeof(NvModelMaterialHeader));

            // Read in the Texture Descriptors for each of the possible types of textures
            ReadTextureDescs(pTextureDesc, pDestMat->m_ambientTextures, pSrcMat->_ambientTextureOffset, pSrcMat->_ambientTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_diffuseTextures, pSrcMat->_diffuseTextureOffset, pSrcMat->_diffuseTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_specularTextures, pSrcMat->_specularTextureOffset, pSrcMat->_specularTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_bumpMapTextures, pSrcMat->_bumpMapTextureOffset, pSrcMat->_bumpMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_reflectionTextures, pSrcMat->_reflectionTextureOffset, pSrcMat->_reflectionTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_displacementMapTextures, pSrcMat->_displacementMapTextureOffset, pSrcMat->_displacementMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_specularPowerTextures, pSrcMat->_specularPowerTextureOffset, pSrcMat->_specularPowerTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_alphaMapTextures, pSrcMat->_alphaMapTextureOffset, pSrcMat->_alphaMapTextureCount);
            ReadTextureDescs(pTextureDesc, pDestMat->m_decalTextures, pSrcMat->_decalTextureOffset, pSrcMat->_decalTextureCount);

            // Update pSrcMat pointer to point to the next bit of memory after the end of the texture desc array
            pSrcMat = reinterpret_cast<NvModelMaterialHeader*>(reinterpret_cast<uint8_t*>(pSrcMat) + pSrcMat->_materialBlockSize);
        }
        return reinterpret_cast<uint8_t*>(pSrcMat);
    }

    uint8_t* NvModelExtBin::ReadMeshes_v3(uint8_t* pMeshes)
    {

        // read mesh headers
        for (uint32_t i = 0; i < m_meshCount; i++)
        {
            NvModelSubMeshHeader_v3* pSrcMesh = reinterpret_cast<NvModelSubMeshHeader_v3*>(pMeshes);
            SubMesh* pDestMesh = m_subMeshes + i;

            pDestMesh->m_vertexCount = pSrcMesh->_vertexCount;
            pDestMesh->m_indexCount = pSrcMesh->_indexCount;
            pDestMesh->m_vertSize = pSrcMesh->_vertexSize;
            pDestMesh->m_normalOffset = pSrcMesh->_nOffset;
            pDestMesh->m_texCoordOffset = pSrcMesh->_tcOffset;
            pDestMesh->m_texCoordCount = pSrcMesh->_tcSize;
            pDestMesh->m_tangentOffset = pSrcMesh->_sTanOffset;
            pDestMesh->m_colorOffset = pSrcMesh->_cOffset;
            pDestMesh->m_colorCount = pSrcMesh->_colorCount;
            pDestMesh->m_boneIndexOffset = pSrcMesh->_boneIndexOffset;
            pDestMesh->m_boneWeightOffset = pSrcMesh->_boneWeightOffset;
            pDestMesh->m_bonesPerVertex = pSrcMesh->_bonesPerVertex;
            pDestMesh->m_materialId = pSrcMesh->_matIndex;
            
            // Read in the bone map
            int32_t boneCount = pSrcMesh->_boneMapCount;
            if (boneCount > 0)
            {
                uint8_t* pBoneMap = pMeshes + sizeof(NvModelSubMeshHeader_v3);
                uint32_t boneMapSize = sizeof(int32_t) * boneCount;
                pDestMesh->m_boneMap.resize(boneCount);
                memcpy(&(pDestMesh->m_boneMap[0]), pBoneMap, boneMapSize);
                
                uint8_t* pBoneTransforms = pBoneMap + boneMapSize;
                uint32_t boneTransformsSize = sizeof(nv::matrix4f) * boneCount;
                pDestMesh->m_meshToBoneTransforms.resize(boneCount);
                memcpy(&(pDestMesh->m_meshToBoneTransforms[0]), pBoneTransforms, boneTransformsSize);
            }

            uint32_t vertBufferSize = pDestMesh->m_vertSize * pDestMesh->m_vertexCount * sizeof(float);
            pDestMesh->m_vertices = new float[pDestMesh->m_vertSize * pDestMesh->m_vertexCount];
            memcpy(pDestMesh->m_vertices, pMeshes + pSrcMesh->_vertArrayBase, vertBufferSize);
            
            uint32_t indexBufferSize = pDestMesh->m_indexCount * sizeof(uint32_t);
            pDestMesh->m_indices = new uint32_t[pDestMesh->m_indexCount];
            memcpy(pDestMesh->m_indices, pMeshes + pSrcMesh->_indexArrayBase, indexBufferSize);

            pMeshes += pSrcMesh->_indexArrayBase + indexBufferSize;
        }
        return pMeshes;
    }

    uint8_t* NvModelExtBin::ReadMeshes(uint8_t* pMeshes)
    {

        // read mesh headers
        for (uint32_t i = 0; i < m_meshCount; i++)
        {
            NvModelSubMeshHeader* pSrcMesh = reinterpret_cast<NvModelSubMeshHeader*>(pMeshes);
            SubMesh* pDestMesh = m_subMeshes + i;

            pDestMesh->m_vertexCount = pSrcMesh->_vertexCount;
            pDestMesh->m_indexCount = pSrcMesh->_indexCount;
            pDestMesh->m_vertSize = pSrcMesh->_vertexSize;
            pDestMesh->m_normalOffset = pSrcMesh->_nOffset;
            pDestMesh->m_texCoordOffset = pSrcMesh->_tcOffset;
            pDestMesh->m_texCoordCount = pSrcMesh->_tcSize;
            pDestMesh->m_tangentOffset = pSrcMesh->_sTanOffset;
            pDestMesh->m_colorOffset = pSrcMesh->_cOffset;
            pDestMesh->m_colorCount = pSrcMesh->_colorCount;
            pDestMesh->m_boneIndexOffset = pSrcMesh->_boneIndexOffset;
            pDestMesh->m_boneWeightOffset = pSrcMesh->_boneWeightOffset;
            pDestMesh->m_bonesPerVertex = pSrcMesh->_bonesPerVertex;
            pDestMesh->m_materialId = pSrcMesh->_matIndex;
            pDestMesh->m_parentBone = pSrcMesh->_parentBone;

            // Read in the bone map
            int32_t boneCount = pSrcMesh->_boneMapCount;
            if (boneCount > 0)
            {
                uint8_t* pBoneMap = pMeshes + sizeof(NvModelSubMeshHeader);
                uint32_t boneMapSize = sizeof(int32_t) * boneCount;
                pDestMesh->m_boneMap.resize(boneCount);
                memcpy(&(pDestMesh->m_boneMap[0]), pBoneMap, boneMapSize);

                uint8_t* pBoneTransforms = pBoneMap + boneMapSize;
                uint32_t boneTransformsSize = sizeof(nv::matrix4f) * boneCount;
                pDestMesh->m_meshToBoneTransforms.resize(boneCount);
                memcpy(&(pDestMesh->m_meshToBoneTransforms[0]), pBoneTransforms, boneTransformsSize);
            }

            uint32_t vertBufferSize = pDestMesh->m_vertSize * pDestMesh->m_vertexCount * sizeof(float);
            pDestMesh->m_vertices = new float[pDestMesh->m_vertSize * pDestMesh->m_vertexCount];
            memcpy(pDestMesh->m_vertices, pMeshes + pSrcMesh->_vertArrayBase, vertBufferSize);

            uint32_t indexBufferSize = pDestMesh->m_indexCount * sizeof(uint32_t);
            pDestMesh->m_indices = new uint32_t[pDestMesh->m_indexCount];
            memcpy(pDestMesh->m_indices, pMeshes + pSrcMesh->_indexArrayBase, indexBufferSize);

            pMeshes += pSrcMesh->_indexArrayBase + indexBufferSize;
        }
        return pMeshes;
    }



}
