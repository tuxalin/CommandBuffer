//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/SchoolStateManager.h
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
#ifndef SCHOOLSTATEMANAGER_H_
#define SCHOOLSTATEMANAGER_H_
#include "NV/NvMath.h"

/// Structure to hold last computed state for a particular School
struct SchoolState
{
    /// Centroid of the school
    nv::vec3f m_center;
    /// Radius of the school in meters
    float     m_radius;
    /// Aggression factor (determines avoidance with/of other schools)
    float     m_aggression;
};

/// Class to manage a set of states for each school that can be written to
/// and read from in a lock-free, thread-safe manner.  The "Read" and "Write"
/// buffers are alternated each frame, so that all threads are either reading
/// or writing to them, but never both.
class SchoolStateManager
{
public:
    /// Constructor. Determines how many schools will be managed
    /// \param maxSchools Maximum number of school states contained in the buffer
    SchoolStateManager(uint32_t maxSchools)
        : m_capacity(maxSchools)
        , m_numReadStates(0)
        , m_numWriteStates(0)
    {
        m_readBuffer = new SchoolState[maxSchools];
        m_writeBuffer = new SchoolState[maxSchools];
    }

    ~SchoolStateManager()
    {
        delete[] m_readBuffer;
        delete[] m_writeBuffer;
    }

    /// Swap the write buffer to the read buffer in preparation for updating
    /// the next frame
    /// \param numSchools Number of active schools this frame
    void BeginFrame(uint32_t numSchools)
    {
        // Swap the read and write buffers
        SchoolState* pTemp = m_writeBuffer;
        m_writeBuffer = m_readBuffer;
        m_readBuffer = pTemp;

        // The number of readable states is the number of states
        // that were written last frame
        m_numReadStates = m_numWriteStates;

        // The number of writable states is the number passed in
        m_numWriteStates = numSchools;
    }

    /// Return the number of states written to last frame, thus readable this frame
    /// \return The number of states in the readable buffer
    uint32_t GetNumReadStates() const { return m_numReadStates; }

    /// Retrieve a pointer to the readable buffer of SchoolStates for this frame
    SchoolState* GetReadStates() { return m_readBuffer; }

    /// Return the number of states writable to for this frame
    /// \return The number of states in the writable buffer
    uint32_t GetNumWriteStates() const { return m_numWriteStates; }

    /// Retrieve a pointer to the writable buffer of SchoolStates for this frame
    SchoolState* GetWriteStates() { return m_writeBuffer; }

protected:
    /// Default constructor.  Unavailable as the maximum 
    /// number of states must be declared at creation time.
    SchoolStateManager() {}

    /// Maximum number of SchoolStates that can be held in the
    /// readable/writable buffers
    uint32_t m_capacity;

    /// Current number of SchoolStates in m_readBuffer
    uint32_t m_numReadStates;

    /// Pointer to the currently readable set of SchoolStates
    SchoolState* m_readBuffer;

    /// Current number of SchoolStates in m_writeBuffer
    uint32_t m_numWriteStates;

    /// Pointer to the currently writable set of SchoolStates
    SchoolState* m_writeBuffer;
};

#endif // SCHOOLSTATEMANAGER_H_
