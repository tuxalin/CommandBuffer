//----------------------------------------------------------------------------------
// File:        NV/NvStopWatch.h
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
 
/* Stopwatch Timing Functions */

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <NvSimpleTypes.h>

/// \file
/// Basic wall-clock timer

/// Cross-platform wall-clock timer.
/// Timer including multiple start-stop support
/// Abstract base class.  The application framework must provide an implementation
/// and a method of creating instances
class NvStopWatch 
{
public:
    /// Constructor, default
    NvStopWatch() { m_running = false; } 
    /// Destructor
    virtual ~NvStopWatch() { }

    /// Starts time measurement
    virtual void start() = 0;

    /// Stop time measurement
    virtual void stop() = 0;

    /// Reset time counters to zero
    virtual void reset() = 0;

    /// Test whether the timer is running
    /// \return true if the timer is running (between #start() and #stop() calls) and false if not
    bool isRunning() { return m_running; }

    /// Get elapsed time
    /// Time in seconds after start. If the stop watch is still running (i.e. there
    /// was no call to #stop()) then the elapsed time is returned, otherwise the
    /// summed time between all #start() and #stop() calls is returned
    /// \return The elapsed time in seconds
    virtual const float getTime() const = 0;

protected:
    bool m_running;
};

class NvStopWatchFactory {
public:
    NvStopWatchFactory() { }
    virtual ~NvStopWatchFactory() { }

    virtual NvStopWatch* createStopWatch() = 0;
};


#endif
