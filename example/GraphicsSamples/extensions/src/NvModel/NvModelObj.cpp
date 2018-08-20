//----------------------------------------------------------------------------------
// File:        NvModel/NvModelObj.cpp
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

#include "NvModelObj.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

#include <NvAssert.h>
#include <NV/NvTokenizer.h>

#include <algorithm>

#include <map>
#include <set>
#include <stdio.h>
#include <string.h>

using std::vector;
using std::set;
using std::map;
using std::min;
using std::max;
using namespace nv;

//////////////////////////////////////////////////////////////////////
//
// Local data structures
//
//////////////////////////////////////////////////////////////////////

//
//  Index gathering and ordering structure
////////////////////////////////////////////////////////////
struct IdxSet {
	uint32_t pIndex;
	uint32_t nIndex;
	uint32_t tIndex;
	uint32_t tanIndex;
	uint32_t cIndex;

	bool operator< (const IdxSet &rhs) const {
		if (pIndex < rhs.pIndex)
			return true;
		else if (pIndex == rhs.pIndex) {
			if (nIndex < rhs.nIndex)
				return true;
			else if (nIndex == rhs.nIndex) {

				if (tIndex < rhs.tIndex)
					return true;
				else if (tIndex == rhs.tIndex) {
					if (tanIndex < rhs.tanIndex)
						return true;
					else if (tanIndex == rhs.tanIndex)
						return (cIndex < rhs.cIndex);
				}
			}
		}

		return false;
	}
};

//
//  Edge connectivity structure 
////////////////////////////////////////////////////////////
struct Edge {
	uint32_t pIndex[2]; //position indices

	bool operator< (const Edge &rhs) const {
		return (pIndex[0] == rhs.pIndex[0]) ? (pIndex[1] < rhs.pIndex[1]) : pIndex[0] < rhs.pIndex[0];
	}

	Edge(uint32_t v0, uint32_t v1) {
		pIndex[0] = std::min(v0, v1);
		pIndex[1] = std::max(v0, v1);
	}

private:
	Edge() {} // disallow the default constructor
};

NvModel* NvModelObj::CreateFromObj(uint8_t* data, float scale, bool computeNormals, bool computeTangents) {
	NvModelObj* modelObj = new NvModelObj;
	modelObj->loadFromFileData(data);
	if (computeNormals || computeTangents)
		modelObj->computeNormals();
	if (computeTangents)
		modelObj->computeTangents();
	modelObj->rescale(scale);
	modelObj->compileModel(NvModelPrimType::TRIANGLES);
	return modelObj;
}

