//----------------------------------------------------------------------------------
// File:        NvModel/NvModelVectorCompactor.h
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
#ifndef _NVMODELVECTORCOMPACTOR_H_
#define _NVMODELVECTORCOMPACTOR_H_
#pragma once

#include <NvAssert.h>
#include <NV/NvLogs.h>
#include <vector>
#include <map>

namespace Nv
{
    // VectorCompactor is a class to contain a set of vectors, which it optimizes by 
    // removing vectors that are duplicates of each other, within a given tolerance.
    // It also maintains a mapping of the original vector's index, which is determined
    // by the order in which it was added to the compactor, to its position in the 
    // compacted set.  The container may be given a comparator object to control
    // the methods used to compare the vectors for merging.  
    template <class T, typename Cmp >
    class VectorCompactor
    {
    public:
        typedef typename std::vector<T> VecArray;

        VectorCompactor(float epsilon, uint32_t reserveSize, Cmp comp = Cmp()) :
            m_mappedIndices(0, -1),
            m_epsilon(epsilon),
            m_comp(comp)
        {
            Reserve(reserveSize);
        }

        // Clear out the data from the container so that it may be reused
        void Clear()
        {
            m_vecs.resize(0);
            m_mappedIndices.resize(0);
            m_vecMap.clear();
        }

        // Reserve an initial size for the container and its underlying structures.
        // It will grow as needed, but providing a reasonable starting size will
        // reduce the number of re-allocations and copies.
        /// \param[in] size Number of elements to be resize the container to hold
        void Reserve(uint32_t size)
        {
            m_vecs.reserve(size);
            m_mappedIndices.reserve(size);
        }

        // Adds a vector to the container.  The vector's original index will be 
        // equal to the number of vectors already added.  The returned index will
        // be the vector's position in the compacted set of vectors.
        // \param[in] v Vector to add to the container
        // \return Index into the compacted set of vectors tha matches the added vector
        int32_t Append(const T& v)
        {
            int32_t index = FindOrAddObject(v);
            NV_ASSERT(-1 != index);
            m_mappedIndices.push_back(index);
            return index;
        }

        // Returns the vector stored at the given index in the compacted set of vectors
        // \param[in] id Index in the compacted set of vectors that holds the requested vector
        // \param[out] out Object in which to store the retrieved vector
        // \return True if a vector existed at the provided index and was retrieved into
        //         the 'out' parameter.  False if no vector existed with the given index.
        bool GetObject(int32_t id, T& out)
        {
            if ((id < 0) || (id >= (int32_t)(m_vecs.size())))
            {
                return false;
            }
            out = m_vecs[id];
            return true;
        }

        // Returns the index, into the array of compacted vectors, that holds the vector
        // inserted with the given, original index.
        // \param[in] index Original index of the vector
        // \return Matching index in the compacted set of vectors that was used to store the vector
        int32_t Remap(int32_t index)
        {
            if ((index < 0) || (index >= (int32_t)(m_mappedIndices.size())))
            {
                NV_ASSERT(0);
                return -1;
            }
            NV_ASSERT(m_mappedIndices[index] != -1);
            return m_mappedIndices[index];
        }

        // Returns the set of compacted vectors
        const VecArray& GetVectors() { return m_vecs; }

        // Returns the number of vectors in the compacted set of vectors
        uint32_t GetVectorCount() { return (uint32_t)(m_vecs.size()); }

        // Note that the key can not be modified once added, so doing this rescale
        // causes the keys to be out of sync with the vectors themselves.  No new
        // vectors should be added after this method is called, or they may be
        // sorted incorrectly.
        // \param[in] scale Scale factor to multiply all vectors by
        // \param[in] center Object space point about which center the vectors before scaling them
        void RescaleToOrigin(float scale, const T& center)
        {
            typename std::vector<T>::iterator it = m_vecs.begin();
            typename std::vector<T>::const_iterator itEnd = m_vecs.end();

            while (it != itEnd)
            {
                (*it) = scale * ((*it) - center);
                ++it;
            }
        }

    protected:
        // Structure used to order vectors added to the container.  We use a one-dimensional
        // container to hold the vectors, so simply sort them by a single coordinate.
        struct VecCompare
        {
            bool operator()(const T& lhs, const std::pair<T, int32_t>& rhs) const
            {
                return lhs[0] < rhs.first[0];
            }

            bool operator()(const T& lhs, const T&rhs) const
            {
                return lhs[0] < rhs[0];
            }
        };

        // Useful typedefs for internal containers and associated objects
        typedef typename std::pair<T, int32_t> VecMapPair;
        typedef typename std::multimap<T, int32_t, VecCompare> VecMap;

