//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtObj.cpp
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
#include "NvModelExtObj.h"

#include <NvAssert.h>
#include <NV/NvTokenizer.h>
#include "NvModelMeshFace.h"
#include "NvModelSubMeshObj.h"

namespace Nv
{
// Debug helper to ignore all material definitions in the model
// and instead create a material for each smoothing group in 
// the model, using a color for each one chosen from a table. 
// This can be used to identify bad smoothing group definitions
// which can often explain odd normals and lighting.
#define DEBUG_SMOOTHING_AS_MATS 0

#if DEBUG_SMOOTHING_AS_MATS
    static const uint32_t sColorCount = 23;
    static const nv::vec3f sColors[sColorCount] =
    {
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },

        { 0.0f, 0.0f, 0.5f },
        { 0.0f, 0.5f, 0.0f },
        { 0.5f, 0.0f, 0.0f },
        { 0.0f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.0f },
        { 0.5f, 0.0f, 0.5f },
        { 0.5f, 0.5f, 0.5f },

        { 0.0f, 0.5f, 1.0f },
        { 0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 1.0f },

        { 0.0f, 1.0f, 0.5f },
        { 0.5f, 1.0f, 0.0f },
        { 0.5f, 1.0f, 0.5f },

        { 1.0f, 0.0f, 0.5f },
        { 1.0f, 0.5f, 0.0f },
        { 1.0f, 0.5f, 0.5f }
    };
