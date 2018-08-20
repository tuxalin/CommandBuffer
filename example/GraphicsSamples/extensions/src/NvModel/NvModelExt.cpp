//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExt.cpp
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
#include "NvModelExtObj.h"
#include <NV/NvTokenizer.h>
#include "NvModelMeshFace.h"
#include "NvModel/NvModelSubMesh.h"
#include "NvModel/NvSkeleton.h"
#include "NvModelExtFile.h"

#ifdef LINUX
#include <stdio.h>
#endif

namespace Nv
{
	NvModelFileLoader* NvModelExt::ms_pLoader = NULL;

	NvModelExt* NvModelExt::CreateFromObj(const char* filename, float scale,
		bool generateNormals, bool generateTangents,
		float vertMergeThreshold, float normMergeThreshold, uint32_t initialVertCount) {
		return NvModelExtObj::Create(filename, scale, generateNormals, generateTangents,
			vertMergeThreshold, normMergeThreshold, initialVertCount);
	}

	NvModelExt* NvModelExt::CreateFromPreprocessed(const char* filename) {
		return NvModelExtBin::Create(filename);
	}

	NvModelExt::NvModelExt() :
		m_boundingBoxMin(0.0f, 0.0f, 0.0f),
		m_boundingBoxMax(0.0f, 0.0f, 0.0f),
        m_boundingBoxCenter(0.0f, 0.0f, 0.0f),
        m_pSkeleton(NULL)
	{
	}

	NvModelExt::~NvModelExt()
	{
	}

    int32_t AppendTextureDescs(std::vector<NvModelTextureDesc>& destDescs, const TextureDescArray& srcDescs, int32_t currentOffset, int32_t& outOffset)
    {
        if (srcDescs.empty())
        {
            outOffset = -1;
            return 0;
        }
        outOffset = currentOffset;

        // Add all of the texture descriptors to our descriptor array
        Nv::TextureDescArray::const_iterator texDescIt;
        Nv::TextureDescArray::const_iterator texDescEnd = srcDescs.end();
        for (texDescIt = srcDescs.begin(); texDescIt != texDescEnd; ++texDescIt)
        {
            const TextureDesc& srcDesc = *texDescIt;
            NvModelTextureDesc destDesc;
            destDesc._textureIndex = srcDesc.m_textureIndex;
            destDesc._UVIndex = srcDesc.m_UVIndex;

            // Map modes and filter modes are equivalent between the two structures, so they can just be copied
            destDesc._mapModeS = static_cast<NvModelTextureDesc::MapMode>(srcDesc.m_mapModes[0]);
            destDesc._mapModeT = static_cast<NvModelTextureDesc::MapMode>(srcDesc.m_mapModes[1]);
            destDesc._mapModeU = static_cast<NvModelTextureDesc::MapMode>(srcDesc.m_mapModes[2]);
            destDesc._minFilter = static_cast<NvModelTextureDesc::FilterMode>(srcDesc.m_minFilter);
            destDescs.push_back(destDesc);
        }
        return srcDescs.size();
    }

	bool NvModelExt::WritePreprocessedModel(const char* filename) {
#ifdef _WIN32
		FILE* fp = NULL;
	
		errno_t err = fopen_s(&fp, filename, "wb");
		if (err || !fp)
		{
			LOGE("Unable to write file: %s\n", filename);
			return false;
		}

        // File structure:
        // NvModelExtFileHeader
        // NvModelTextureBlock
        // SkeletonDataBlock
        // M x NvMaterialBlock
        // N x NvModelSubMeshBlock
        int32_t totalBytesWritten = 0;
        totalBytesWritten += WriteFileHeader(fp);
        totalBytesWritten += WriteTextureBlock(fp);
        totalBytesWritten += WriteSkeletonBlock(fp);
        totalBytesWritten += WriteMaterials(fp);
        WriteMeshes(fp);
		
        fclose(fp);

		return true;
#else
		return false;
#endif
	}

    int32_t NvModelExt::WriteFileHeader(FILE* fp) const {
        // Build the file header
        NvModelExtFileHeader fileHdr;
        fileHdr._magic[0] = 'N';
        fileHdr._magic[1] = 'V';
        fileHdr._magic[2] = 'M';
        fileHdr._magic[3] = 'E';
        fileHdr._headerSize = sizeof(NvModelExtFileHeader);
        fileHdr._version = NVMODEL_FILE_VERSION;
        fileHdr._subMeshCount = GetMeshCount();
        fileHdr._matCount = GetMaterialCount();
        for (int i = 0; i < 3; i++) {
            fileHdr._boundingBoxMin[i] = m_boundingBoxMin[i];
            fileHdr._boundingBoxMax[i] = m_boundingBoxMax[i];
            fileHdr._boundingBoxCenter[i] = m_boundingBoxCenter[i];
        }

        // Write the file header
        return fwrite(&fileHdr, sizeof(NvModelExtFileHeader), 1, fp) * sizeof(NvModelExtFileHeader);
    }