        // Finds an existing vector in the set of compacted vectors that is
        // close enough to the given vector to merge and returns the index
        // of that vector.  If none is within the current tolerance, the new
        // vector will be added and its new index returned.  Uses the 
        // specialized Diff() and ShouldMerge() methods to determine redundancy.
        // \param[in] v Vector to be added to the container
        // \return The index of the vector in the compacted set
        int32_t FindOrAddObject(const T& v)
        {
            // If we have nothing yet, then we simply add this vector to the set
            if (m_vecs.empty())
            {
                return Add(v, m_vecMap.begin());
            }

            // We'll need to find the subset of existing vectors that are potential candidates
            // for welding by finding all within the epsilon of the given vector.  We
            // will use a vector whose components are all epsilon to add and subtract
            // from the given vector to define the extents of our search.  Since we don't
            // know how many components our contained vectors will have, we will use a
            // vec4f to initialize it, as all vectors can be initialized from one.
            T epsilonVec = T(nv::vec4f(m_epsilon, m_epsilon, m_epsilon, m_epsilon));

            // Find the first vector whose x is greater than our minimum
            T minVec = v - epsilonVec;
            typename VecMap::iterator rangeStart = m_vecMap.lower_bound(minVec);

            if (rangeStart == m_vecMap.end())
            {
                // All existing positions are "below" our minimum point, so insert a 
                // new vector at the end of the map
                return Add(v, --rangeStart);
            }

            // Find the first vector whose x is greater than our maximum, thus out of our 
            // range of candidates
            T maxVec = v + epsilonVec;
            typename VecMap::iterator rangeEnd = m_vecMap.upper_bound(maxVec);

            if (rangeStart == rangeEnd)
            {
                // No positions are in the range we're looking for, so insert one 
                // at the closest point in the map
                return Add(v, --rangeStart);
            }

            // Use the first candidate within our possible range to initialize 
            // our search as the closest candidate
            typename VecMap::iterator closestPosition = rangeStart;
            typename VecMap::iterator currentIt = rangeStart;
            float bestDist2 = m_comp.Diff(v, rangeStart->first);

            // Check all the other vectors in the range for a closer fit
            for (++currentIt; currentIt != rangeEnd; ++currentIt)
            {
                float dist2 = m_comp.Diff(v, currentIt->first);
                if (dist2 < bestDist2)
                {
                    bestDist2 = dist2;
                    closestPosition = currentIt;
                }
            }

            // Once we have our closest match, see if it's close enough
            if (!m_comp.ShouldMerge(bestDist2, m_epsilon))
            {
                // It wasn't, so we need to add the given vector as a new one
                return Add(v, rangeStart);
            }

            // We found a suitable vector for re-use
            return closestPosition->second;
        }

        // Adds the given vector to the compacted set, using the given hint
        // to find the right spot
        int32_t Add(const T& v, const typename VecMap::iterator& hint)
        {
            // The remapped index will be the current number of vectors in the compacted set
            int32_t index = int32_t(m_vecs.size());
            m_vecs.push_back(v);

            // Add a map entry for the given vector to our vector map, pointing it towards
            // the proper location in the compacted set
            m_vecMap.insert(hint, VecMapPair(v, index));

            // Return the new, compacted set index to the caller to be added to the remapping vector
            return index;
        }

        Cmp m_comp;         // The comparator object used to evaluate vectors for merging
        float m_epsilon;    // The tolerance to be used when evaluating vectors for merging

        VecArray m_vecs;    // The compacted set of vectors
        VecMap m_vecMap;    // The map of existing vectors to their index in the compacted set

        std::vector<int32_t> m_mappedIndices;   // One entry per vector added to the container, 
                                                // containing the index in the compacted set that 
                                                // holds the vector (or the one that it was merged
                                                // with), in order of their addition to the set
    };

    // Comparator objects to be used to allow the compactor to hold different types of vectors
    
    // Position Comparator
    // Compares vectors as positions, using the distance between the points as the difference to test
    template <class T>
    struct Difference_Position
    {
        float Diff(const T& v1, const T& v2)
        {
            return square_norm(v2 - v1);
        }

        bool ShouldMerge(float diff, float epsilon)
        {
            return diff < (epsilon * epsilon);
        }
    };

    // Normal Comparator
    // Compares vectors as normals, using the dot product between the vectors to determine
    // how closely aligned they are.
    template <class T>
    struct Difference_Normal
    {
        float Diff(const T& v1, const T& v2)
        {
            return 1.0f - dot(v1, v2);
        }

        bool ShouldMerge(float diff, float epsilon)
        {
            return (diff < epsilon);
        }
    };

    // Helper struct to simplify declarations of compactors for different vector usages
    template <class T> 
    struct NvModelVectorCompactor
    {
        typedef VectorCompactor < T, Difference_Position<T> > Positions;
        typedef VectorCompactor < T, Difference_Normal<T> > Normals;
    };
}
#endif