bool NvModelObj::loadFromFileData(uint8_t* fileData)
{
    NvTokenizer tok((const char*)fileData, "/");
    
    float val[4];
    int32_t idx[3][3];
    int32_t match;
    char format = 0;
    bool vtx4Comp = false;
    bool tex3Comp = false;
    bool hasTC = false;
    bool hasNormals = false;

#if 0 //def _DEBUG
    tok.setVerbose();
    LOGI("NvModel::loadObjFromFileData..");
#endif

    while ( !tok.atEOF() )
    {
        if (!tok.readToken()) {
            tok.consumeToEOL();
            continue; // likely EOL we didn't explicitly handle?
        }

        const char* tmp = tok.getLastTokenPtr();
        switch (tmp[0]) {
            case '#':
                //comment line, eat the remainder
                tok.consumeToEOL();
                break;

            case 'v':
                switch (tmp[1]) {
                    case '\0':
                        //vertex, 3 or 4 components
                        val[3] = 1.0f;  //default w coordinate
                        match = tok.getTokenFloatArray(val, 4);
                        _positions.push_back( val[0]);
                        _positions.push_back( val[1]);
                        _positions.push_back( val[2]);
                        _positions.push_back( val[3]);
                        vtx4Comp |= ( match == 4);
                        NV_ASSERT( match > 2 && match < 5);
                        break;

                    case 'n':
                        //normal, 3 components
                        match = tok.getTokenFloatArray(val, 3);
                        _normals.push_back( val[0]);
                        _normals.push_back( val[1]);
                        _normals.push_back( val[2]);
                        NV_ASSERT( match == 3);
                        break;

                    case 't':
                        //texcoord, 2 or 3 components
                        val[2] = 0.0f;  //default r coordinate
                        match = tok.getTokenFloatArray(val, 3);
                        _texCoords.push_back( val[0]);
                        _texCoords.push_back( val[1]);
                        _texCoords.push_back( val[2]);
                        tex3Comp |= ( match == 3);
                        NV_ASSERT( match > 1 && match < 4);
                        break;
                    case 'p':
                        // Parameter space vertices not supported...
                        tok.consumeToEOL();
                        break;
                }
                break;

            case 'f':
            {
                //face
                // determine the type, and read the initial vertex, all entries in a face must have the same format
                // formats are:
                // 1  #
                // 2  #/#
                // 3  #/#/#
                // 4  #//#

                // we need to 'hand read' the first run, to decode the formatting.
                format = 0;
                if (!tok.getTokenInt(idx[0][0])) {
                    NV_ASSERT(0);
                    return false;
                }
                // on our way.
                format = 1;
                if (tok.consumeOneDelim()) {
                    if (tok.consumeOneDelim()) {
                        // automatically format 4.
                        format = 4;
                    }
                    if (!tok.getTokenInt(idx[0][1])) {
                        NV_ASSERT(0);
                        return false;
                    }

                    if (format != 4)
                    {
                        format = 2; // at least format 2.
                        tok.setConsumeWS(false);
                        if (tok.consumeOneDelim()) {
                            if (tok.getTokenInt(idx[0][2])) {
                                // automatically format 3
                                format = 3;
                            }
                            // else remain format 2, in case of "#/#/" wacky format.
                        }
                        tok.setConsumeWS(true);
                    }
                }

                switch (format) {
                    case 1: // #
                    { //This face has only vertex indices
                        //remap them to the right spot
                        idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int32_t)_positions.size() - idx[0][0]);

                        //grab the second vertex to prime
                        tok.getTokenInt(idx[1][0]);

                        //remap them to the right spot
                        idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int32_t)_positions.size() - idx[1][0]);

                        while ( tok.getTokenInt(idx[2][0]) ) {
                            //remap them to the right spot
                            idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int32_t)_positions.size() - idx[2][0]);

                            //add the indices
                            for (int32_t ii = 0; ii < 3; ii++) {
                                _pIndex.push_back( idx[ii][0]);
                                _tIndex.push_back( 0); //dummy index to keep things in synch
                                _nIndex.push_back( 0); //dummy normal index to keep everything in synch
                            }

                            //prepare for the next iteration
                            idx[1][0] = idx[2][0];
                        }

                        break;
                    }

                    case 2: // #/#
                    { //This face has vertex and texture coordinate indices
                        //remap them to the right spot
                        idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int32_t)_positions.size() - idx[0][0]);
                        idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int32_t)_texCoords.size() - idx[0][1]);

                        //grab the second vertex to prime
                        tok.getTokenIntArray(idx[1], 2);

                        //remap them to the right spot
                        idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int32_t)_positions.size() - idx[1][0]);
                        idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int32_t)_texCoords.size() - idx[1][1]);

                        while ( tok.getTokenIntArray(idx[2], 2) == 2) {
                            //remap them to the right spot
                            idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int32_t)_positions.size() - idx[2][0]);
                            idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int32_t)_texCoords.size() - idx[2][1]);

                            //add the indices
                            for (int32_t ii = 0; ii < 3; ii++) {
                                _pIndex.push_back( idx[ii][0]);
                                _tIndex.push_back( idx[ii][1]);
                                _nIndex.push_back( 0); //dummy normal index to keep everything in synch
                            }

                            //prepare for the next iteration
                            idx[1][0] = idx[2][0];
                            idx[1][1] = idx[2][1];
                        }

                        hasTC = true;
                        break;
                    }

                    case 3: // #/#/#
                    { //This face has vertex, texture coordinate, and normal indices
                        //remap them to the right spot
                        idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int32_t)_positions.size() - idx[0][0]);
                        idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int32_t)_texCoords.size() - idx[0][1]);
                        idx[0][2] = (idx[0][2] > 0) ? (idx[0][2] - 1) : ((int32_t)_normals.size() - idx[0][2]);

                        //grab the second vertex to prime
                        tok.getTokenIntArray(idx[1], 3);

                        //remap them to the right spot
                        idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int32_t)_positions.size() - idx[1][0]);
                        idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int32_t)_texCoords.size() - idx[1][1]);
                        idx[1][2] = (idx[1][2] > 0) ? (idx[1][2] - 1) : ((int32_t)_normals.size() - idx[1][2]);

                        //create the fan
                        while ( tok.getTokenIntArray(idx[2], 3) == 3) {
                            //remap them to the right spot
                            idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int32_t)_positions.size() - idx[2][0]);
                            idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int32_t)_texCoords.size() - idx[2][1]);
                            idx[2][2] = (idx[2][2] > 0) ? (idx[2][2] - 1) : ((int32_t)_normals.size() - idx[2][2]);

                            //add the indices
                            for (int32_t ii = 0; ii < 3; ii++) {
                                _pIndex.push_back( idx[ii][0]);
                                _tIndex.push_back( idx[ii][1]);
                                _nIndex.push_back( idx[ii][2]);
                            }

                            //prepare for the next iteration
                            idx[1][0] = idx[2][0];
                            idx[1][1] = idx[2][1];
                            idx[1][2] = idx[2][2];
                        }

                        hasTC = true;
                        hasNormals = true;
                        break;
                    }

                    case 4: // #//#
                    { //This face has vertex and normal indices
                        //remap them to the right spot
                        idx[0][0] = (idx[0][0] > 0) ? (idx[0][0] - 1) : ((int32_t)_positions.size() - idx[0][0]);
                        idx[0][1] = (idx[0][1] > 0) ? (idx[0][1] - 1) : ((int32_t)_normals.size() - idx[0][1]);

                        //grab the second vertex to prime
                        tok.getTokenIntArray(idx[1], 2);

                        //remap them to the right spot
                        idx[1][0] = (idx[1][0] > 0) ? (idx[1][0] - 1) : ((int32_t)_positions.size() - idx[1][0]);
                        idx[1][1] = (idx[1][1] > 0) ? (idx[1][1] - 1) : ((int32_t)_normals.size() - idx[1][1]);

                        //create the fan
                        while ( tok.getTokenIntArray(idx[2], 2) == 2) {
                            //remap them to the right spot
                            idx[2][0] = (idx[2][0] > 0) ? (idx[2][0] - 1) : ((int32_t)_positions.size() - idx[2][0]);
                            idx[2][1] = (idx[2][1] > 0) ? (idx[2][1] - 1) : ((int32_t)_normals.size() - idx[2][1]);

                            //add the indices
                            for (int32_t ii = 0; ii < 3; ii++) {
                                _pIndex.push_back( idx[ii][0]);
                                _nIndex.push_back( idx[ii][1]);
                                _tIndex.push_back(0); // dummy index, to ensure that the buffers are of identical size
                            }

                            //prepare for the next iteration
                            idx[1][0] = idx[2][0];
                            idx[1][1] = idx[2][1];
                        }

                        hasNormals = true;
                        break;
                    }

                    default:
                        NV_ASSERT(0);
                        return false;
                }

            }
            break;

            case 's':
            case 'g':
            case 'u':
                //all presently ignored
            default:
                tok.consumeToEOL();
        };
    }

    //post-process data

    //free anything that ended up being unused
    if (!hasNormals) {
        _normals.clear();
        _nIndex.clear();
    }

    if (!hasTC) {
        _texCoords.clear();
        _tIndex.clear();
    }

    //set the defaults as the worst-case for an obj file
    _posSize = 4;
    _tcSize = 3;

    //compact to 3 component vertices if possible
    if (!vtx4Comp) {
        vector<float>::iterator src = _positions.begin();
        vector<float>::iterator dst = _positions.begin();

        for ( ; src < _positions.end(); ) {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src++;
        }

        _positions.resize( (_positions.size() / 4) * 3);
		
		_posSize = 3;
    }

    //compact to 2 component tex coords if possible
    if (!tex3Comp) {
        vector<float>::iterator src = _texCoords.begin();
        vector<float>::iterator dst = _texCoords.begin();

        for ( ; src < _texCoords.end(); ) {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src++;
        }

        _texCoords.resize( (_texCoords.size() / 3) * 2);

        _tcSize = 2;
    }

    return true;
}