    int32_t NvModelExt::WriteTextureBlock(FILE* fp) const {
        int32_t totalBytesWritten = 0;

        // Create the offset array for the string table of texture names and determine the size of the table
        uint32_t textureCount = GetTextureCount();
        std::vector<uint32_t> textureNameOffsets;
        textureNameOffsets.resize(textureCount);
        uint32_t textureStringTableSize = 0;
        for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
        {
            textureNameOffsets[textureIndex] = textureStringTableSize;
            // Must pad strings out to 4 byte multiples such that subsequent
            // elements are 4-byte aligned
            textureStringTableSize += GetPaddedStringLength(GetTextureName(textureIndex));
        }
        NV_ASSERT(textureStringTableSize % 4 == 0);

        NvModelTextureBlockHeader tbh;
        tbh._textureCount = textureCount;
        tbh._textureBlockSize = sizeof(NvModelTextureBlockHeader) + // Block Header
                                (sizeof(uint32_t) * textureCount) + // String Offset Table
                                textureStringTableSize;             // String Data

        // Write the Texture block header
        totalBytesWritten += fwrite(&tbh, sizeof(NvModelTextureBlockHeader), 1, fp) * sizeof(NvModelTextureBlockHeader);
        if (0 == totalBytesWritten)
        {
            return 0;
        }

        if (textureCount > 0)
        {
            // Write the texture name string table offset array
            totalBytesWritten += fwrite(&(textureNameOffsets[0]), sizeof(uint32_t), textureCount, fp) * sizeof(uint32_t);

            // Write the texture names including the null terminating character and
            // any padding out to the next 4 chars for each string
            for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
            {
                std::string textureName = GetTextureName(textureIndex);
                totalBytesWritten += WritePaddedString(fp, textureName);
            }
        }
        NV_ASSERT(totalBytesWritten == tbh._textureBlockSize);
        return totalBytesWritten;
    }

    int32_t NvModelExt::WriteSkeletonBlock(FILE* fp) const {
        NvModelSkeletonDataBlockHeader hdr;
        if ((NULL == m_pSkeleton) || (m_pSkeleton->GetNumNodes() == 0))
        {
            hdr._boneCount = 0;
            hdr._skeletonBlockSize = sizeof(NvModelSkeletonDataBlockHeader);
            return fwrite(&hdr, hdr._skeletonBlockSize, 1, fp) * hdr._skeletonBlockSize;
        }

        hdr._boneCount = m_pSkeleton->GetNumNodes();
        // Determine the size of the data block
        int32_t blockSize = sizeof(NvModelSkeletonDataBlockHeader);
        for (uint32_t boneIndex = 0; boneIndex < hdr._boneCount; ++boneIndex)
        {
            const NvSkeletonNode* pNode = m_pSkeleton->GetNodeByIndex(boneIndex);
            blockSize += sizeof(NvModelBoneData);       // Header
            blockSize += GetPaddedStringLength(pNode->m_name);
            blockSize += pNode->m_childNodes.size() * sizeof(int32_t);
            blockSize += pNode->m_meshes.size() * sizeof(uint32_t);
        }
        hdr._skeletonBlockSize = blockSize;

        int32_t bytesWritten = 0;
        bytesWritten += fwrite(&hdr, sizeof(NvModelSkeletonDataBlockHeader), 1, fp) * sizeof(NvModelSkeletonDataBlockHeader);
        for (uint32_t boneIndex = 0; boneIndex < hdr._boneCount; ++boneIndex)
        {
            const NvSkeletonNode* pNode = m_pSkeleton->GetNodeByIndex(boneIndex);
            NvModelBoneData mbd;
            mbd._parentIndex = pNode->m_parentNode;
            memcpy(mbd._parentRelTransform, pNode->m_parentRelTransform._array, 16 * sizeof(float));
            mbd._nameLength = GetPaddedStringLength(pNode->m_name);
            mbd._numChildren = pNode->m_childNodes.size();
            mbd._numMeshes = pNode->m_meshes.size();
            bytesWritten += fwrite(&mbd, sizeof(NvModelBoneData), 1, fp) * sizeof(NvModelBoneData);
            bytesWritten += WritePaddedString(fp, pNode->m_name);
            for (std::vector<int32_t>::const_iterator childIt = pNode->m_childNodes.begin(); childIt != pNode->m_childNodes.end(); ++childIt)
            {
                bytesWritten += fwrite(&(*childIt), sizeof(int32_t), 1, fp) * sizeof(int32_t);
            }
            for (std::vector<uint32_t>::const_iterator meshIt = pNode->m_meshes.begin(); meshIt != pNode->m_meshes.end(); ++meshIt)
            {
                bytesWritten += fwrite(&(*meshIt), sizeof(uint32_t), 1, fp) * sizeof(uint32_t);
            }
        }
        NV_ASSERT(bytesWritten == blockSize);
        return bytesWritten;
    }