#endif

	NvModelExtObj* NvModelExtObj::Create(const char* filename, float scale, 
		bool generateNormals, bool generateTangents,
		float vertMergeThreshold, float normMergeThreshold, uint32_t initialVertCount)
	{
		NvModelExtObj* pModel = new NvModelExtObj(vertMergeThreshold, normMergeThreshold);
		pModel->LoadObjFromFile(filename);
		pModel->RescaleToOrigin(scale);
		if (generateNormals)
			pModel->GenerateNormals();

		if (generateTangents)
			pModel->GenerateTangents();

		for (uint32_t i = 0; i < pModel->m_subMeshes.size(); i++) {
			pModel->InitProcessedVerts(i);
			pModel->InitProcessedIndices(i);
		}

		return pModel;
	}

	NvModelExtObj::NvModelExtObj(float vertMergeThreshold, float normMergeThreshold, uint32_t initialVertCount) :
        m_numPositionComponents(3),
        m_positions(vertMergeThreshold, initialVertCount),
        m_numTexCoordComponents(2),
        m_normals(normMergeThreshold, initialVertCount),
        m_texCoords(0.00001f, initialVertCount),
        m_tangents(0.00001f, initialVertCount)
    {
        m_rawMaterials.reserve(32);
        m_subMeshes.reserve(32);
        
        ResetModel();
    }

	NvModelExtObj::~NvModelExtObj()
    {
    }

	void NvModelExtObj::InitializeDefaultMaterial()
    {
        // Initialize our material set and map with a default material
        Material defaultMaterial;
        m_rawMaterials.push_back(defaultMaterial);
		m_materialMap["default"] = 0;
    }

	void NvModelExtObj::ResetModel()
    {
        m_positions.Clear();
        m_normals.Clear();
        m_texCoords.Clear();
        m_tangents.Clear();

        m_rawMaterials.resize(0);
        m_materialMap.clear();
        m_textures.resize(0);
        std::vector<SubMeshObj*>::iterator it = m_subMeshes.begin();
		std::vector<SubMeshObj*>::const_iterator itEnd = m_subMeshes.end();
        for (; it != itEnd; ++it)
        {
            delete (*it);
        }
        m_subMeshes.resize(0);

        m_numPositionComponents = 3;
        m_numTexCoordComponents = 2;
        m_boundingBoxMin = nv::vec3f(0.0f, 0.0f, 0.0f);
        m_boundingBoxMax = nv::vec3f(0.0f, 0.0f, 0.0f);

        InitializeDefaultMaterial();
    }

	SubMesh* NvModelExtObj::GetSubMesh(uint32_t subMeshID)
	{
		return GetSubMeshObj(subMeshID);
	}

	SubMeshObj* NvModelExtObj::GetSubMeshObj(uint32_t subMeshID)
	{
		if (subMeshID >= m_subMeshes.size())
			return NULL;
		return m_subMeshes[subMeshID];
	}

	SubMeshObj* NvModelExtObj::GetSubMeshForMaterial(uint32_t materialId)
    {
        // If a mesh using the given material id exists, return it
		std::vector<SubMeshObj*>::iterator it = m_subMeshes.begin();
		std::vector<SubMeshObj*>::const_iterator itEnd = m_subMeshes.end();
        for (; it != itEnd; ++it)
        {
            if ((*it)->m_materialId == materialId)
            {
                return (*it);
            }
        }

        // No sub mesh found with that material. Create a new one and return it
		SubMeshObj* pSubmesh = new SubMeshObj();
        pSubmesh->m_materialId = materialId;

        m_subMeshes.push_back(pSubmesh);
        return pSubmesh;
    }

	bool NvModelExtObj::LoadObjFromFile(const char* pFileName)
    {
        if (NULL == ms_pLoader)
        {
            return false;
        }

        // Use the provided loader callback to load the file into memory
        char *pData = ms_pLoader->LoadDataFromFile(pFileName);
        if (NULL == pData)
        {
            return false;
        }

        bool result = LoadObjFromMemory(pData);

        // Free the OBJ buffer
        ms_pLoader->ReleaseData(pData);

        return result;
    }

	bool NvModelExtObj::LoadObjFromMemory(const char* pFileData)
    {
        NvTokenizer tok(pFileData, "/");

        int32_t currentMaterial = 0;
        int32_t currentSmoothingGroup = 0;
		SubMeshObj* currentSubMesh = GetSubMeshForMaterial(0);

        bool bHas4CompPos = false;
        bool bHas3CompTex = false;
        bool bBoundingBoxInitialized = false;
        int32_t nextPosIndex = 0;
        int32_t nextNormalIndex = 0;
        int32_t nextTexCoordIndex = 0;

        while (!tok.atEOF())
        {
            if (!tok.readToken())
            {
                tok.consumeToEOL();
                continue; // likely EOL we didn't explicitly handle?
            }

            const char* tmp = tok.getLastTokenPtr();
            uint32_t compCount = 0;

            switch (tmp[0])
            {
            case '#':
            {
                //comment line, eat the remainder
                tok.consumeToEOL();
                break;
            }
            case 'v':
            {
                // Some kind of vertex component
                switch (tmp[1])
                {
                case '\0':
                {
                    nv::vec4f pos;
                    //vertex position, 3 or 4 components

                    compCount = tok.getTokenFloatArray((float*)pos, 4);
					if (compCount > 3) compCount = 3;
					pos.w = 1.0f;  //default w coordinate
					NV_ASSERT(compCount > 2 && compCount < 5);
                    if (bBoundingBoxInitialized)
                    {
                        // Grow our bounding box, if necessary
                        m_boundingBoxMin = nv::min(m_boundingBoxMin, (nv::vec3f)pos);
                        m_boundingBoxMax = nv::max(m_boundingBoxMax, (nv::vec3f)pos);
                    }
                    else
                    {
                        // Make sure that our bounding box starts out with a valid, contained point
                        m_boundingBoxMin = (nv::vec3f)pos;
                        m_boundingBoxMax = (nv::vec3f)pos;
                        bBoundingBoxInitialized = true;
                    }

                    int32_t index = m_positions.Append(pos);
                    NV_ASSERT(index != -1);
                    ++nextPosIndex;
                    bHas4CompPos |= (compCount == 4);
                    break;
                }
                case 'n':
                {
                    //normal, 3 components
                    nv::vec3f norm;
                    compCount = tok.getTokenFloatArray((float*)norm, 3);
                    NV_ASSERT(compCount == 3);
                    int32_t index = m_normals.Append(norm);
                    NV_ASSERT(index != -1);
                    ++nextNormalIndex;
                    break;
                }
                case 't':
                {
                    //texcoord, 2 or 3 components
                    nv::vec3f texCoord;
                    texCoord.z = 0.0f;  //default r coordinate
                    compCount = tok.getTokenFloatArray((float*)texCoord, 3);
                    NV_ASSERT(compCount > 1 && compCount < 4);
                    if (compCount == 3)
                    {
                        // Often, the file will contain 3 texture coordinates, but the third is all 0s
                        // Attempt to detect this case and reduce our texture coordinate size of possible.
                        if ((texCoord.z > -0.0001f) && (texCoord.z < 0.0001f))
                        {
                            compCount = 2;
                        }
                    }
                    int32_t index = m_texCoords.Append(texCoord);
                    NV_ASSERT(index != -1);
                    ++nextTexCoordIndex;
                    bHas3CompTex |= (compCount == 3);
                    break;
                }
                case 'p':
                {
                    // Parameter space vertices not supported...
                    break;
                }
                }
                tok.consumeToEOL();
                break;
            }
            case 'f':
            {
                //face
                MeshFace face;
                MeshVertex vert;

                face.m_material = uint32_t(currentMaterial);
                face.m_smoothingGroup = currentSmoothingGroup;
                face.m_pSubMesh = currentSubMesh;

                // determine the type, and read the initial vertex, all entries in a face must have the same format
                // formats are:
                // 1  #         : Position Only
                // 2  #/#       : Position and TexCoord
                // 3  #/#/#     : Position, TexCoord and Normal
                // 4  #//#      : Position and Normal
                OBJFaceFormat format = Face_Invalid;

                // Some obj files have malformed face entries that contain a trailing delimiter when 
                // defining Position and TexCoord vertices.  We'll check for this case so that we
                // can consume the extra delimiter and try to recover
                bool bTrailingDelimiter = false;

                // Indices in OBJ files are 1-based and may be absolute or relative.
                // To facilitate this, we'll read indices into a temporary value that 
                // can be remapped before we store it into our actual data structures.
                int32_t objIndex;
                if (!tok.getTokenInt(objIndex))
                {
                    NV_ASSERT(0);
                    return false;
                }

                // Remap from the files 1-based, and possibly negative, value to a non-negative,
                // 0-based index, then use that index to retrieve the correct index from the 
                // compacting data store of positions.  We'll do this for all vertex components
                // that we read in from the file.
                vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                if (tok.consumeOneDelim())
                {
                    if (tok.consumeOneDelim())
                    {
                        // Two delimiters in a row means it has to be format #//#
                        format = Face_PosNormal;

                        // and we need to read in the normal
                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                        vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                    }
                    else
                    {
                        // The next token is the texture coordinate
                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }

                        vert.m_texcoord = m_texCoords.Remap(RemapObjIndex(objIndex, nextTexCoordIndex));

                        // If there's a delimiter following this, then there's also a normal
                        if (tok.consumeOneDelim())
                        {
                            // Therefore, format #/#/# and we need to fetch the normal
                            // Also, check for malformed face with a trailing delimiter by not consuming
                            // whitespace when reading this token (some bad OBJ files have a #/#/ format
                            // which is not standard).
                            tok.setConsumeWS(false);
                            if (tok.getTokenInt(objIndex))
                            {
                                vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                                format = Face_PosTexNormal;
                            }
                            else
                            {
                                // Set our format to correctly reflect the actual data in the face definition,
                                // but also set our flag so we can consume that extra delimiter each time
                                bTrailingDelimiter = true;
                                format = Face_PosTex;
                            }
                            // Restoring our setting to consume whitespace
                            tok.setConsumeWS(true);
                        }
                        else
                        {
                            // otherwise it's format #/#, so no normal
                            format = Face_PosTex;
                        }
                    }
                }
                else
                {
                    // A single token and no delimiters means a position only format
                    format = Face_PosOnly;
                }

                // Add our new vertex to the submesh, checking to see if one
                // just like it has already been added, using the original one
                // instead if so.
                face.m_verts[0] = currentSubMesh->FindOrAddVertex(vert);
                switch (format)
                {
                case Face_PosOnly:
                {
                    // Get the second vertex
                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));
                    face.m_verts[1] = currentSubMesh->FindOrAddVertex(vert);

                    // If there are more than three vertices in this face, then we need to create
                    // a triangle fan.  If there are only three vertices, then it will be a fan of
                    // one triangle.
                    bool bGeneratedFaceNormal = false;
                    while (tok.getTokenInt(objIndex))
                    {
                        vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));
                        face.m_verts[2] = currentSubMesh->FindOrAddVertex(vert);

                        // Generate our face normal.  We only need to do this once and reuse
                        // it for all triangles in the face.
                        if (!bGeneratedFaceNormal)
                        {
                            face.CalculateFaceNormal(m_positions.GetVectors());
                            bGeneratedFaceNormal = true;
                        }

                        currentSubMesh->m_rawFaces.push_back(face);

                        // Move this vertex into the second position so that the next vertex,
                        // if there is one, will create a triangle with the first vertex, this 
                        // vertex and the next one.
                        face.m_verts[1] = face.m_verts[2];
                    }
                    break;
                }
                case Face_PosTex:
                {
                    // Get the second vertex
                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                    // Consume the '/' between the position and texture
                    if (!tok.consumeOneDelim())
                    {
                        NV_ASSERT(0);
                        return false;
                    }

                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_texcoord = m_texCoords.Remap(RemapObjIndex(objIndex, nextTexCoordIndex));

                    if (bTrailingDelimiter)
                    {
                        if (!tok.consumeOneDelim())
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                    }
                    face.m_verts[1] = currentSubMesh->FindOrAddVertex(vert);

                    // If there are more than three vertices in this face, then we need to create
                    // a triangle fan.  If there are only three vertices, then it will be a fan of
                    // one triangle.
                    bool bGeneratedFaceNormal = false;
                    while (tok.getTokenInt(objIndex))
                    {
                        vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                        // Consume the '/' between the position and texture
                        if (!tok.consumeOneDelim())
                        {
                            NV_ASSERT(0);
                            return false;
                        }

                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                        vert.m_texcoord = m_texCoords.Remap(RemapObjIndex(objIndex, nextTexCoordIndex));
                        face.m_verts[2] = currentSubMesh->FindOrAddVertex(vert);

                        // Generate our face normal.  We only need to do this once and reuse
                        // it for all triangles in the face.
                        if (!bGeneratedFaceNormal)
                        {
                            face.CalculateFaceNormal(m_positions.GetVectors());
                            bGeneratedFaceNormal = true;
                        }

                        currentSubMesh->m_rawFaces.push_back(face);

                        // Move this vertex into the second position so that the next vertex,
                        // if there is one, will create a triangle with the first vertex, this 
                        // vertex and the next one.
                        face.m_verts[1] = face.m_verts[2];
                    }
                    break;
                }
                case Face_PosTexNormal:
                {
                    // Get the second vertex
                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                    // Consume the '/' between the position and texture
                    if (!tok.consumeOneDelim())
                    {
                        NV_ASSERT(0);
                        return false;
                    }

                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_texcoord = m_texCoords.Remap(RemapObjIndex(objIndex, nextTexCoordIndex));

                    // Consume the '/' between the texture and the normal
                    if (!tok.consumeOneDelim())
                    {
                        NV_ASSERT(0);
                        return false;
                    }

                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                    face.m_verts[1] = currentSubMesh->FindOrAddVertex(vert);

                    // If there are more than three vertices in this face, then we need to create
                    // a triangle fan.  If there are only three vertices, then it will be a fan of
                    // one triangle.
                    while (tok.getTokenInt(objIndex))
                    {
                        vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                        // Consume the '/' between the position and texture
                        if (!tok.consumeOneDelim())
                        {
                            NV_ASSERT(0);
                            return false;
                        }

                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                        vert.m_texcoord = m_texCoords.Remap(RemapObjIndex(objIndex, nextTexCoordIndex));

                        // Consume the '/' between the texture and the normal
                        if (!tok.consumeOneDelim())
                        {
                            NV_ASSERT(0);
                            return false;
                        }

                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                        vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                        face.m_verts[2] = currentSubMesh->FindOrAddVertex(vert);

                        currentSubMesh->m_rawFaces.push_back(face);

                        // Move this vertex into the second position so that the next vertex,
                        // if there is one, will create a triangle with the first vertex, this 
                        // vertex and the next one.
                        face.m_verts[1] = face.m_verts[2];
                    }
                    break;
                }
                case Face_PosNormal:
                {
                    // Get the second vertex
                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));

                    // Consume the '//' between the position and normal indices
                    if (!tok.consumeOneDelim() || !tok.consumeOneDelim())
                    {
                        NV_ASSERT(0);
                        return false;
                    }

                    if (!tok.getTokenInt(objIndex))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                    face.m_verts[1] = currentSubMesh->FindOrAddVertex(vert);

                    // If there are more than three vertices in this face, then we need to create
                    // a triangle fan.  If there are only three vertices, then it will be a fan with
                    // one triangle.
                    while (tok.getTokenInt(objIndex))
                    {
                        vert.m_pos = m_positions.Remap(RemapObjIndex(objIndex, nextPosIndex));
                        NV_ASSERT(RemapObjIndex(objIndex, nextPosIndex) == (objIndex - 1));

                        // Consume the '//' between the position and normal indices
                        if (!tok.consumeOneDelim() || !tok.consumeOneDelim())
                        {
                            NV_ASSERT(0);
                            return false;
                        }

                        if (!tok.getTokenInt(objIndex))
                        {
                            NV_ASSERT(0);
                            return false;
                        }
                        vert.m_normal = m_normals.Remap(RemapObjIndex(objIndex, nextNormalIndex));
                        face.m_verts[2] = currentSubMesh->FindOrAddVertex(vert);

                        currentSubMesh->m_rawFaces.push_back(face);

                        // Move this vertex into the second position so that the next vertex,
                        // if there is one, will create a triangle with the first vertex, this 
                        // vertex and the next one.
                        face.m_verts[1] = face.m_verts[2];
                    }
                    break;
                }
                default:
                {
                    NV_ASSERT(0);
                    return false;
                }
                }
                tok.consumeToEOL();
                break;
            }
            case 's':
                if (!tok.getTokenInt(currentSmoothingGroup)) // should return 0 if no conversion possible, as in the case of the 'off' setting
                {
                    NV_ASSERT(0);
                    return false;
                }
                tok.consumeToEOL();

