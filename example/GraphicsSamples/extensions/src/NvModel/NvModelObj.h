//----------------------------------------------------------------------------------
// File:        NvModel/NvModelObj.h
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

#ifndef NV_MODEL_OBJ_H
#define NV_MODEL_OBJ_H

#include <NvSimpleTypes.h>


#include "NvModel/NvModel.h"
#include <vector>
#include "NV/NvMath.h"

/// \file
/// GL-compatible geometric object loader and optimizer

/// Non-rendering geometry model.
/// Graphics-API-agnostic geometric model class, including model loading from
/// OBJ file data, optimization, bounding volumes and rescaling.  
/// Can compute addition items such as facet normals and tangents as needed.
class NvModelObj : public NvModel {
public:

    /// Create a model.
    /// Creates an empty model
	static NvModel* CreateFromObj(uint8_t* data, float scale, bool computeNormals, bool computeTangents);
	static NvModel* CreateFromPreprocessed(uint8_t* data);

	virtual ~NvModelObj() {}

    /// Load raw model from OBJ data.
    /// Loads a model from the given block of raw OBJ-file data
    /// \param[in] fileData a pointer to the in-memory representation of the OBJ file.
    /// This data is not cached locally and can be freed once the function returns
    /// \return true on success and false on failure
	bool loadFromFileData(uint8_t*fileData);

    /// Process a model into rendering-friendly form.
    /// This function takes the raw model data in the internal
    ///  structures, and attempts to bring it to a format directly
    ///  accepted for vertex array style rendering. This means that
    ///  a unique compiled vertex will exist for each unique
    ///  combination of position, normal, tex coords, etc that are
    ///  used in the model. The prim parameter, tells the model
    ///  what type of index list to compile. By default it compiles
    ///  a simple triangle mesh with no connectivity. 
    /// \param[in] prim the desired primitive type that will be used for rendering;
    /// the target of the compilation operation
    void compileModel( NvModelPrimType::Enum prim = NvModelPrimType::TRIANGLES);

    ///  Computes an AABB from the data.
    /// This function returns the points defining the axis-
    /// aligned bounding box containing the model.
    /// \param[out] minVal the returned minimum corner of the AABB
    /// \param[out] maxVal the returned maximum corner of the AABB
    void computeBoundingBox( nv::vec3f &minVal, nv::vec3f &maxVal);

    /// Rescale the model to the desired radius and centers it at the origin.
    /// Rescales object based on bounding box such that the new bounding box
    /// "radius" (half the diagonal of the bounding box) is the given target value and
    /// translates the vertices so that the bounding box is centered at the origin
    /// \param[in] radius the desired target radius of the geometry
    void rescale( float radius);

    /// Compute tangent vectors in the "S" UV direction.
    /// This function computes tangents in the s direction on
    /// the model. It operates on the raw data, so it should only
    /// be used before compiling a model into a HW friendly form.
    /// This can cause model expansion, since it can keep vertices
    /// from being shared.  Thus it should be used only when the results
    /// are required by the rendering method
    void computeTangents();

    /// Compute per-vertex normals.
    /// This function computes vertex normals for a model
    /// which did not have them. It computes them on the raw
    /// data, so it should be done before compiling the model
    /// into a HW friendly format.
    void computeNormals();

    /// Remove zero-area/length primitives.
    /// Removes primitives that will add nothing to the rendered result
    void removeDegeneratePrims();

	///@{
	/// Vertex data existence queries.
	/// \return true if the given vertex attribute exists in the primitive and false if not
	bool hasNormals() const { return _normals.size() > 0; }
	bool hasTexCoords() const { return _texCoords.size() > 0; }
	bool hasTangents() const { return _sTangents.size() > 0; }
	bool hasColors() const { return _colors.size() > 0; }
	///@}

protected:
	NvModelObj() { }

    //data structures for model data, not optimized for rendering
    std::vector<float> _positions;
    std::vector<float> _normals;
    std::vector<float> _texCoords;
    std::vector<float> _sTangents;
    std::vector<float> _colors;

    std::vector<uint32_t> _pIndex;
    std::vector<uint32_t> _nIndex;
    std::vector<uint32_t> _tIndex;
    std::vector<uint32_t> _tanIndex;
    std::vector<uint32_t> _cIndex;
};

#endif
