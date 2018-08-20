//----------------------------------------------------------------------------------
// File:        NvModel/NvSkeleton.h
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
#ifndef _NVSKELETON_H_
#define _NVSKELETON_H_

#include "NV/NvMath.h"
#include <string>
#include <vector>

namespace Nv
{
    /// NvSkeletonNode holds the definition of a single
    /// node in an NvSkeleton.
    struct NvSkeletonNode
    {
        std::string m_name;
        int32_t m_parentNode;
        nv::matrix4f m_parentRelTransform;
        std::vector<int32_t> m_childNodes;
        std::vector<uint32_t> m_meshes;
    };

    ///
    /// NvSkeleton holds an array of nodes that comprise the hierarchy of 
    /// transformations for the model.  These nodes form the basis for 
    /// applying hierarchical transformations to meshes within a model as
    /// well as skinning those meshes.  An NvSkeleton is a tree where each
    /// node has at most one parent, and the index of a parent node MUST
    /// be less than that of all of its children.  This allows all transforms
    /// to be propagated through the tree with a single sequential traversal
    /// of the array and any operations performed on nodes in such a traversal
    /// to be performed on any child's parent before the child itself.  Note
    /// that a skeleton may contain more than one "tree" (i.e. multiple nodes
    /// with no parent node), but by definition, node 0 is the root of a tree. 
    /// 
    class NvSkeleton
    {
    public:
        /// Default constructor.  Creates skeleton with 0 bones.
        NvSkeleton() {}

        /// Constructor initializes skeleton with the given array of nodes
        /// \param pNodes Pointer to an array of NvSkeletonNodes that have
        ///               been initialized with data defining the skeletal
        ///               hierarchy
        /// \param numNodes Number of NvSkeletonNodes contained in the 
        ///                 array pointed to by pNodes
        NvSkeleton(const NvSkeletonNode* pNodes, uint32_t numNodes);

        /// Retrieves the number of nodes contained in the skeleton
        /// \return Number of nodes contained in the skeleton
        int32_t GetNumNodes() const { return m_nodes.size(); }

        /// Retrieves the index of the first node in the skeleton
        /// with the given name
        /// \param name Name of the node to find
        /// \return The index of the first node with a name matching that
        ///         provided, but -1 if no node contained a matching name
        /// \note There may be more than one node in the skeleton with
        ///       the given name.  The matching node with the lowest index
        ///       will be returned.
        int32_t GetNodeIndexByName(const std::string& name) const;

        /// Retrieves the first node in the skeleton with the given name
        /// \param name Name of the node to find
        /// \return A pointer to the node whose name matches that provided.
        ///         NULL if no node contained a matching name
        /// \note There may be more than one node in the skeleton with
        ///       the given name.  The matching node with the lowest index
        ///       will be returned.
        const NvSkeletonNode* GetNodeByName(const std::string& name) const;

        /// Retrieves the node in the node array at the given index
        /// \param index Index of the node to retrieve
        /// \return A pointer to the node at the index provided.
        ///         NULL if the index was not valid
        const NvSkeletonNode* GetNodeByIndex(uint32_t index) const;

        /// Retrieves a pointer to the array of matrices representing
        /// the current model-space transforms for the nodes of the skeleton
        /// \return A pointer to the array of current transform matrices,
        ///         NULL if there are no nodes, thus no current transforms.
        nv::matrix4f* GetTransforms();

        /// Retrieves a pointer to the matrix representing the current
        /// model-space transform of the node at the given index.
        /// \param index Index of the node in the skeleton for which to
        ///              retrieve a current transform
        /// \return A pointer to the matrix representing the current transform
        ///         of the node at the given index, but NULL if the index
        ///         was invalid.
        nv::matrix4f* GetTransform(uint32_t index);

    protected:
        // Convenience typedefs
        typedef std::vector<NvSkeletonNode> NodeArray;
        typedef std::vector<nv::matrix4f> NodeTransformArray;

        // All nodes in the skeleton
        NodeArray m_nodes;  

        // Matrices containing the current, model-space 
        // transforms for each corresponding node
        NodeTransformArray m_nodeTransforms;  
    };
}

#endif // _NVSkELETON_H_
