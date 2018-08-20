//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtObj.h
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
#ifndef _NVMODELEXTOBJ_H_
#define _NVMODELEXTOBJ_H_

#include "NvModel/NvModelExt.h"
#include "NvModel/NvModelSubMesh.h"
#include "NvModel/NvModelMaterial.h"
#include "NvModelVectorCompactor.h"
#include <string>
#include <vector>
//#include <ext/hash_map>
#include <unordered_map>
#ifndef _WIN32
#include <tr1/unordered_map>
#endif
class NvTokenizer;

namespace Nv
{
	class MeshFace;
	class SubMeshObj;

    class NvModelExtObj : public NvModelExt
    {
    public:
        /// Factory method to create a model to be loaded
        /// \param[in] vertMergeThreshold Vertices within this distance of each other will be merged together
        /// \param[in] normMergeThreshold Normals whose dot product is within this value of 1.0 will be merged together
        /// \param[in] initialVertCount Initial size of all containers to minimize re-allocations while loading the mesh
        /// \return A pointer to a new, empty model with the given settings
		static NvModelExtObj* Create(const char* filename, float scale, 
			bool generateNormals, bool generateTangents,
			float vertMergeThreshold = 0.01f, float normMergeThreshold = 0.001f, uint32_t initialVertCount = 3000);
		virtual ~NvModelExtObj();

        /// Loads the model data from the OBJ file with the given file name
        /// \param[in] fileName Name of the file containing the OBJ definition to load
        /// \return True if the file was loaded successfully and the model now contains
        ///         the processed data.  False if there was a problem loading and/or
        ///         processing the data.
        bool LoadObjFromFile(const char* fileName);

        /// Loads the model data from the OBJ file in the given memory buffer
        /// \param[in] pLoadData Pointer to the buffer containing the OBJ definition to load
        /// \return True if the OBJ was parsed successfully and the model now contains
        ///         the processed data.  False if there was a problem processing the data.
        bool LoadObjFromMemory(const char* pLoadData);

        virtual uint32_t GetMeshCount() const { return uint32_t(m_subMeshes.size()); }
		
		/// Returns the mesh contained in the model with the given ID
		/// \param[in] subMeshID ID, or index, of the mesh to retrieve
		/// \return A pointer to the mesh with the given ID.  Null if
		///         no mesh exists with that ID.
		virtual SubMesh* GetSubMesh(uint32_t subMeshID);

		SubMeshObj* GetSubMeshObj(uint32_t subMeshID);

		/// Returns the number of materials defined by the model
		/// \return Number of materials defined by the model
        virtual uint32_t GetMaterialCount() const { return uint32_t(m_rawMaterials.size()); }

		/// Returns the material in the model with the given ID
		/// \param[in] materialID ID, or index, of the material to retrieve
		/// \return A pointer to the material with the given ID.  Null if
		///         no material exists with that ID.
		virtual Material* GetMaterial(uint32_t materialID)
		{
			if (materialID >= m_rawMaterials.size())
				return NULL;
			return &(m_rawMaterials[materialID]);
		}

        /// Returns the number of textures used by the model
        /// \return Number of textures used by the model
        virtual uint32_t GetTextureCount() const { return uint32_t(m_textures.size()); }

        /// Returns the name of the texture in the model with the given ID
        /// \param[in] texture ID, or index, of the texture to retrieve
        /// \return A string containing the file name of the texture with
        ///         the given ID.  Null if no texture exists with that ID.
        virtual std::string GetTextureName(uint32_t textureID) const
        {
            if (textureID >= m_textures.size())
                return std::string("");
            return m_textures[textureID].m_name;
        }

		/// Modifies all positions in the model so that the model is centered about the
        /// origin and uniformly scaled such that the largest extent (distance between
        /// the center and the side) is equal to the given radius.
        /// \param[in] radius New largest extent of the mesh's bounding box
        void RescaleToOrigin(float radius);

        /// Iterates over all vertices and generates normals by averaging the normals of all
        /// faces, within the same smoothing group, that contain the vertex.  Any faces within
        /// smoothing group 0 will not have their vertices' normals averaged, but will have their
        /// face normal used for their vertices instead.
        void GenerateNormals();

        /// Iterates over all vertices and generates tangent vectors for them.  Vertices must
        /// have normals and texture coordinates in order to generate tangents.
        void GenerateTangents();

		bool InitProcessedIndices(uint32_t subMeshID);

		bool InitProcessedVerts(uint32_t subMeshID);


    protected:
        // Constructor is protected to force the factory method to be used to create a new NvModelExt
        NvModelExtObj(float vertMergeThreshold, float normMergeThreshold, uint32_t initialVertCount = 3000);

    private:
        // Helper enum used to indicate the format of each face as it
        // is being parsed from the OBJ file
        enum OBJFaceFormat
        {
            Face_Invalid = 0,
            Face_PosOnly = 1,
            Face_PosTex = 2,
            Face_PosTexNormal = 3,
            Face_PosNormal = 4
        };

        // Creates a default material to be used by any mesh that doesn't specify one
        void InitializeDefaultMaterial();

        // Clears out all data to prepare for loading a new model
        void ResetModel();