    int32_t NvModelExt::WriteMaterials(FILE* fp) {
        int32_t bytesWritten = 0;
        uint32_t matCount = GetMaterialCount();
        for (uint32_t matIndex = 0; matIndex < matCount; ++matIndex)
        {
            const Material* pMat = GetMaterial(matIndex);
            NvModelMaterialHeader mhdr;
            // Build a vector of texture descriptions so that we can compute the size of the material
            // header and we can then sequentially write out each texture descriptor to the file
            std::vector<NvModelTextureDesc> texDescs;

            memcpy(&mhdr._ambient, &pMat->m_ambient, 3 * sizeof(float));
            memcpy(&mhdr._diffuse, &pMat->m_diffuse, 3 * sizeof(float));
            memcpy(&mhdr._emissive, &pMat->m_emissive, 3 * sizeof(float));
            mhdr._alpha = pMat->m_alpha;
            memcpy(&mhdr._specular, &pMat->m_specular, 3 * sizeof(float));
            mhdr._shininess = pMat->m_shininess;
            mhdr._opticalDensity = pMat->m_opticalDensity;
            memcpy(&mhdr._transmissionFilter, &pMat->m_transmissionFilter, 3 * sizeof(float));

            // Gather all of our textures up, keeping a running offset and storing the proper offset/sizes in the header
            int32_t currentOffset = 0;
            mhdr._ambientTextureCount = AppendTextureDescs(texDescs, pMat->m_ambientTextures, currentOffset, mhdr._ambientTextureOffset);
            currentOffset += mhdr._ambientTextureCount;

            mhdr._diffuseTextureCount = AppendTextureDescs(texDescs, pMat->m_diffuseTextures, currentOffset, mhdr._diffuseTextureOffset);
            currentOffset += mhdr._diffuseTextureCount;

            mhdr._specularTextureCount = AppendTextureDescs(texDescs, pMat->m_specularTextures, currentOffset, mhdr._specularTextureOffset);
            currentOffset += mhdr._specularTextureCount;

            mhdr._bumpMapTextureCount = AppendTextureDescs(texDescs, pMat->m_bumpMapTextures, currentOffset, mhdr._bumpMapTextureOffset);
            currentOffset += mhdr._bumpMapTextureCount;

            mhdr._reflectionTextureCount = AppendTextureDescs(texDescs, pMat->m_reflectionTextures, currentOffset, mhdr._reflectionTextureOffset);
            currentOffset += mhdr._reflectionTextureCount;

            mhdr._displacementMapTextureCount = AppendTextureDescs(texDescs, pMat->m_displacementMapTextures, currentOffset, mhdr._displacementMapTextureOffset);
            currentOffset += mhdr._displacementMapTextureCount;

            mhdr._specularPowerTextureCount = AppendTextureDescs(texDescs, pMat->m_specularPowerTextures, currentOffset, mhdr._specularPowerTextureOffset);
            currentOffset += mhdr._specularPowerTextureCount;

            mhdr._alphaMapTextureCount = AppendTextureDescs(texDescs, pMat->m_alphaMapTextures, currentOffset, mhdr._alphaMapTextureOffset);
            currentOffset += mhdr._alphaMapTextureCount;

            mhdr._decalTextureCount = AppendTextureDescs(texDescs, pMat->m_decalTextures, currentOffset, mhdr._decalTextureOffset);
            currentOffset += mhdr._decalTextureCount;

            mhdr._illumModel = pMat->m_illumModel;

            // Finally calculate the header size including our texture descriptors
            mhdr._materialBlockSize = sizeof(NvModelMaterialHeader) + (texDescs.size() * sizeof(NvModelTextureDesc));

            // Write out the header
            bytesWritten += fwrite(&mhdr, sizeof(NvModelMaterialHeader), 1, fp) * sizeof(NvModelMaterialHeader);

            // Write out the array of texture descriptors that accompany the material
            std::vector<NvModelTextureDesc>::const_iterator texIt;
            std::vector<NvModelTextureDesc>::const_iterator texEnd = texDescs.end();
            for (texIt = texDescs.begin(); texIt != texEnd; ++texIt)
            {
                const NvModelTextureDesc& desc = *texIt;
                bytesWritten += fwrite(&desc, sizeof(NvModelTextureDesc), 1, fp) * sizeof(NvModelTextureDesc);
            }
        }
        return bytesWritten;
    }

