//----------------------------------------------------------------------------------
// File:        NvGamepad\win/NvGamepadXInput.h
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
#ifndef NV_GAMEPAD_XINPUT_H
#define NV_GAMEPAD_XINPUT_H

#include <NvSimpleTypes.h>
#include <NvGamepad/NvGamepad.h>
#include <Windows.h>
#include <xinput.h>

/// \file
/// XINPUT (Win32) implementation of cross-platform gamepad support

typedef DWORD(WINAPI *XInputGetStatePtr)(DWORD dwUserIndex, XINPUT_STATE* pState);

/// Class representing the XINPUT (Win32) implementation of gamepads.
/// Class implementing gamepad support for the XINPUT (Win32) platform.  This
/// is mainly a "producer" class, used by an app framework or main loop
/// implementation to regularly poll the XINPUT gamepads, so that an application
/// can access it as the #NvGamepad base class and read the gamepads.  As a
/// result, the only new function exposed by this subclass is #pollGamepads,
/// which calls into the XINPUT library to allow the class to update the
/// gamepad state.
class NvGamepadXInput : public NvGamepad {
public:
	NvGamepadXInput();
	virtual ~NvGamepadXInput();
	virtual bool getState(int32_t padID, State& state); // false if no pad connected at ID

														/// Calls the XINPUT polling functions to update the state of all gamepads.
														/// Used by the app framework or main loop to update the state of all gamepads
														/// Updates the internal representation of the state of all attached gamepads
														/// \return a mask of the changed gamepads.  Bits 0-(MAX_GAMEPADS-1) will be 1 if the
														/// gamepad of that index (1<<index) has changed.
	virtual uint32_t pollGamepads();

	virtual void setMaxGamepadCount(int32_t max);
	virtual int32_t getMaxGamepadCount();

protected:
	void updateFromXState(State& dest, const XINPUT_STATE& src);
	State* mStates;
	XINPUT_STATE* mXStates;
	static HMODULE msXInputDLL;
	static XInputGetStatePtr msXInputGetState;
};

#endif
