//----------------------------------------------------------------------------------
// File:        NvAppBase/NvFramerateCounter.cpp
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
 
/* Framerate stats */
#include <NvAppBase/NvFramerateCounter.h>
#include <NvAppBase/NvAppBase.h>
#include <NV/NvStopWatch.h>

NvFramerateCounter::NvFramerateCounter(NvStopWatchFactory* factory) {
    m_stopWatch = factory->createStopWatch();
    m_reportFrames = 5;
    m_reportRate = 0.5f;
    m_framesSinceReport = 0;
    m_meanFramerate = 0.0f;
}

NvFramerateCounter::~NvFramerateCounter() {

}

bool NvFramerateCounter::nextFrame() {
    if (!m_stopWatch->isRunning()) {
        m_stopWatch->start();
        m_framesSinceReport = 0;
        return false;
    }

    m_framesSinceReport++;

    // Only report if the number of frames AND the min time are exceeded
    if (m_framesSinceReport >= m_reportFrames) {
        float time = m_stopWatch->getTime();
        if (time > m_reportRate) {
            m_meanFramerate = m_framesSinceReport / time;
            m_framesSinceReport = 0;
            m_stopWatch->reset();
            return true;
        }
    }

    return false;
}

void NvFramerateCounter::reset() {
    m_framesSinceReport = 0;
    m_stopWatch->stop();
    m_stopWatch->reset();
    m_stopWatch->start();
}
