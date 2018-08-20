//----------------------------------------------------------------------------------
// File:        NvAppBase/NvCPUTimer.h
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
#ifndef NV_CPUTIMER_H
#define NV_CPUTIMER_H

#include "NV/NvStopWatch.h"


/// A CPU timer class
class NvCPUTimer
{
public:
    NvCPUTimer()
    : m_elapsedTime(0.f)
    , m_stopwatch(NULL)
	, m_cycles(0) {
    }

    /// Initializes the timer.  This function must be called after globalInit
    void init() {
        m_stopwatch = ms_factory->createStopWatch();
    }

    /// Resets the elapsed time accumulator to zero
    void reset() {
        m_elapsedTime = 0.0f;
		m_cycles = 0;
    }

    /// Starts the timer
    void start() {
        m_stopwatch->start();
		m_cycles++;
    }

    /// Stops the timer
    void stop() {
        m_stopwatch->stop();
        m_elapsedTime += m_stopwatch->getTime();
    }

    /// Returns the accumulated time of all completed start/stop
    /// cycles since the last reset
    /// \return the accumulated time
    float getScaledCycles() {
        return m_elapsedTime;
    }

	/// Returns the number of times the timer has been started and stopped since reset
	/// \return the start/stop count (1 per start/stop pair)
	uint32_t getStartStopCycles() { return m_cycles; }

    /// Static initialization to pass the app framework's CPU
    /// stopwatch factory object, to be used in the creation of
    /// all CPU timers.
    /// \param [in] factory the factory object, likely from the app framework
    static void globalInit(NvStopWatchFactory* factory) {
        ms_factory = factory;
    }

protected:
    /// \privatesection
    NvStopWatch* m_stopwatch;
    float m_elapsedTime;
	uint32_t m_cycles;
    static NvStopWatchFactory* ms_factory;
};


/// A helper class that allows for simple "code block" implicit timer
/// start/stops.  The following code:
/// \code
///     {
///         NvCPUTimerScope myScope(myTimer);
///         // ... my block of timed code
///     }
/// \endcode
/// Is equivalent to:
/// \code
///     {
///         myTimer->start();
///         // ... my block of timed code
///         myTimer->stop();
///     }
/// \endcode

struct NvCPUTimerScope {
    /// Constructor - starts the given timer
    /// \param [in] timer the timer to be used
    NvCPUTimerScope(NvCPUTimer *timer) : m_timer(timer) { timer->start(); }
    /// Destructor - stops the timer that was passed to the constructor
    ~NvCPUTimerScope() { m_timer->stop(); }
    NvCPUTimer *m_timer;
};


#endif