//
// compile the model to an acceptable format
//////////////////////////////////////////////////////////////////////
void NvModelObj::compileModel(NvModelPrimType::Enum prim) {
	bool needsTriangles = false;
	bool needsTrianglesWithAdj = false;
	bool needsEdges = false;
	bool needsPoints = false;

	if ((prim & NvModelPrimType::POINTS) == NvModelPrimType::POINTS)
		needsPoints = true;

	if ((prim & NvModelPrimType::TRIANGLES) == NvModelPrimType::TRIANGLES)
		needsTriangles = true;

	if ((prim & NvModelPrimType::TRIANGLES_WITH_ADJACENCY) == NvModelPrimType::TRIANGLES_WITH_ADJACENCY) {
		needsTriangles = true;
		needsTrianglesWithAdj = true;
	}

	if ((prim & NvModelPrimType::EDGES) == NvModelPrimType::EDGES) {
		needsTriangles = true;
		needsEdges = true;
	}


	//merge the points
	map<IdxSet, uint32_t> pts;

	//find whether a position is unique
	set<uint32_t> ptSet;

	vector<uint32_t> indices[4];
	vector<float> vertices;

	{
		vector<uint32_t>::iterator pit = _pIndex.begin();
		vector<uint32_t>::iterator nit = _nIndex.begin();
		vector<uint32_t>::iterator tit = _tIndex.begin();
		vector<uint32_t>::iterator tanit = _tanIndex.begin();
		vector<uint32_t>::iterator cit = _cIndex.begin();

		while (pit < _pIndex.end()) {
			IdxSet idx;
			idx.pIndex = *pit;

			if (_normals.size() > 0)
				idx.nIndex = *nit;
			else
				idx.nIndex = 0;

			if (_tIndex.size() > 0)
				idx.tIndex = *tit;
			else
				idx.tIndex = 0;

			if (_tanIndex.size() > 0)
				idx.tanIndex = *tanit;
			else
				idx.tanIndex = 0;

			if (_cIndex.size() > 0)
				idx.cIndex = *cit;
			else
				idx.cIndex = 0;

			map<IdxSet, uint32_t>::iterator mit = pts.find(idx);

			if (mit == pts.end()) {

				if (needsTriangles)
					indices[2].push_back((uint32_t)pts.size());

				//since this one is a new vertex, check to see if this position is already referenced
				if (needsPoints && ptSet.find(idx.pIndex) != ptSet.end()) {
					ptSet.insert(idx.pIndex);
				}

				pts.insert(map<IdxSet, uint32_t>::value_type(idx, (uint32_t)pts.size()));

				//position
				vertices.push_back(_positions[idx.pIndex*_posSize]);
				vertices.push_back(_positions[idx.pIndex*_posSize + 1]);
				vertices.push_back(_positions[idx.pIndex*_posSize + 2]);
				if (_posSize == 4)
					vertices.push_back(_positions[idx.pIndex*_posSize + 3]);

				//normal
				if (_normals.size() > 0) {
					vertices.push_back(_normals[idx.nIndex * 3]);
					vertices.push_back(_normals[idx.nIndex * 3 + 1]);
					vertices.push_back(_normals[idx.nIndex * 3 + 2]);
				}

				//texture coordinate
				if (_texCoords.size() > 0) {
					vertices.push_back(_texCoords[idx.tIndex*_tcSize]);
					vertices.push_back(_texCoords[idx.tIndex*_tcSize + 1]);
					if (_tcSize == 3)
						vertices.push_back(_texCoords[idx.tIndex*_tcSize + 2]);
				}

				//tangents
				if (_sTangents.size() > 0) {
					vertices.push_back(_sTangents[idx.tanIndex * 3]);
					vertices.push_back(_sTangents[idx.tanIndex * 3 + 1]);
					vertices.push_back(_sTangents[idx.tanIndex * 3 + 2]);
				}

				//colors
				if (_colors.size() > 0) {
					vertices.push_back(_colors[idx.cIndex*_cSize]);
					vertices.push_back(_colors[idx.cIndex*_cSize + 1]);
					vertices.push_back(_colors[idx.cIndex*_cSize + 2]);
					if (_cSize == 4)
						vertices.push_back(_colors[idx.cIndex*_cSize + 3]);
				}
			}
			else {
				if (needsTriangles)
					indices[2].push_back(mit->second);
			}

			//advance the iterators if the components are present
			pit++;

			if (hasNormals())
				nit++;

			if (hasTexCoords())
				tit++;

			if (hasTangents())
				tanit++;

			if (hasColors())
				cit++;
		}
	}

	//create an edge list, if necessary
	if (needsEdges || needsTrianglesWithAdj) {
		std::multimap<Edge, uint32_t> edges;


		//edges are only based on positions only
		for (int32_t ii = 0; ii < (int32_t)_pIndex.size(); ii += 3) {
			for (int32_t jj = 0; jj < 3; jj++) {
				Edge w(_pIndex[ii + jj], _pIndex[ii + (jj + 1) % 3]);
				std::multimap<Edge, uint32_t>::iterator it = edges.find(w);

				//if we are storing edges, make sure we store only one copy
				if (needsEdges && it == edges.end()) {
					indices[1].push_back(indices[2][ii + jj]);
					indices[1].push_back(indices[2][ii + (jj + 1) % 3]);
				}
				edges.insert(std::multimap<Edge, uint32_t>::value_type(w, ii / 3));
			}
		}


		//now handle triangles with adjacency
		if (needsTrianglesWithAdj) {
			for (int32_t ii = 0; ii < (int32_t)_pIndex.size(); ii += 3) {
				for (int32_t jj = 0; jj < 3; jj++) {
					Edge w(_pIndex[ii + jj], _pIndex[ii + (jj + 1) % 3]);
					std::multimap<Edge, uint32_t>::iterator it = edges.lower_bound(w);
					std::multimap<Edge, uint32_t>::iterator limit = edges.upper_bound(w);
					uint32_t adjVertex = 0;

					while (it != edges.end() && it->second == (uint32_t)(ii / 3) && it != limit)
						it++;

					if (it == edges.end() || it == limit || it->first.pIndex[0] != w.pIndex[0] || it->first.pIndex[1] != w.pIndex[1]) {
						//no adjacent triangle found, duplicate the vertex
						adjVertex = indices[2][ii + jj];
					}
					else {
						uint32_t triOffset = it->second * 3; //compute the starting index of the triangle
						adjVertex = indices[2][triOffset]; //set the vertex to a default, in case the adjacent triangle it a degenerate

						//find the unshared vertex
						for (int32_t kk = 0; kk<3; kk++) {
							if (_pIndex[triOffset + kk] != w.pIndex[0] && _pIndex[triOffset + kk] != w.pIndex[1]) {
								adjVertex = indices[2][triOffset + kk];
								break;
							}
						}
					}

					//store the vertices for this edge
					indices[3].push_back(indices[2][ii + jj]);
					indices[3].push_back(adjVertex);
				}
			}
		}

	}

	//create selected prim

	//set the offsets and vertex size
	_pOffset = 0; //always first
	_vtxSize = _posSize;
	if (hasNormals()) {
		_nOffset = _vtxSize;
		_vtxSize += 3;
	}
	else {
		_nOffset = -1;
	}
	if (hasTexCoords()) {
		_tcOffset = _vtxSize;
		_vtxSize += _tcSize;
	}
	else {
		_tcOffset = -1;
	}
	if (hasTangents()) {
		_sTanOffset = _vtxSize;
		_vtxSize += 3;
	}
	else {
		_sTanOffset = -1;
	}
	if (hasColors()) {
		_cOffset = _vtxSize;
		_vtxSize += _cSize;
	}
	else {
		_cOffset = -1;
	}

	m_minExtent = nv::vec3f(1e10f, 1e10f, 1e10f);
	m_maxExtent = -m_minExtent;

	for (vector<float>::iterator pit = _positions.begin() + _posSize; pit < _positions.end(); pit += _posSize) {
		m_minExtent = nv::min(m_minExtent, nv::vec3f(&pit[0]));
		m_maxExtent = nv::max(m_maxExtent, nv::vec3f(&pit[0]));
	}

	// allocate and copy vectors to arrays
	_vertexCount = vertices.size() / _vtxSize;
	_indexCount = indices[2].size();

	{
		_indices = new uint32_t[indices[2].size()];
		for (uint32_t i = 0; i < indices[2].size(); i++)
			_indices[i] = indices[2][i];

		for (uint32_t j = 0; j < 4; j++)
			indices[j].clear();
	}

	{
		_vertices = new float[vertices.size()];
		for (uint32_t i = 0; i < vertices.size(); i++)
			_vertices[i] = vertices[i];
		vertices.clear();
	}

}

