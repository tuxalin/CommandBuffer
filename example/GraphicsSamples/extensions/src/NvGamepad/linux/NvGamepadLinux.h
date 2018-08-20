//----------------------------------------------------------------------------------
// File:        NvGamepad\linux/NvGamepadLinux.h
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
#ifndef NV_GAMEPAD_LINUX_H
#define NV_GAMEPAD_LINUX_H

#include <NvSimpleTypes.h>

/// \file
/// Linux implementation of cross-platform gamepad support

#include <NvGamepad/NvGamepad.h>

/// Class representing the Linux implementation of gamepads.
/// Class implementing gamepad support for the Linux platform, using
/// standard linux joystick /dev/input/jsX event device.
class NvGamepadLinux : public NvGamepad {
public:
    /// Opens up the joystick device for reading.
    NvGamepadLinux();
    /// Closes the linux joystick device handle.
    virtual ~NvGamepadLinux();

    /// Attempts to read new data from joystick and convert into appropriate
    /// platform-independent tracking data.
    /// Returns mask of gamepads with changed data.
	virtual uint32_t pollGamepads();

    virtual bool getState(int32_t padID, State& state);
    virtual void setMaxGamepadCount(int32_t max);
    virtual int32_t getMaxGamepadCount();

    /// Whether there are gamepads ready to query, system is ready for use.
    /// \return true if we have a valid gamepad opened.
    virtual bool isReady() { return (mJoyFile>=0); }

private:
    uint32_t LinuxButtonToNV(uint32_t btn) {
        switch (btn) {
        case 0: return BUTTON_A;
        case 1: return BUTTON_B;
        case 2: return BUTTON_X;
        case 3: return BUTTON_Y;
        case 4: return BUTTON_LEFT_SHOULDER;
        case 5: return BUTTON_RIGHT_SHOULDER;
        case 6: return BUTTON_BACK;
        case 7: return BUTTON_START;
        case 8: return BUTTON_CENTER; //nv/xb/ps button.
        case 9: return BUTTON_LEFT_THUMB;
        case 10: return BUTTON_RIGHT_THUMB;
        default: return 0;
        }
    };

    int mJoyFile;
    int mStickDeadzone;
    int mTriggerDeadzone;
    State* mStates;
};

#endif

