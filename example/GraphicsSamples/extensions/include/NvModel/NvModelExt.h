//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExt.h
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
#ifndef _NVMODELEXT_H_
#define _NVMODELEXT_H_

#include "NV/NvMath.h"
#include <string>

namespace Nv
{
	class Material;
	class SubMesh;
    class NvSkeleton;

    // Interface class to be implemented by the caller to allow the loading
    // process to read files, both the mesh and any embedded files such as
    // material libraries.  
    class NvModelFileLoader
    {
    public:
        NvModelFileLoader() {}
        virtual ~NvModelFileLoader() {}

        /// Method to be called to load a resource file
        /// \param[in] fileName Name of the file to load
        /// \return A pointer to a buffer containing the contents of the requested file or
        ///         null if the file could not be read.  The buffer is still owned by the
        ///         file loader and must be kept in memory until ReleaseData() is
        ///         called on the returned buffer.
        virtual char* LoadDataFromFile(const char* fileName) = 0;

        /// Releases the memory used by the file loader
        /// \param[in] pData Pointer to the data to release
        virtual void ReleaseData(char* pData) = 0;
    };

	class NvModelExt
	{
	public:
		/// Create a model from OBJ data
		/// \param[in] filename path/name of the OBJ file data
		/// \param[in] scale the target radius to which we want the model scaled, or <0 if no scaling should be done
		/// \param[in] computeNormals indicate whether per-vertex normals should be estimated and added
		/// \param[in] computeTangents indicate whether per-vertex tangent vectors should be estimated and added
		/// \param[in] vertMergeThreshold the distance between vertices that should be considered "the same" and allow for merging
		/// \param[in] normMergeThreshold the distance between normals that should be considered "the same" and allow for merging
		/// \param[in] initialVertCount the scaling of the internal structures for expected vertex count
		/// \return a pointer to the new model
		static NvModelExt* CreateFromObj(const char* filename, float scale,
			bool generateNormals, bool generateTangents,
			float vertMergeThreshold = 0.01f, float normMergeThreshold = 0.001f, uint32_t initialVertCount = 3000);

		/// Create a model from a preprocessed "NVE" file, which is much faster and more efficient to load than OBJ
		/// \param[in] filename path/name of the NVE file data
		/// \return a pointer to the new model
		static NvModelExt* CreateFromPreprocessed(const char* filename);

		virtual ~NvModelExt();

		/// Sets the file loader object to be used when requesting external files
		/// to be loaded into memory.
		/// \param[in] pLoader Pointer to the file loader to request file data from
		static void SetFileLoader(NvModelFileLoader* pLoader) { ms_pLoader = pLoader; }

		/// Get the point defined by the minimum values in each axis contained
		/// within the axis-aligned bounding box of the model.
		/// \return Vector containing the minimum X,Y and Z of the bounding box
		nv::vec3f GetMinExt() const { return m_boundingBoxMin; }

		/// Get the point defined by the maximum values in each axis contained
		/// within the axis-aligned bounding box of the model.
		/// \return Vector containing the maximum X,Y and Z of the bounding box
        nv::vec3f GetMaxExt() const { return m_boundingBoxMax; }

		/// Get the point defined by the center of the axis-aligned bounding box of the model.
		/// \return Vector containing the center of the bounding box
        nv::vec3f GetCenter() const { return m_boundingBoxCenter; }

		/// Returns the number of meshes contained in the model
		/// \return Number of meshes contained in the model
        virtual uint32_t GetMeshCount() const = 0;

		/// Returns the mesh contained in the model with the given ID
		/// \param[in] subMeshID ID, or index, of the mesh to retrieve
		/// \return A pointer to the mesh with the given ID.  Null if
		///         no mesh exists with that ID.
		virtual SubMesh* GetSubMesh(uint32_t subMeshID) = 0;

		/// Returns the number of materials defined by the model
		/// \return Number of materials defined by the model
        virtual uint32_t GetMaterialCount() const = 0;

		/// Returns the material in the model with the given ID
		/// \param[in] materialID ID, or index, of the material to retrieve
		/// \return A pointer to the material with the given ID.  Null if
		///         no material exists with that ID.
		virtual Material* GetMaterial(uint32_t materialID) = 0;

        /// Returns the number of textures used by the model
        /// \return Number of textures used by the model
        virtual uint32_t GetTextureCount() const = 0;

        /// Returns the name of the texture in the model with the given ID
        /// \param[in] texture ID, or index, of the texture to retrieve
        /// \return A string containing the file name of the texture with
        ///         the given ID.  Null if no texture exists with that ID.
        virtual std::string GetTextureName(uint32_t textureID) const = 0;

        /// Returns a pointer to the skeleton used by the model, if it exists
        /// \return Pointer to the model's skeleton or NULL if the model
        ///         does not have one.
        virtual NvSkeleton* GetSkeleton() { return m_pSkeleton; }

        /// Serializes the model out to a file in a binary format that 
        /// can quickly be loaded back in
        /// \param filename Name of the file in which to write the model's data
        /// \return True if the model was successfully written to the file,
        ///         False if an error occurred during file creation or writing.
		bool WritePreprocessedModel(const char* filename);

	protected:
		/// \privatesection
		// Constructor is protected to force the factory method to be used to create a new NvModelExt
		NvModelExt();

        int32_t WriteFileHeader(FILE* fp) const;
        int32_t WriteTextureBlock(FILE* fp) const;
        int32_t WriteSkeletonBlock(FILE* fp) const;
        int32_t WriteMaterials(FILE* fp);
        int32_t WriteMeshes(FILE* fp);
        int32_t WritePaddedString(FILE* fp, const std::string& str) const;

        uint32_t GetPaddedStringLength(const std::string& str) const;

		// Pointer to the NvModelFileLoader object to use for all File I/O operations
		static NvModelFileLoader* ms_pLoader;

        // Pointer to the skeleton for the model. NULL if it doesn't contain one.
        NvSkeleton* m_pSkeleton;

		// Axis-aligned bounding box definition
		nv::vec3f m_boundingBoxMin;
		nv::vec3f m_boundingBoxMax;
		nv::vec3f m_boundingBoxCenter;
	};
}
#endif // _NVMODELEXT_H_
