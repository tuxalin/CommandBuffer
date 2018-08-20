//----------------------------------------------------------------------------------
// File:        NvGamepad/NvGamepad.h
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
#ifndef NV_GAMEPAD_H
#define NV_GAMEPAD_H

#include <NvSimpleTypes.h>

/// \file
/// Cross-platform gamepad support

/// Class representing the state of all gamepads.
/// Class representing the state of all attached gamepads, designed to be used by
/// consumers of gamepad events.  This represents the digital buttons and analog
/// axes of multiple gamepads.  This is an interface class.  The platform will make
/// a concrete instance of this available to the consumer through some method external
/// to this API
class NvGamepad {
public:
    const static uint32_t BUTTON_DPAD_UP        = 0x00000001; ///< Mask for DPAD up direction
    const static uint32_t BUTTON_DPAD_DOWN      = 0x00000002; ///< Mask for DPAD down direction
    const static uint32_t BUTTON_DPAD_LEFT      = 0x00000004; ///< Mask for DPAD left direction
    const static uint32_t BUTTON_DPAD_RIGHT     = 0x00000008; ///< Mask for DPAD right direction
    const static uint32_t BUTTON_START          = 0x00000010; ///< Mask for START button
    const static uint32_t BUTTON_BACK           = 0x00000020; ///< Mask for BACK button
    const static uint32_t BUTTON_LEFT_THUMB     = 0x00000040; ///< Mask for left thumbstick click
    const static uint32_t BUTTON_RIGHT_THUMB    = 0x00000080; ///< Mask for right thumbstick click
    const static uint32_t BUTTON_LEFT_SHOULDER  = 0x00000100; ///< Mask for left shoulder button (AKA L1)
    const static uint32_t BUTTON_RIGHT_SHOULDER = 0x00000200; ///< Mask for right shoulder button (AKA R1)
    const static uint32_t BUTTON_PLAY_PAUSE     = 0x00000400; ///< Mask for a media play/pause button (AKA remotes)
    const static uint32_t BUTTON_CENTER         = 0x00000800; ///< Mask for center button (AKA X, NV, PS)
    const static uint32_t BUTTON_A              = 0x00001000; ///< Mask for A action button
    const static uint32_t BUTTON_B              = 0x00002000; ///< Mask for B action button
    const static uint32_t BUTTON_X              = 0x00004000; ///< Mask for X action button
    const static uint32_t BUTTON_Y              = 0x00008000; ///< Mask for Y action button

    const static int32_t MAX_GAMEPADS = 4; ///< Max number of supported, simultaneous gamepads

    /// The state of a single gamepad.
    /// The full state of the axes and buttons on a single gamepad
    struct State {
        uint32_t mTimestamp; ///< Logical clock incremented on any change
        uint32_t mButtons; ///< Flag vector of BUTTON_*, with a bit set to 1 indicating the corresponding button is pressed
        float mLeftTrigger; ///< Left analog trigger value [0.0, 1.0]
        float mRightTrigger; ///< Right analog trigger value [0.0, 1.0]
        float mThumbLX; ///< Left thumbstick horizontal axis [-1.0, 1.0], deadzone already zeroed
        float mThumbLY; ///< Left thumbstick vertical axis [-1.0, 1.0], deadzone already zeroed
        float mThumbRX; ///< Right thumbstick horizontal axis [-1.0, 1.0], deadzone already zeroed
        float mThumbRY; ///< Right thumbstick vertical axis [-1.0, 1.0], deadzone already zeroed
    };

    /// Get the state of a gamepad.
    /// Queries the state of the desired gamepad, copying it into an
    /// application-supplied status structure
    /// \param[in] padID the index of the pad to read [0, MAX_GAMEPADS)
    /// \param[out] state the current state of the queried gamepad
    /// \return True on success, false if no pad connected at padID
    /// \sa State
    virtual bool getState(int32_t padID, State& state) = 0; // 

    /// Sets the maximum number of simultaneous gamepads.
    /// Sets the maximum number of gamepads that the system should track
    /// For apps that will not support more than a fixed count of controllers,
    /// this could lower the expense of the library.  This is purely a hint
    /// \param[in] sets the maximum number of expected gamepads [1, #MAX_GAMEPADS]
    virtual void setMaxGamepadCount(int32_t max) = 0;

    /// Gets the maximum number of simultaneous gamepads.
    /// \return the current maximum number of gamepads.  Default is #MAX_GAMEPADS
    virtual int32_t getMaxGamepadCount() = 0;

    virtual ~NvGamepad() { }

    /// Whether there are gamepads ready to query, system is ready for use.
    /// \return true by default, subclasses may determine situations to return false.
    virtual bool isReady() { return true; }

	virtual uint32_t pollGamepads() = 0;

	/// \privatesection
	static NvGamepad* createDefaultGamePad();
};

#endif
