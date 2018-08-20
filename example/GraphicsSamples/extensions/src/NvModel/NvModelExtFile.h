//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtFile.h
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
#ifndef NVMODELEXTFILE_H_
#define NVMODELEXTFILE_H_

namespace Nv
{
    static const uint32_t NVMODEL_FILE_VERSION = 4;

    // File structure (v4):
    // All structures and component elements MUST be 4-byte aligned
    // NvModelExtFileHeader
    // NvModelTextureBlock
    // SkeletonDataBlock
    // M x NvMaterialBlock
    // N x NvModelSubMeshBlock

    struct NvModelExtFileHeader_v2 {
        uint8_t _magic[4];
        uint32_t _headerSize; // includes magic
        uint32_t _version;
        uint32_t _subMeshCount;
        uint32_t _matCount;
        float _boundingBoxMin[3];
        float _boundingBoxMax[3];
        float _boundingBoxCenter[3];
        uint32_t _textureCount;
        uint32_t _textureBlockSize;
        // ptr + headersize = vertex base
        // ptr + headersize + vertexCount * vertexSize = index base
    };

	struct NvModelExtFileHeader {
		uint8_t _magic[4];
		uint32_t _headerSize; // includes magic
		uint32_t _version;
		uint32_t _subMeshCount;
		uint32_t _matCount;
		float _boundingBoxMin[3];
		float _boundingBoxMax[3];
		float _boundingBoxCenter[3];
		// ptr + headersize = vertex base
		// ptr + headersize + vertexCount * vertexSize = index base
	};

    struct NvModelTextureBlockHeader {
        uint32_t _textureCount;
        uint32_t _textureBlockSize;  // 
    };

    struct NvModelTextureDesc
    {
        enum MapMode
        {
            MapMode_Wrap = 0x0,
            MapMode_Clamp = 0x1,
            MapMode_Mirror = 0x2,
            MapMode_Force32Bit = 0x7FFFFFFF
        };

        enum FilterMode
        {
            FilterMode_Nearest = 0x0,
            FilterMode_Linear = 0x1,
            FilterMode_Force32Bit = 0x7FFFFFFF
        };

        int32_t _textureIndex;
        int32_t _UVIndex;
        MapMode _mapModeS;
        MapMode _mapModeT;
        MapMode _mapModeU;
        FilterMode _minFilter;
    };

    struct NvModelSkeletonDataBlockHeader {
        uint32_t _boneCount;
        uint32_t _skeletonBlockSize;
    };

    struct NvModelBoneData {
        int32_t _parentIndex;
        float _parentRelTransform[16];
        int32_t _nameLength;
        int32_t _numChildren;
        int32_t _numMeshes;
    };

    struct NvModelSubMeshHeader_v1 {
        uint32_t _vertexCount;
        uint32_t _indexCount;
        uint32_t _vertexSize; // size of each vert IN BYTES!
        uint32_t _indexSize; // size of each index IN BYTES!
        int32_t _pOffset;
        int32_t _nOffset;
        int32_t _tcOffset;
        int32_t _sTanOffset;
        int32_t _cOffset;
        int32_t _posSize;
        int32_t _tcSize;
        int32_t _vertArrayBase; // offset in bytes from the start of the file
        int32_t _indexArrayBase; // offset in bytes from the start of the file
        int32_t _matIndex;
    };

    struct NvModelSubMeshHeader_v3 {
        uint32_t _vertexCount;
        uint32_t _indexCount;
        uint32_t _vertexSize; // size of each vert IN BYTES!
        uint32_t _indexSize; // size of each index IN BYTES!
        int32_t _pOffset;
        int32_t _nOffset;
        int32_t _tcOffset;
        int32_t _sTanOffset;
        int32_t _cOffset;
        int32_t _boneIndexOffset;
        int32_t _boneWeightOffset;
        int32_t _posSize;
        int32_t _tcSize;
        int32_t _colorCount;
        int32_t _bonesPerVertex;
        int32_t _boneMapCount;

        // offset in bytes from the start of the block
        // (in version 1, this was the offset in bytes 
        //  from the start of the file)
        int32_t _vertArrayBase; 

        // offset in bytes from the start of the block
        // (in version 1, this was the offset in bytes 
        //  from the start of the file)
        int32_t _indexArrayBase;
        int32_t _matIndex;
    };

    struct NvModelSubMeshHeader {
        uint32_t _vertexCount;
        uint32_t _indexCount;
        uint32_t _vertexSize; // size of each vert IN BYTES!
        uint32_t _indexSize; // size of each index IN BYTES!
        int32_t _pOffset;
        int32_t _nOffset;
        int32_t _tcOffset;
        int32_t _sTanOffset;
        int32_t _cOffset;
        int32_t _boneIndexOffset;
        int32_t _boneWeightOffset;
        int32_t _posSize;
        int32_t _tcSize;
        int32_t _colorCount;
        int32_t _bonesPerVertex;
        int32_t _boneMapCount;

        // offset in bytes from the start of the block
        // (in version 1, this was the offset in bytes 
        //  from the start of the file)
        int32_t _vertArrayBase;

        // offset in bytes from the start of the block
        // (in version 1, this was the offset in bytes 
        //  from the start of the file)
        int32_t _indexArrayBase;
        int32_t _matIndex;

        int32_t _parentBone;
    };

    struct NvModelMaterialHeader_v1 {
		float _ambient[3];
		float _diffuse[3];
		float _emissive[3];
		float _alpha;
		float _specular[3];
		float _shininess;
		float _opticalDensity;
		float _transmissionFilter[3];
		int32_t _ambientTexture;
		int32_t _diffuseTexture;
		int32_t _specularTexture;
		int32_t _bumpMapTexture;
		int32_t _reflectionTexture;
		int32_t _displacementMapTexture;
		int32_t _specularPowerTexture;
		int32_t _alphaMapTexture;
		int32_t _decalTexture;
		int32_t _illumModel;
    };

	struct NvModelMaterialHeader {
        uint32_t _materialBlockSize; // includes header + TextureDesc array
        float _ambient[3];
		float _diffuse[3];
		float _emissive[3];
		float _alpha;
		float _specular[3];
		float _shininess;
		float _opticalDensity;
		float _transmissionFilter[3];
		int32_t _ambientTextureOffset;
        int32_t _ambientTextureCount;
        int32_t _diffuseTextureOffset;
        int32_t _diffuseTextureCount;
        int32_t _specularTextureOffset;
        int32_t _specularTextureCount;
        int32_t _bumpMapTextureOffset;
        int32_t _bumpMapTextureCount;
        int32_t _reflectionTextureOffset;
        int32_t _reflectionTextureCount;
        int32_t _displacementMapTextureOffset;
        int32_t _displacementMapTextureCount;
        int32_t _specularPowerTextureOffset;
        int32_t _specularPowerTextureCount;
        int32_t _alphaMapTextureOffset;
        int32_t _alphaMapTextureCount;
        int32_t _decalTextureOffset;
        int32_t _decalTextureCount;
        int32_t _illumModel;
	};

}

#endif
