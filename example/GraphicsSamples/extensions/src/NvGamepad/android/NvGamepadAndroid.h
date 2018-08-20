//----------------------------------------------------------------------------------
// File:        NvGamepad\android/NvGamepadAndroid.h
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

#ifndef NV_GAMEPAD_ANDROID_H
#define NV_GAMEPAD_ANDROID_H

#include <NvSimpleTypes.h>

/// \file
/// Android implementation of cross-platform gamepad support

#include <NvGamepad/NvGamepad.h>

struct AInputEvent;
typedef struct AInputEvent AInputEvent;

/// Class representing the Android implementation of gamepads.
/// Class implementing gamepad support for the Android platform.  This
/// is mainly a "producer" class, used by an app framework or main loop
/// implementation to provide Android-specific events, so that an application
/// can access it as the #NvGamepad base class and read the gamepads.  As a
/// result, the only new function exposed by this subclass is #pollGamepads,
/// which passes Android input events to the implementation for updating the
/// gamepad state.
class NvGamepadAndroid : public NvGamepad {
public:
    NvGamepadAndroid();
    virtual ~NvGamepadAndroid();

    /// Passes Android input events to the gamepad system for processing.
    /// Used by the app framework or main loop to pass all Android input events
    /// to the gamepad system for processing.  Updates the internal representation
    /// of the state of all attached gamepads
    /// \param[in] event the Android input event to be processed by the gamepad
    /// system.  The caller need not and should not pre-process these events;
    /// all filtering will be done by the function and irrelevant events ignored
    /// \param[in,out] changedMask a mask of the changed gamepads.  Bits 
    /// 0-(#MAX_GAMEPADS-1) will be 1 if the gamepad of that index (1<<index) has changed.
    /// Note that an existing nonzero mask may be sent - this function will either
    /// leave the mask unchanged or will add set bits for any newly-changed pads
    /// \return true if gamepad support "ate" the event, and should not be
    /// processed any further by the app's framework
    bool processEvent(AInputEvent* event, uint32_t& changedMask);

    virtual bool getState(int32_t padID, State& state);
    virtual void setMaxGamepadCount(int32_t max);
    virtual int32_t getMaxGamepadCount();

	virtual uint32_t pollGamepads() { return 0; }

protected:
    static void ProcessDPAD(AInputEvent* event, State& state);

    uint32_t mCurrentTimestamp;
    uint32_t* mLastReturnedTimestamps;
    State* mStates;
};

#endif