        /// Loads a material library file, using the NvModelFileLoader, included by an OBJ file 
        /// and processes its material definitions into a usable format.
        /// \param[in] libName Name of the file containing the material library definitions
        void LoadMaterialLibraryFromFile(const std::string& libName);

        /// Loads a material library file from the provided memory buffer
        /// and processes its material definitions into a usable format.
        /// \param[in] pLoadData Pointer to the memory buffer containing the material library definitions
        void LoadMaterialLibraryFromMemory(const char* pLoadData);

        // Performs post-loading optimizations on the mesh to reduce data size
        // and redundancy
        void OptimizeModel();

        // Removes any meshes that have no geometry
        void RemoveEmptySubmeshes();

        /// Helper method to read a texture definition line from a material file
        /// and add it to the set of textures
        /// \param[in] tok The tokenizer being used to parse the library, with its current
        ///                 position at the start of the parameters for the texture
        ///                 definition to be read
        /// \param[out] desc Texture descriptor to initialize with read in parameters
        /// \return True if the line was successfully parsed and matched to a texture,
        ///         False if an error occurred.
        bool ReadTextureLine(NvTokenizer& tok, TextureDesc& desc);

        // Number of floats contained in the positions defined by the OBJ file
        uint32_t m_numPositionComponents;

        // Data store for positions used by vertices in the mesh.  Detects duplicates,
        // within a tolerance, and eliminates them, providing a mapping from the position's
        // original index, based on the order it was added to the store, to its new index
        // in the compacted set of positions.
        NvModelVectorCompactor<nv::vec4f>::Positions m_positions;

        // Data store for normals used by vertices in the mesh. Also removes duplicates
        // like the abover positional one.
        NvModelVectorCompactor<nv::vec3f>::Normals m_normals;

        // Number of floats contained in the texture coordinates defined by the OBJ file
        uint32_t m_numTexCoordComponents;


        // Duplicate removing data store for texture coordinates used by vertices in the mesh.
        NvModelVectorCompactor<nv::vec3f>::Positions m_texCoords;
        // Duplicate removing data store for tangent vectors used by vertices in the mesh.
        NvModelVectorCompactor<nv::vec3f>::Normals m_tangents;

        // Array of material definitions used by the mesh, as defined by any included material libraries
        std::vector<Material> m_rawMaterials;

        // Map from material name to its index in the raw materials array
        typedef std::tr1::unordered_map<std::string, uint32_t> MaterialMap;
        MaterialMap m_materialMap;

        // Simple structure to define a texture used by a material in the mesh.  Currently only
        // contains the name of the texture.  All other texture paramaters defined in the 
        // material library are discarded.
        struct Texture
        {
            std::string m_name;
            bool operator<(const Texture& other) { return (m_name < other.m_name); }
            bool operator==(const Texture& other) { return (m_name == other.m_name); }
        };

        // Array of all textures defined by all materials in the mesh
        std::vector<Texture> m_textures;

        // Array of all sub meshs that comprise the model
        std::vector<SubMeshObj*> m_subMeshes;

        // Method to remap an index in an obj file to the corresponding index in the given vector.
        //
        // Indices in obj files are 1-based.  Since we're using 0-based vectors
        // to store data in, we need to remap the file indices to our vector indices
        // by subtracting one.  Additionally, indices can be negative, indicating that
        // the index is relative to the most recently defined object (-1 is the most
        // recent). Note that this remapping is not the same as, or related to, the remapping
        // done by the vector compactors that mesh data is stored in.  This is simply to map
        // from the numbers read from the OBJ file to a non-negative, 0-based index.
        template <class T>
        int32_t RemapObjIndex(int32_t objIndex, const std::vector<T>& v)
        {
            return ((objIndex > 0) ? (objIndex - 1) : (((int32_t)v.size()) + objIndex));
        }

        int32_t RemapObjIndex(int32_t objIndex, uint32_t count)
        {
            return ((objIndex > 0) ? (objIndex - 1) : (count + objIndex));
        }

        /// Returns the submesh in the model that uses the given material.  There will
        /// be exactly one submesh for each material defined in the model.
        /// \param[in] materialID ID (or index) of the material to retrieve the mesh for
        /// \return A pointer to the submesh that uses the material with the given ID, 
        ///         if there is one.  Null if there is no such material definition or
        ///         associated mesh.
        SubMeshObj* GetSubMeshForMaterial(uint32_t materialID);

        ///////////////////////////////
        // Normal generation helpers //
        ///////////////////////////////

        // Set of faces within a single smoothing group that refer to a vertex
        struct ReferringFaceGroup
        {
            typedef std::pair<MeshFace*, uint32_t> FaceVert;
            typedef std::vector<FaceVert> FaceVerts;

            uint32_t m_smoothingGroup;
            FaceVerts m_referringFaces;
        };

        // Set of all faces that refer to a single vertex, divided by smoothing group
        typedef std::vector<ReferringFaceGroup> FaceGroups;

        // One 'FaceGroups' per position in the shared array
        typedef std::vector<FaceGroups> VertexFaces;
    };
}

#endif // _NVMODELEXT_H_