#if DEBUG_SMOOTHING_AS_MATS
                {
                    // See if the material already exists
                    char tmp[16];
                    sprintf_s(tmp, 16, "%d", currentSmoothingGroup);
                    std::string materialName = tmp;

                    MaterialMap::iterator fIt = m_materialMap.find(materialName);
                    if (fIt == m_materialMap.end())
                    {
                        // We need to add the material
                        currentMaterial = m_rawMaterials.size();
                        m_materialMap[materialName] = currentMaterial;

                        Material newMaterial;
                        newMaterial.m_name = materialName;
                        newMaterial.m_diffuse = sColors[currentMaterial % sColorCount];
                        m_rawMaterials.push_back(newMaterial);
                    }
                    else
                    {
                        currentMaterial = fIt->second;
                    }
                    currentSubMesh = GetSubMeshForMaterial(currentMaterial);
                }
#endif
                break;
            case 'm':
            {
#if DEBUG_SMOOTHING_AS_MATS
                // Using artificially constructed materials, defined by smoothing groups, so 
                // ignore all real materials
                tok.consumeToEOL();
                break; 
#endif
                // mtllib
                std::string materialLibName;
                if (!tok.getTokenString(materialLibName))
                {
                    NV_ASSERT(0);
                    return false;
                }

                // Load the material library so that subsequent faces can use the materials it defines
                LoadMaterialLibraryFromFile(materialLibName);

                tok.consumeToEOL();
                break;
            }
            case 'u':
            {
#if DEBUG_SMOOTHING_AS_MATS
                tok.consumeToEOL();
                break;
#endif
                // usemtl
                std::string materialName;
                if (!tok.getTokenString(materialName))
                {
                    NV_ASSERT(0);
                    return false;
                }

                // See if the material already exists
                MaterialMap::iterator fIt = m_materialMap.find(materialName);
                if (fIt == m_materialMap.end())
                {
                    // We need to add the material
                    currentMaterial = int32_t(m_rawMaterials.size());
                    m_materialMap[materialName] = currentMaterial;

                    Material newMaterial;
                    m_rawMaterials.push_back(newMaterial);
                }
                else
                {
                    currentMaterial = fIt->second;
                }

                // Switch to the submesh that uses the active material
                currentSubMesh = GetSubMeshForMaterial(currentMaterial);
                tok.consumeToEOL();
                break;
            }
            case 'g':
            case 'o':
                //all presently ignored
            default:
                tok.consumeToEOL();
            }
        }

        m_numPositionComponents = bHas4CompPos ? 4 : 3;
        m_numTexCoordComponents = bHas3CompTex ? 3 : 2;

        OptimizeModel();

        return true;
    }

	void NvModelExtObj::OptimizeModel()
    {
        RemoveEmptySubmeshes();
    }

	void NvModelExtObj::RemoveEmptySubmeshes()
    {
        // Remove any submeshes that contain no faces
        std::vector<SubMeshObj*>::iterator smIt = m_subMeshes.begin();
        while (smIt != m_subMeshes.end())
        {
            if ((*smIt)->m_rawFaces.empty())
            {
                delete (*smIt);
                smIt = m_subMeshes.erase(smIt);
            }
            else
            {
                ++smIt;
            }
        }
    }

	void NvModelExtObj::LoadMaterialLibraryFromFile(const std::string& libName)
    {
        if (NULL == ms_pLoader)
        {
            return;
        }

        char *pData = ms_pLoader->LoadDataFromFile(libName.c_str());
        if (NULL == pData)
        {
            return;
        }

        LoadMaterialLibraryFromMemory(pData);
        ms_pLoader->ReleaseData(pData);
    }

	void NvModelExtObj::LoadMaterialLibraryFromMemory(const char* pLoadData)
    {
        NvTokenizer tok(pLoadData, "");
        Material* pCurrentMaterial = NULL;

        while (!tok.atEOF())
        {
            if (!tok.readToken())
            {
                tok.consumeToEOL();
                continue; // likely EOL we didn't explicitly handle?
            }

            const char* tmp = tok.getLastTokenPtr();

            switch (tmp[0])
            {
            case '#':
            {
                //comment line, eat the remainder
                tok.consumeToEOL();
                break;
            }
            case 'n':
            {
                // newmtl
                if (0 != strncmp(tmp, "newmtl", 6))
                {
                    // Unsupported token
                    tok.consumeToEOL();
                    break;
                }

                std::string materialName;
                if (!tok.getTokenString(materialName))
                {
                    NV_ASSERT(0);
                    return;
                }

                tok.consumeToEOL();

                // See if this is a redefinition of an existing material
                MaterialMap::iterator fIt = m_materialMap.find(materialName);
                if (fIt != m_materialMap.end())
                {
                    // Existing material
                    uint32_t materialId = fIt->second;
                    NV_ASSERT(materialId < m_rawMaterials.size());
                    pCurrentMaterial = &(m_rawMaterials[materialId]);
                    break;
                }

                // New material, so we need to create it before filling it in
                Material newMaterial;
                uint32_t materialId = uint32_t(m_rawMaterials.size());
                m_rawMaterials.push_back(newMaterial);
                pCurrentMaterial = &(m_rawMaterials.back());
                m_materialMap[materialName] = materialId;
                break;
            }
            case 'K':
            {
                switch (tmp[1])
                {
                case 'a':
                {
                    // Ambient surface color
                    if (!tok.getTokenFloatArray((float*)(pCurrentMaterial->m_ambient), 3))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    tok.consumeToEOL();
                    break;
                }
                case 'd':
                {
                    // Diffuse surface color
                    if (!tok.getTokenFloatArray((float*)(pCurrentMaterial->m_diffuse), 3))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    tok.consumeToEOL();
                    break;
                }
                case 's':
                {
                    // Specular surface color
                    if (!tok.getTokenFloatArray((float*)(pCurrentMaterial->m_specular), 3))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    tok.consumeToEOL();
                    break;
                }
                case 'e':
                {
                    // Emissive surface color
                    if (!tok.getTokenFloatArray((float*)(pCurrentMaterial->m_emissive), 3))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    tok.consumeToEOL();
                    break;
                }
                default:
                {
                    // Unknown color type
                    tok.consumeToEOL();
                    break;
                }
                }
                break;
            }
            case 'N':
            {
                if (tmp[1] == 'i')
                {
                    // Optical Density
                    if (!tok.getTokenFloat(pCurrentMaterial->m_opticalDensity))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                }
                else if (tmp[1] == 's')
                {
                    // Specular exponent
                    float shininess;
                    if (!tok.getTokenFloat(shininess))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    pCurrentMaterial->m_shininess = (uint32_t)shininess;
                }
                tok.consumeToEOL();
                break;
            }
            case 'T':
            {
                if (tmp[1] == 'f')
                {
                    // Transmission Filter
                    if (!tok.getTokenFloatArray((float*)(pCurrentMaterial->m_transmissionFilter), 3))
                    {
                        NV_ASSERT(0);
                        return;
                    }
                    tok.consumeToEOL();
                    break;
                }

                // Check for the Tr version of alpha/dissolve/Transparent value
                if (tmp[1] != 'r')
                {
                    tok.consumeToEOL();
                    break;
                }
                // Matches Tr, so fall through and handle the same as 'd'
            }
            case 'd':
            {
                // Dissolve
                if (!tok.getTokenFloat(pCurrentMaterial->m_alpha))
                {
                    NV_ASSERT(0);
                    return;
                }
                tok.consumeToEOL();
                break;
            }
            case 'i':
            {
                // Illlumination model
                if (0 != strncmp(tmp, "illum", 5))
                {
                    // Unsupported line type
                    tok.consumeToEOL();
                    break;
                }
                int32_t illum;
                if (!tok.getTokenInt(illum))
                {
                    NV_ASSERT(0);
                    return;
                }
                pCurrentMaterial->m_illumModel = static_cast<Material::IlluminationModel>(illum);
                tok.consumeToEOL();
                break;
            }
            case 'm':
            {
                TextureDesc d;
                if (0 == strncmp(tmp, "map_Ka", 6))
                {
                    // Ambient Color Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_ambientTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_Kd", 6))
                {
                    // Diffuse Color Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_diffuseTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_Ks", 6))
                {
                    // Specular Color Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_specularTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_bump", 8))
                {
                    // Bump Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_bumpMapTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_Ns", 6))
                {
                    // Specular Power Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_specularPowerTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_d", 5))
                {
                    // Alpha (Dissolve) Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_alphaMapTextures.push_back(d);
                    }
                }
                else if (0 == strncmp(tmp, "map_decal", 9))
                {
                    // Decal Texture Map
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_decalTextures.push_back(d);
                    }
                }
                else
                {
                    // Unknown directive
                    tok.consumeToEOL();
                }
                break;
            }
            case 'b':
            {
                if (0 == strncmp(tmp, "bump", 4))
                {
                    TextureDesc d;
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_bumpMapTextures.push_back(d);
                    }
                }
                else
                {
                    tok.consumeToEOL();
                }
                break;
            }
            case 'r':
            {
                if (0 == strncmp(tmp, "refl", 4))
                {
                    TextureDesc d;
                    if (ReadTextureLine(tok, d))
                    {
                        pCurrentMaterial->m_reflectionTextures.push_back(d);
                    }
                }
                else
                {
                    tok.consumeToEOL();
                }
                break;
            }
            default:
            {
                // Unhandled line
                tok.consumeToEOL();
                break;
            }
            }
        }
    }

	bool NvModelExtObj::ReadTextureLine(NvTokenizer& tok, TextureDesc& desc)
    {
        memset(&desc, 0, sizeof(TextureDesc));
        desc.m_textureIndex = -1;
        Texture texture;

        while (!tok.atEOF())
        {
            if (!tok.readToken())
            {
                tok.consumeToEOL();
                break; // likely EOL we didn't explicitly handle?
            }

            const char* tmp = tok.getLastTokenPtr();

            if (tmp[0] == '-')
            {
                if (0 == strncmp(tmp, "-blendu", 7))
                {
                    // Eat the on/off option
                    std::string dummy;
                    if (!tok.getTokenString(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-blendv", 7))
                {
                    // Eat the on/off option
                    std::string dummy;
                    if (!tok.getTokenString(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-boost", 6))
                {
                    // Eat the boost value
                    float dummy;
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-mm", 3))
                {
                    // Eat the base and gain values
                    float dummy;
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-o", 2))
                {
                    // Eat the origin offset value(s) (there will be 1 to 3)
                    float dummy;
                    // Must be at least 1
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    // 2 and 3 are optional
                    if (tok.getTokenFloat(dummy))
                    {
                        tok.getTokenFloat(dummy);
                    }
                }
                else if (0 == strncmp(tmp, "-s", 2))
                {
                    // Eat the scale value(s) (there will be 1 to 3)
                    float dummy;
                    // Must be at least 1
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    // 2 and 3 are optional
                    if (tok.getTokenFloat(dummy))
                    {
                        tok.getTokenFloat(dummy);
                    }
                }
                else if (0 == strncmp(tmp, "-t", 2))
                {
                    // Eat the Turbulence value(s) (there will be 1 to 3)
                    float dummy;
                    // Must be at least 1
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    // 2 and 3 are optional
                    if (tok.getTokenFloat(dummy))
                    {
                        tok.getTokenFloat(dummy);
                    }
                }
                else if (0 == strncmp(tmp, "-texres", 7))
                {
                    // Eat the resolution option
                    std::string dummy;
                    if (!tok.getTokenString(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-clamp", 6))
                {
                    // Eat the on/off option
                    std::string clamping;
                    if (!tok.getTokenString(clamping))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                    if (clamping == "on")
                    {
                        desc.m_mapModes[0] = desc.m_mapModes[1] = desc.m_mapModes[2] = TextureDesc::MapMode_Clamp;
                    }
                }
                else if (0 == strncmp(tmp, "-bm", 3))
                {
                    // Eat the bump multiplier value
                    float dummy;
                    if (!tok.getTokenFloat(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-imfchan", 8))
                {
                    // Eat the channel option to create a scalar texture from
                    std::string dummy;
                    if (!tok.getTokenString(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
                else if (0 == strncmp(tmp, "-type", 5))
                {
                    // Eat the texture type option 
                    std::string dummy;
                    if (!tok.getTokenString(dummy))
                    {
                        NV_ASSERT(0);
                        return false;
                    }
                }
            }
            else
            {
                // not an option that we can parse. Could it be a filename (if we don't already have one)?
                if (desc.m_textureIndex == -1)
                {
                    size_t len = strlen(tmp);
                    if (len >= 4)
                    {
                        // There are at least enough characters to hold a file extension
                        if (tmp[len - 4] == '.')
                        {
                            // Fourth character from the end is a period, so a definite possibility.
                            // the other likely option is a floating point value, so try to filter those out
                            int i = 3;
                            while (i > 0)
                            {
                                int digit = tmp[len - i] - '0';
                                if ((digit >= 0) && (digit <= 9))
                                {
                                    // Found a number, so probably not our file name
                                    break;
                                }
                                --i;
                            }
                            if (i == 0)
                            {
                                // We didn't find a digit, so use this as our filename
                                texture.m_name = tmp;
                                for (size_t index = 0; index < m_textures.size(); ++index)
                                {
                                    if (m_textures[index].m_name == texture.m_name)
                                    {
                                        desc.m_textureIndex = int32_t(index);
                                        break;
                                    }
                                }
                                if (desc.m_textureIndex == -1)
                                {
                                    // No matching texture found, so this is a new one.
                                    desc.m_textureIndex = int32_t(m_textures.size());
                                    m_textures.push_back(texture);
                                }
                            }
                        }
                    }
                }
            }
        }

        return desc.m_textureIndex != -1;
    }

	void NvModelExtObj::RescaleToOrigin(float radius)
    {
        nv::vec3f r = 0.5f*(m_boundingBoxMax - m_boundingBoxMin);
        nv::vec4f center = nv::vec4f(m_boundingBoxMin + r, 0.0f);
        float oldRadius = std::max(r.x, std::max(r.y, r.z));
        float scale = radius / oldRadius;

        m_positions.RescaleToOrigin(scale, center);
    }

	void NvModelExtObj::GenerateNormals()
    {
        if (m_normals.GetVectorCount() > 0)
        {
            // Normals already exist; no need to generate
            return;
        }

        // Every position will generate at least one normal, but possibly more,
        // depending on smoothing groups and other discontinuities.
        // Start by building up a mapping from each position to every 
        // Face/Vertex that references it, divided by smoothing group.
        uint32_t numPositions = m_positions.GetVectorCount();
        m_normals.Reserve(numPositions);  // Avoid re-allocations the best we can

        VertexFaces vertexReferences;
        vertexReferences.resize(numPositions);

        // Iterate over all submeshes and process their faces, adding references to each
        // vertex that the face refers to
        std::vector<SubMeshObj*>::const_iterator smEnd = m_subMeshes.end();
		for (std::vector<SubMeshObj*>::iterator smIt = m_subMeshes.begin(); smIt != smEnd; ++smIt)
        {
			SubMeshObj* pSubMesh = *smIt;
            std::vector<MeshFace>& faces = pSubMesh->m_rawFaces;

            std::vector<MeshFace>::const_iterator faceEnd = faces.end();
            for (std::vector<MeshFace>::iterator faceIt = faces.begin(); faceIt != faceEnd; ++faceIt)
            {
                MeshFace* pFace = &(*faceIt);
                for (uint32_t vIndex = 0; vIndex < 3; ++vIndex)
                {
                    // Get the index of the vertex
                    uint32_t posIndex = pSubMesh->m_srcVertices[pFace->m_verts[vIndex]].m_pos;
                    NV_ASSERT(posIndex < numPositions);

                    // Find the face group for the smoothing group that the face belongs to, creating
                    // a new one if one doesn't already exist.
                    FaceGroups& faceGroups = vertexReferences[posIndex];
                    ReferringFaceGroup* pGroup = NULL;

                    FaceGroups::const_iterator faceGroupEnd = faceGroups.end();
                    for (FaceGroups::iterator faceGroupIt = faceGroups.begin(); faceGroupIt != faceGroupEnd; ++faceGroupIt)
                    {
                        if (faceGroupIt->m_smoothingGroup == pFace->m_smoothingGroup)
                        {
                            // found one
                            pGroup = &(*faceGroupIt);
                            break;
                        }
                    }
                    if (pGroup == NULL)
                    {
                        faceGroups.resize(faceGroups.size() + 1);
                        pGroup = &(faceGroups.back());
                        pGroup->m_smoothingGroup = pFace->m_smoothingGroup;
                    }

                    pGroup->m_referringFaces.push_back(ReferringFaceGroup::FaceVert(pFace, vIndex));
                }
            }
        }

        // Mapping of positions to MeshVertex is complete.  Now iterate over all of these mappings
        // and generate normals, associating them with the appropriate MeshVertices.
        const NvModelVectorCompactor<nv::vec4f>::Positions::VecArray& positions = m_positions.GetVectors();

        VertexFaces::const_iterator vfEnd = vertexReferences.end();
        for (VertexFaces::iterator vfIt = vertexReferences.begin(); vfIt != vfEnd; ++vfIt)
        {
            FaceGroups& faceGroups = (*vfIt);
            FaceGroups::const_iterator fgEnd = faceGroups.end();
            for (FaceGroups::iterator fgIt = faceGroups.begin(); fgIt != fgEnd; ++fgIt)
            {
                // For each smoothing group of faces, we need to generate an averaged normal, except for
                // smoothing group 0, which is the "smoothing off" group
                ReferringFaceGroup& group = (*fgIt);
                if (group.m_smoothingGroup > 0)
                {
                    nv::vec3f normal(0.0f, 0.0f, 0.0f);

                    ReferringFaceGroup::FaceVerts& faces = group.m_referringFaces;
                    ReferringFaceGroup::FaceVerts::const_iterator fvEnd = faces.end();
                    if (faces.size() == 1)
                    {
                        // Only one face in this group, so just re-use its face normal
                        normal = faces.front().first->m_faceNormal;
                    }
                    else
                    {
                        // Sum up the face normals of each face, where each is weighted by an appropriate factor
                        for (ReferringFaceGroup::FaceVerts::iterator fvIt = faces.begin(); fvIt != fvEnd; ++fvIt)
                        {
                            MeshFace* pFace = fvIt->first;
                            NV_ASSERT(NULL != pFace);
                            normal += (pFace->GetFaceWeight(positions, fvIt->second) * pFace->m_faceNormal);
                        }
                    }

                    // Unitize the normal and add it to the shared list
                    float normalLen = length(normal);
                    if (normalLen > 0.0000001)
                    {
                        normal *= 1.0f / normalLen;
                    }
                    else
                    {
                        // It's a zero-vector, so give it a default value in the positive y so that it's usable, if not correct
                        normal.y = 1.0f;
                    }
                    uint32_t normalIndex = m_normals.Append(normal);

                    // Now point all of those face vertices to their new normal
                    for (ReferringFaceGroup::FaceVerts::iterator fvIt = faces.begin(); fvIt != fvEnd; ++fvIt)
                    {
                        MeshFace* pFace = fvIt->first;
                        NV_ASSERT(NULL != pFace);
                        SubMeshObj* pSubMesh = pFace->m_pSubMesh;
                        NV_ASSERT(NULL != pSubMesh);
                        int32_t newVertIndex = pSubMesh->SetNormal(pFace->m_verts[fvIt->second], normalIndex);
                        pFace->m_verts[fvIt->second] = newVertIndex;
                    }
                }
                else
                {
                    // For each face, add their face normal to the set of normals and point the vertex to it
                    ReferringFaceGroup::FaceVerts::const_iterator fvEnd = group.m_referringFaces.end();
                    for (ReferringFaceGroup::FaceVerts::iterator fvIt = group.m_referringFaces.begin(); fvIt != fvEnd; ++fvIt)
                    {
                        MeshFace* pFace = fvIt->first;
                        NV_ASSERT(NULL != pFace);
                        SubMeshObj* pSubMesh = pFace->m_pSubMesh;
                        NV_ASSERT(NULL != pSubMesh);
                        uint32_t normalIndex = m_normals.Append(pFace->m_faceNormal);
                        int32_t newVertIndex = pSubMesh->SetNormal(pFace->m_verts[fvIt->second], normalIndex);
                        pFace->m_verts[fvIt->second] = newVertIndex;
                    }
                }
            }
        }
    }

	void NvModelExtObj::GenerateTangents()
    {
        if (m_tangents.GetVectorCount() > 0)
        {
            // Tangents already exist; no need to generate
            return;
        }

        if (m_texCoords.GetVectorCount() == 0)
        {
            // We need texture coordinates to generate tangents
            return;
        }

        if (m_normals.GetVectorCount() == 0)
        {
            // We also need normals to generate tangents
            GenerateNormals();
            if (m_normals.GetVectorCount() == 0)
            {
                // Unable to generate the missing normals for some reason 
                return;
            }
        }

        // Iterate over all the vertices in all the faces in all the submeshes and calculate 
        // tangent vectors for each of them.
		std::vector<SubMeshObj*>::const_iterator smEnd = m_subMeshes.end();
		for (std::vector<SubMeshObj*>::iterator smIt = m_subMeshes.begin(); smIt != smEnd; ++smIt)
        {
			SubMeshObj* pSubMesh = *smIt;
            std::vector<MeshFace>& faces = pSubMesh->m_rawFaces;

            if (!(pSubMesh->HasNormals()) || !(pSubMesh->HasTexCoords()))
            {
                // this submesh doesn't have the components that we need to generate tangents
                continue;
            }

            std::vector<MeshFace>::const_iterator faceEnd = faces.end();
            for (std::vector<MeshFace>::iterator faceIt = faces.begin(); faceIt != faceEnd; ++faceIt)
            {
                MeshFace* pFace = &(*faceIt);
				SubMeshObj* pSubMesh = pFace->m_pSubMesh;
				const SubMeshObj::MeshVertexArray& verts = pSubMesh->m_srcVertices;

                // We'll need all three positions and all three UV sets to calculate
                // each tangent, so go ahead and load them all once.
                nv::vec4f positions[3];
                m_positions.GetObject(verts[pFace->m_verts[0]].m_pos, positions[0]);
                m_positions.GetObject(verts[pFace->m_verts[1]].m_pos, positions[1]);
                m_positions.GetObject(verts[pFace->m_verts[2]].m_pos, positions[2]);

                nv::vec3f uvs[3];
                m_texCoords.GetObject(verts[pFace->m_verts[0]].m_texcoord, uvs[0]);
                m_texCoords.GetObject(verts[pFace->m_verts[1]].m_texcoord, uvs[1]);

                for (uint32_t vIndex = 0; vIndex < 3; ++vIndex)
                {
                    // Given the current index, determine the index
                    // of the adjacent vertices in the definition of
                    // the face.
                    uint32_t nextIndex = (vIndex + 1) % 3;
                    uint32_t lastIndex = (vIndex + 2) % 3;

                    nv::vec3f tangent;

                    //compute the edge and tc differentials
                    nv::vec3f dp0 = (nv::vec3f)(positions[nextIndex] - positions[vIndex]);
                    nv::vec3f dp1 = (nv::vec3f)(positions[lastIndex] - positions[vIndex]);
                    nv::vec2f dst0 = (nv::vec2f)(uvs[nextIndex] - uvs[vIndex]);
                    nv::vec2f dst1 = (nv::vec2f)(uvs[lastIndex] - uvs[vIndex]);

                    // Make sure there's no divide by 0
                    float factor = 1.0f;
                    float denom = dst0[0] * dst1[1] - dst1[0] * dst0[1];
                    if (fabsf(denom) > 0.00001f)
                    {
                        factor /= denom;
                    }

                    //compute sTangent
                    tangent.x = dp0.x * dst1.y - dp1.x * dst0.y;
                    tangent.y = dp0.y * dst1.y - dp1.y * dst0.y;
                    tangent.z = dp0.z * dst1.y - dp1.z * dst0.y;
                    tangent *= factor;
                    float tangentLen = length(tangent);
                    if (tangentLen > 0.000001)
                    {
                        tangent = normalize(tangent);
                    }
                    else
                    {
                        // It's a zero-vector, so give it a default value in the positive x so that it's usable, if not correct
                        tangent.x = 1.0f;

                    }
                    uint32_t tangentIndex = m_tangents.Append(tangent);
                    int32_t newVertIndex = pSubMesh->SetTangent(pFace->m_verts[vIndex], tangentIndex);
                    pFace->m_verts[vIndex] = newVertIndex;
                }
            }
        }
    }

	bool NvModelExtObj::InitProcessedIndices(uint32_t subMeshID)
    {
		if (subMeshID >= m_subMeshes.size())
			return false;
		SubMeshObj* pSubMesh = m_subMeshes[subMeshID];
        if (NULL == pSubMesh)
        {
            return false;
        }

		pSubMesh->m_indexCount = uint32_t(pSubMesh->m_rawFaces.size()) * 3;

		uint32_t* pBuffer = new uint32_t[pSubMesh->m_indexCount];
        uint32_t* pCurrIndex = pBuffer;
        std::vector<MeshFace>::iterator faceIt = pSubMesh->m_rawFaces.begin();
        std::vector<MeshFace>::const_iterator faceEnd = pSubMesh->m_rawFaces.end();
        for (uint32_t* pCurrIndex = pBuffer; faceIt != faceEnd; ++faceIt)
        {
            for (uint32_t vIndex = 0; vIndex < 3; ++vIndex, ++pCurrIndex)
            {
                *pCurrIndex = (*faceIt).m_verts[vIndex];
            }
        }

		pSubMesh->m_indices = pBuffer;
        return true;
    }

	bool NvModelExtObj::InitProcessedVerts(uint32_t subMeshID)
    {
		if (subMeshID >= m_subMeshes.size())
			return false;
		SubMeshObj* pSubMesh = m_subMeshes[subMeshID];
		if (NULL == pSubMesh)
        {
            return false;
        }

		// calculate the vertex size and the vertex offsets
		uint32_t vertexSize = 0; // In Floats

		// We will always have positions in our vertex, and we will put 
		// them first in the vertex layout
		vertexSize += 3;

		// Account for normals, if there are any
		pSubMesh->m_normalOffset = -1;
		if (pSubMesh->HasNormals())
		{
			pSubMesh->m_normalOffset = vertexSize;
			vertexSize += 3;
		}

		// Account for texture coordinates, if there are any
		pSubMesh->m_texCoordOffset = -1;
		if (pSubMesh->HasTexCoords())
		{
			pSubMesh->m_texCoordOffset = vertexSize;
			vertexSize += 2;
		}

		// Account for tangents, if there are any
		pSubMesh->m_tangentOffset = -1;
		if (pSubMesh->HasTangents())
		{
			pSubMesh->m_tangentOffset = vertexSize;
			vertexSize += 3;
		}

		pSubMesh->m_vertexCount = pSubMesh->m_srcVertices.size();
		pSubMesh->m_vertSize = vertexSize;

		// Allocate a large enough vertex buffer to hold all vertices in the mesh
		pSubMesh->m_vertices = new float[vertexSize * pSubMesh->m_vertexCount];

		float* pBuffer = pSubMesh->m_vertices;

        // Get pointers to the proper positions in the first vertex in the buffer
        float* pCurrPosition = (float*)(pBuffer);
		float* pCurrNormal = (float*)(pBuffer + pSubMesh->m_normalOffset);
		float* pCurrTexCoord = (float*)(pBuffer + pSubMesh->m_texCoordOffset);
		float* pCurrTangent = (float*)(pBuffer + pSubMesh->m_tangentOffset);

        const std::vector<nv::vec4f>& positions = m_positions.GetVectors();
        const std::vector<nv::vec3f>& normals = m_normals.GetVectors();
        const std::vector<nv::vec3f>& texCoords = m_texCoords.GetVectors();
		const std::vector<nv::vec3f>& tangents = m_tangents.GetVectors();

        const SubMeshObj::MeshVertexArray& srcVerts = pSubMesh->m_srcVertices;
		SubMeshObj::MeshVertexArray::const_iterator srcIt = srcVerts.begin();
		SubMeshObj::MeshVertexArray::const_iterator srcEnd = srcVerts.end();
        for (; srcIt != srcEnd; ++srcIt)
        {
            const MeshVertex& vert = (*srcIt);

            memcpy(pCurrPosition, &(positions[vert.m_pos]), sizeof(float) * 3);
			pCurrPosition += vertexSize;

			if (pSubMesh->m_normalOffset >= 0)
            {
                NV_ASSERT(-1 != vert.m_normal);
                memcpy(pCurrNormal, &(normals[vert.m_normal]), sizeof(float) * 3);
				pCurrNormal += vertexSize;
            }
			if (pSubMesh->m_texCoordOffset >= 0)
            {
                NV_ASSERT(-1 != vert.m_texcoord);
                memcpy(pCurrTexCoord, &(texCoords[vert.m_texcoord]), sizeof(float) * 2);
				pCurrTexCoord += vertexSize;
            }
			if (pSubMesh->m_tangentOffset >= 0)
			{
				NV_ASSERT(-1 != vert.m_tangent);
				memcpy(pCurrTangent, &(tangents[vert.m_tangent]), sizeof(float) * 3);
				pCurrTangent += vertexSize;
			}
		}

        return true;
    }
}