//
// compute tangents in the S direction
//
//////////////////////////////////////////////////////////////////////
void NvModelObj::computeTangents() {

	//make sure tangents don't already exist
	if (hasTangents())
		return;

	//make sure that the model has texcoords
	if (!hasTexCoords())
		return;

	//alloc memory and initialize to 0
	_tanIndex.reserve(_pIndex.size());
	_sTangents.resize((_texCoords.size() / _tcSize) * 3, 0.0f);

	// the collision map records any alternate locations for the tangents
	std::multimap< uint32_t, uint32_t> collisionMap;

	//process each face, compute the tangent and try to add it
	for (int32_t ii = 0; ii < (int32_t)_pIndex.size(); ii += 3) {
		vec3f p0(&_positions[_pIndex[ii] * _posSize]);
		vec3f p1(&_positions[_pIndex[ii + 1] * _posSize]);
		vec3f p2(&_positions[_pIndex[ii + 2] * _posSize]);
		vec2f st0(&_texCoords[_tIndex[ii] * _tcSize]);
		vec2f st1(&_texCoords[_tIndex[ii + 1] * _tcSize]);
		vec2f st2(&_texCoords[_tIndex[ii + 2] * _tcSize]);

		//compute the edge and tc differentials
		vec3f dp0 = p1 - p0;
		vec3f dp1 = p2 - p0;
		vec2f dst0 = st1 - st0;
		vec2f dst1 = st2 - st0;

		float factor = 1.0f / (dst0[0] * dst1[1] - dst1[0] * dst0[1]);

		//compute sTangent
		vec3f sTan;
		sTan[0] = dp0[0] * dst1[1] - dp1[0] * dst0[1];
		sTan[1] = dp0[1] * dst1[1] - dp1[1] * dst0[1];
		sTan[2] = dp0[2] * dst1[1] - dp1[2] * dst0[1];
		sTan *= factor;

		//should this really renormalize?
		sTan = normalize(sTan);

		//loop over the vertices, to update the tangents
		for (int32_t jj = 0; jj < 3; jj++) {
			//get the present accumulated tangnet
			vec3f curTan(&_sTangents[_tIndex[ii + jj] * 3]);

			//check to see if it is uninitialized, if so, insert it
			if (curTan[0] == 0.0f && curTan[1] == 0.0f && curTan[2] == 0.0f) {
				_sTangents[_tIndex[ii + jj] * 3] = sTan[0];
				_sTangents[_tIndex[ii + jj] * 3 + 1] = sTan[1];
				_sTangents[_tIndex[ii + jj] * 3 + 2] = sTan[2];
				_tanIndex.push_back(_tIndex[ii + jj]);
			}
			else {
				//check for agreement
				curTan = normalize(curTan);

				if (dot(curTan, sTan) >= cosf(3.1415926f * 0.333333f)) {
					//tangents are in agreement
					_sTangents[_tIndex[ii + jj] * 3] += sTan[0];
					_sTangents[_tIndex[ii + jj] * 3 + 1] += sTan[1];
					_sTangents[_tIndex[ii + jj] * 3 + 2] += sTan[2];
					_tanIndex.push_back(_tIndex[ii + jj]);
				}
				else {
					//tangents disagree, this vertex must be split in tangent space 
					std::multimap< uint32_t, uint32_t>::iterator it = collisionMap.find(_tIndex[ii + jj]);

					//loop through all hits on this index, until one agrees
					while (it != collisionMap.end() && it->first == _tIndex[ii + jj]) {
						curTan = vec3f(&_sTangents[it->second * 3]);

						curTan = normalize(curTan);
						if (dot(curTan, sTan) >= cosf(3.1415926f * 0.333333f))
							break;

						it++;
					}

					//check for agreement with an earlier collision
					if (it != collisionMap.end() && it->first == _tIndex[ii + jj]) {
						//found agreement with an earlier collision, use that one
						_sTangents[it->second * 3] += sTan[0];
						_sTangents[it->second * 3 + 1] += sTan[1];
						_sTangents[it->second * 3 + 2] += sTan[2];
						_tanIndex.push_back(it->second);
					}
					else {
						//we have a new collision, create a new tangent
						uint32_t target = (uint32_t)_sTangents.size() / 3;
						_sTangents.push_back(sTan[0]);
						_sTangents.push_back(sTan[1]);
						_sTangents.push_back(sTan[2]);
						_tanIndex.push_back(target);
						collisionMap.insert(std::multimap< uint32_t, uint32_t>::value_type(_tIndex[ii + jj], target));
					}
				} // else ( if tangent agrees)
			} // else ( if tangent is uninitialized )
		} // for jj = 0 to 2 ( iteration of triangle verts)
	} // for ii = 0 to numFaces *3 ( iterations over triangle faces

	//normalize all the tangents
	for (int32_t ii = 0; ii < (int32_t)_sTangents.size(); ii += 3) {
		vec3f tan(&_sTangents[ii]);
		tan = normalize(tan);
		_sTangents[ii] = tan[0];
		_sTangents[ii + 1] = tan[1];
		_sTangents[ii + 2] = tan[2];
	}
}
//
//compute vertex normals
//////////////////////////////////////////////////////////////////////
void NvModelObj::computeNormals() {

	// don't recompute normals
	if (hasNormals())
		return;

	//allocate and initialize the normal values
	_normals.resize((_positions.size() / _posSize) * 3, 0.0f);
	_nIndex.reserve(_pIndex.size());

	// the collision map records any alternate locations for the normals
	std::multimap< uint32_t, uint32_t> collisionMap;

	//iterate over the faces, computing the face normal and summing it them
	for (int32_t ii = 0; ii < (int32_t)_pIndex.size(); ii += 3) {
		vec3f p0(&_positions[_pIndex[ii] * _posSize]);
		vec3f p1(&_positions[_pIndex[ii + 1] * _posSize]);
		vec3f p2(&_positions[_pIndex[ii + 2] * _posSize]);

		//compute the edge vectors
		vec3f dp0 = p1 - p0;
		vec3f dp1 = p2 - p0;

		vec3f fNormal = cross(dp0, dp1); // compute the face normal
		vec3f nNormal = normalize(fNormal);  // compute a normalized normal

		//iterate over the vertices, adding the face normal influence to each
		for (int32_t jj = 0; jj < 3; jj++) {
			// get the current normal from the default location (index shared with position) 
			vec3f cNormal(&_normals[_pIndex[ii + jj] * 3]);

			// check to see if this normal has not yet been touched 
			if (cNormal[0] == 0.0f && cNormal[1] == 0.0f && cNormal[2] == 0.0f) {
				// first instance of this index, just store it as is
				_normals[_pIndex[ii + jj] * 3] = fNormal[0];
				_normals[_pIndex[ii + jj] * 3 + 1] = fNormal[1];
				_normals[_pIndex[ii + jj] * 3 + 2] = fNormal[2];
				_nIndex.push_back(_pIndex[ii + jj]);
			}
			else {
				// check for agreement
				cNormal = normalize(cNormal);

				if (dot(cNormal, nNormal) >= cosf(3.1415926f * 0.333333f)) {
					//normal agrees, so add it
					_normals[_pIndex[ii + jj] * 3] += fNormal[0];
					_normals[_pIndex[ii + jj] * 3 + 1] += fNormal[1];
					_normals[_pIndex[ii + jj] * 3 + 2] += fNormal[2];
					_nIndex.push_back(_pIndex[ii + jj]);
				}
				else {
					//normals disagree, this vertex must be along a facet edge 
					std::multimap< uint32_t, uint32_t>::iterator it = collisionMap.find(_pIndex[ii + jj]);

					//loop through all hits on this index, until one agrees
					while (it != collisionMap.end() && it->first == _pIndex[ii + jj]) {
						cNormal = normalize(vec3f(&_normals[it->second * 3]));

						if (dot(cNormal, nNormal) >= cosf(3.1415926f * 0.333333f))
							break;

						it++;
					}

					//check for agreement with an earlier collision
					if (it != collisionMap.end() && it->first == _pIndex[ii + jj]) {
						//found agreement with an earlier collision, use that one
						_normals[it->second * 3] += fNormal[0];
						_normals[it->second * 3 + 1] += fNormal[1];
						_normals[it->second * 3 + 2] += fNormal[2];
						_nIndex.push_back(it->second);
					}
					else {
						//we have a new collision, create a new normal
						uint32_t target = (uint32_t)_normals.size() / 3;
						_normals.push_back(fNormal[0]);
						_normals.push_back(fNormal[1]);
						_normals.push_back(fNormal[2]);
						_nIndex.push_back(target);
						collisionMap.insert(std::multimap< uint32_t, uint32_t>::value_type(_pIndex[ii + jj], target));
					}
				} // else ( if normal agrees)
			} // else (if normal is uninitialized)
		} // for each vertex in triangle
	} // for each face

	//now normalize all the normals
	for (int32_t ii = 0; ii < (int32_t)_normals.size(); ii += 3) {
		vec3f norm(&_normals[ii]);
		norm = normalize(norm);
		_normals[ii] = norm[0];
		_normals[ii + 1] = norm[1];
		_normals[ii + 2] = norm[2];
	}

}

