//----------------------------------------------------------------------------------
// File:        NV/NvTime.h
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

#ifndef _NV_TIME_H
#define _NV_TIME_H

#include <time.h>

/** Generalize a 64-bit integer time value in nanoseconds. */
typedef uint64_t               NvUST; // U64 unadjusted system time value
/** Convert 64-bit nsecs value to floating-point seconds. */
#define UST2SECS(t)     ((double)(((double)(t)) / 1.0e9))
/** Convert floating-point seconds to a 64-bit nsecs value. */
#define SECS2UST(t)     ((NvUST)(((double)(t)) * 1.0e9))

/** Convert nsecs value to ms value. */
#define UST2MS(t)     ((t) / (NvUST)(1000000))
/** Convert ms to 64-bit nsecs value. */
#define MS2UST(t)     ((t) * (NvUST)(1000000))

inline NvUST NvTimeGetTime()
{
    NvUST nowu;
#ifdef _WIN32
    clock_t now = clock(); // clocks are really ms...
    nowu = MS2UST(now);
#else
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    nowu = SECS2UST(now.tv_sec) + now.tv_nsec;
#endif
    return nowu;
}

inline float NvTimeDiffInSecs(NvUST newTime, NvUST oldTime)
{
    if (oldTime<newTime)
        return (float)UST2SECS(newTime-oldTime);
    return 0; // !!!!!TBD should this return 1 just to 'tick' a bit?
}

#endif //_NV_TIME_H