    int32_t NvModelExt::WriteMeshes(FILE* fp) {
        int32_t totalBytesWritten = 0;

        uint32_t meshCount = GetMeshCount();

        // write mesh data blocks
        for (uint32_t i = 0; i < meshCount; i++) {
            SubMesh* pMesh = GetSubMesh(i);
            
            NvModelSubMeshHeader mhdr;
            mhdr._vertexCount = pMesh->getVertexCount();
            mhdr._indexCount = pMesh->getIndexCount();
            mhdr._vertexSize = pMesh->getVertexSize();
            mhdr._indexSize = sizeof(uint32_t);
            mhdr._pOffset = 0;
            mhdr._nOffset = pMesh->getNormalOffset();
            mhdr._tcOffset = pMesh->getTexCoordOffset();
            mhdr._tcSize = pMesh->getTexCoordCount();
            mhdr._sTanOffset = pMesh->getTangentOffset();
            mhdr._cOffset = pMesh->getColorOffset();
            mhdr._boneIndexOffset = pMesh->getBoneIndexOffset();
            mhdr._boneWeightOffset = pMesh->getBoneWeightOffset();
            mhdr._colorCount = pMesh->getColorCount();
            mhdr._bonesPerVertex = pMesh->getBonesPerVertex();
            NV_ASSERT(pMesh->m_boneMap.size() == pMesh->m_meshToBoneTransforms.size());
            mhdr._boneMapCount = pMesh->m_boneMap.size();
            mhdr._vertArrayBase = sizeof(NvModelSubMeshHeader) + ((sizeof(int32_t) + sizeof(nv::matrix4f)) * mhdr._boneMapCount);
            mhdr._indexArrayBase = mhdr._vertArrayBase + (pMesh->getVertexCount() * pMesh->getVertexSize() * sizeof(float));
            mhdr._matIndex = pMesh->m_materialId;
            mhdr._parentBone = pMesh->m_parentBone;

            // Write out the header
            totalBytesWritten += fwrite(&mhdr, sizeof(NvModelSubMeshHeader), 1, fp) * sizeof(NvModelSubMeshHeader);

            // Write out the bone map
            if (mhdr._bonesPerVertex > 0)
            {
                totalBytesWritten += fwrite(&(pMesh->m_boneMap[0]), sizeof(int32_t), mhdr._boneMapCount, fp) * sizeof(int32_t);
                totalBytesWritten += fwrite(&(pMesh->m_meshToBoneTransforms[0]), sizeof(nv::matrix4f), mhdr._boneMapCount, fp) * sizeof(nv::matrix4f);
            }

            // write vertex data
            totalBytesWritten += fwrite(pMesh->getVertices(), sizeof(float), pMesh->getVertexCount() * pMesh->getVertexSize(), fp) * sizeof(float);

            // write index data
            totalBytesWritten += fwrite(pMesh->getIndices(), sizeof(uint32_t), pMesh->getIndexCount(), fp) * sizeof(uint32_t);
        }
        return totalBytesWritten;
    }

    uint32_t NvModelExt::GetPaddedStringLength(const std::string& str) const
    {
        // Account for null character and round up to the next 4-byte multiple (+1+3)/4*4
        return (((str.length() + 4) / 4) * 4);  
    }

    int32_t NvModelExt::WritePaddedString(FILE* fp, const std::string& str) const
    {
        size_t stringSize = str.length() + 1;
        uint32_t bytesWritten = fwrite(str.c_str(), sizeof(char), stringSize, fp) * sizeof(char);

        // Determine if we need to pad out the string with zeroes
        uint32_t unaligned = stringSize % 4;
        if (unaligned != 0)
        {
            // Number of characters, including the null terminator, is not a multiple of four.
            // Pad out the string name with extra zeroes to make it so
            bytesWritten += fwrite("\0\0\0", sizeof(char), 4 - unaligned, fp) * sizeof(char);
        }
        return bytesWritten;
    }

}