//
//
//////////////////////////////////////////////////////////////////////
void NvModelObj::computeBoundingBox(nv::vec3f &minVal, nv::vec3f &maxVal) {

	if (_positions.empty())
		return;

	minVal = nv::vec3f(1e10f, 1e10f, 1e10f);
	maxVal = -minVal;

	for (vector<float>::iterator pit = _positions.begin() + _posSize; pit < _positions.end(); pit += _posSize) {
		minVal = nv::min(minVal, nv::vec3f(&pit[0]));
		maxVal = nv::max(maxVal, nv::vec3f(&pit[0]));
	}
}

//
//
//////////////////////////////////////////////////////////////////////
void NvModelObj::rescale(float radius) {

	if (_positions.empty())
		return;

	if (radius < 0.0f)
		return;

	vec3f minVal, maxVal;
	computeBoundingBox(minVal, maxVal);

	vec3f r = 0.5f*(maxVal - minVal);
	vec3f center = minVal + r;
	//    float oldRadius = length(r);
	float oldRadius = std::max(r.x, std::max(r.y, r.z));
	float scale = radius / oldRadius;

	for (vector<float>::iterator pit = _positions.begin(); pit < _positions.end(); pit += _posSize) {
		vec3f np = scale*(vec3f(&pit[0]) - center);
		//vec3f np = scale*(vec3f(&pit[0]));
		pit[0] = np.x;
		pit[1] = np.y;
		pit[2] = np.z;
	}
}

