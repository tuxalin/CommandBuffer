//----------------------------------------------------------------------------------
// File:        NvModel/NvSkeleton.cpp
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
#include "NvModel/NvSkeleton.h"

namespace Nv
{
    NvSkeleton::NvSkeleton(const NvSkeletonNode* pNodes, uint32_t numNodes)
    {
        m_nodes.resize(numNodes);
        m_nodeTransforms.resize(numNodes);
        const NvSkeletonNode* pSrcNode = pNodes;
        for (uint32_t nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex, ++pSrcNode)
        {
            m_nodes[nodeIndex] = *pSrcNode;
            NV_ASSERT(pSrcNode->m_parentNode < (int32_t)nodeIndex);
            if (-1 == pSrcNode->m_parentNode)
            {
                m_nodeTransforms[nodeIndex] = pSrcNode->m_parentRelTransform;
            }
            else
            {
                m_nodeTransforms[nodeIndex] = m_nodeTransforms[pSrcNode->m_parentNode] * pSrcNode->m_parentRelTransform;
            }
        }
    }

    int32_t NvSkeleton::GetNodeIndexByName(const std::string& name) const
    {
        uint32_t numNodes = m_nodes.size();
        for (uint32_t nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
        {
            if (m_nodes[nodeIndex].m_name == name)
            {
                return nodeIndex;
            }
        }
        return -1;
    }

    const NvSkeletonNode* NvSkeleton::GetNodeByName(const std::string& name) const
    {
        NodeArray::const_iterator nodeIt;
        NodeArray::const_iterator nodeEnd = m_nodes.end();
        for (nodeIt = m_nodes.begin(); nodeIt != nodeEnd; ++nodeIt)
        {
            if (nodeIt->m_name == name)
            {
                return &(*nodeIt);
            }
        }

        // No node with that name found
        return NULL;
    }

    const NvSkeletonNode* NvSkeleton::GetNodeByIndex(uint32_t index) const
    {
        if (index >= m_nodes.size())
        {
            return NULL;
        }

        return &(m_nodes[index]);
    }

    nv::matrix4f* NvSkeleton::GetTransforms()
    {
        if (m_nodeTransforms.empty())
        {
            return NULL;
        }
        return &(m_nodeTransforms[0]);
    }

    nv::matrix4f* NvSkeleton::GetTransform(uint32_t index)
    {
        if ((index < 0) || (index >= m_nodeTransforms.size()))
        {
            return NULL;
        }

        return &(m_nodeTransforms[index]);
    }
}