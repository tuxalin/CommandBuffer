//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvGPUTimerVK.h
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
#ifndef NV_GPUTIMERVK_H
#define NV_GPUTIMERVK_H

#include "NvVkUtil/NvPlatformVK.h"
#include "NvVkUtil/NvVkContext.h"

/// \file
/// GPU timers and scope wrappers.

class NvGPUTimerVK
{
public:
    /// Creates a stopped, zeroed timer; need not be called with a bound
    /// OpenGL context
	NvGPUTimerVK()
    : m_elapsedCycles(0.f)
    , m_nextTimeStampQuery(0)
    , m_startStopCycles(0)
	, m_freq(1.0) {
        for (unsigned int i = 0; i < TIMER_COUNT; i++)
            m_timeStampQueryInFlight[i] = false;
    }

    /// Initializes the VK parts of the timer.
    void init(NvVkContext& vk) {
		m_device = vk.device();

		VkQueryPoolCreateInfo queryInfo = { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
		queryInfo.queryCount = TIMER_COUNT<<1;
		queryInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
		VkResult result = vkCreateQueryPool(m_device, &queryInfo, NULL, &m_pool);

		// timestampPeriod = ns / cycle
		// we want ms/cycle
		m_freq = 1e-6 * double(vk.physicalDeviceLimits().timestampPeriod);
    }

    /// Resets the elapsed time and count of start/stop calls to zero
    void reset() {
        m_elapsedCycles = 0.0f;
        m_startStopCycles = 0;
    }

    /// Starts the timer
    void start(VkCommandBuffer cmd) {
		getResults();  // add timings from previous start/stop if pending
		vkCmdResetQueryPool(cmd, m_pool, m_nextTimeStampQuery<<1, 2); // not ideal to do this per query
		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_pool, m_nextTimeStampQuery << 1);
	}

    /// Starts the timer 
	void stop(VkCommandBuffer cmd) {
		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, m_pool, (m_nextTimeStampQuery << 1) + 1);
		m_timeStampQueryInFlight[m_nextTimeStampQuery] = true;
	}

    /// Get the number of start/stop cycles whose values have been accumulated
    /// since the last reset.  This value may be lower than the number of call
    /// pairs to start/stop, as this value does not take into account the start/stop
    /// cycles that are still "in flight" (awaiting results).
    /// \return the number of start/stop cycles represented in the current 
    /// accumulated time
    int32_t getStartStopCycles() { 
        return m_startStopCycles; 
    }

    /// Get the amount of time accumulated for start/stop cycles that have completed
    /// since the last reset.  This value may be lower than the time for all call
    /// pairs to start/stop since the last reset, as this value does not take into 
    /// account the start/stop cycles that are still "in flight" (awaiting results).
    /// \return the accumulated time of completed start/stop cycles since the last reset
    float getScaledCycles() {
        getResults();

        return m_elapsedCycles;
    }

protected:
    /// \privatesection
    void getResults() {
        // Make a pass over all timers - if any are pending results ("in flight"), then
        // grab the the time diff apply it as the new time if it is the most recent stamp.
		// Then, mark the timer as not in flight and record it as a possible next timer to
		// use (since it is now unused)
        int32_t freeQuery = -1;
		uint64_t minTime = 0;
        for (unsigned int i = 0; i < TIMER_COUNT; i++) {
			uint32_t startIdx = i<<1;
			uint32_t endIdx = startIdx + 1;

			// Are we awaiting a result?
            if (m_timeStampQueryInFlight[i]) {
				uint64_t times[2];
				// This throws likely false-positive validation errors in the
				// Core validation layer.  https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/issues/367 
				VkResult result = vkGetQueryPoolResults(m_device, m_pool, startIdx, 2, 2*sizeof(uint64_t), times, 0, 
					VK_QUERY_RESULT_64_BIT);

                // Is the result ready?
				if (result == VK_SUCCESS) {
                
                    // update if newer
					if (minTime < times[1]) {
						m_elapsedCycles = float(double(times[1] - times[0]) * m_freq);
						m_startStopCycles = 1;
						minTime = times[1];
					}
					freeQuery = i;
                    m_timeStampQueryInFlight[i] = false;
				}
            }
        }
        // Use the newly-freed counters if one exists
        // otherwise, swap to the "next" counter
        if (freeQuery >= 0)
        {
            m_nextTimeStampQuery = freeQuery;
        }
        else
        {
            m_nextTimeStampQuery++;
            m_nextTimeStampQuery = (m_nextTimeStampQuery >= TIMER_COUNT) ? 0 : m_nextTimeStampQuery;
        }
    }

    const static unsigned int TIMER_COUNT = 16;

	VkDevice m_device;
    VkQueryPool m_pool;
    float m_elapsedCycles;
    uint32_t m_nextTimeStampQuery;
    int32_t m_startStopCycles;
    bool m_timeStampQueryInFlight[TIMER_COUNT];
	double m_freq;
};

#endif