//
//
//////////////////////////////////////////////////////////////////////
void NvModelObj::removeDegeneratePrims() {
	uint32_t *pSrc = 0, *pDst = 0, *tSrc = 0, *tDst = 0, *nSrc = 0, *nDst = 0, *cSrc = 0, *cDst = 0;
	int32_t degen = 0;

	pSrc = &_pIndex[0];
	pDst = pSrc;

	if (hasTexCoords()) {
		tSrc = &_tIndex[0];
		tDst = tSrc;
	}

	if (hasNormals()) {
		nSrc = &_nIndex[0];
		nDst = nSrc;
	}

	if (hasColors()) {
		cSrc = &_cIndex[0];
		cDst = cSrc;
	}

	for (int32_t ii = 0; ii < (int32_t)_pIndex.size(); ii += 3, pSrc += 3, tSrc += 3, nSrc += 3, cSrc += 3) {
		if (pSrc[0] == pSrc[1] || pSrc[0] == pSrc[2] || pSrc[1] == pSrc[2]) {
			degen++;
			continue; //skip updating the dest
		}

		for (int32_t jj = 0; jj < 3; jj++) {
			*pDst++ = pSrc[jj];

			if (hasTexCoords())
				*tDst++ = tSrc[jj];

			if (hasNormals())
				*nDst++ = nSrc[jj];

			if (hasColors())
				*cDst++ = cSrc[jj];
		}
	}

	_pIndex.resize(_pIndex.size() - degen * 3);

	if (hasTexCoords())
		_tIndex.resize(_tIndex.size() - degen * 3);

	if (hasNormals())
		_nIndex.resize(_nIndex.size() - degen * 3);

	if (hasColors())
		_cIndex.resize(_cIndex.size() - degen * 3);

}
