//----------------------------------------------------------------------------------
// File:        NvGamepad\android/NvGamepadAndroid.cpp
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
#include "NvGamepadAndroid.h"
#include <unistd.h>
#include <android/log.h>
extern "C" {
#include <android/input.h>
}
#include <math.h>

NvGamepad* NvGamepad::createDefaultGamePad() {
	return new NvGamepadAndroid;
}


NvGamepadAndroid::NvGamepadAndroid() {
    mStates = new State[MAX_GAMEPADS];
    mLastReturnedTimestamps = new uint32_t[MAX_GAMEPADS];

    memset(mStates, 0, MAX_GAMEPADS * sizeof(State));
    memset(mLastReturnedTimestamps, 0, MAX_GAMEPADS * sizeof(uint32_t));

    mCurrentTimestamp = 0;

    uint32_t* mLastReturnedTimestamps;
}

NvGamepadAndroid::~NvGamepadAndroid() {
    delete[] mStates;
}

bool NvGamepadAndroid::getState(int32_t padID, State& state) {
    state = mStates[padID];
    mLastReturnedTimestamps[padID] = state.mTimestamp;

    // For now, we assume that any pad that has sent us an event is connected.
    // This is obviously a hack, since it does not include:
    // 1) Connected pads that have yet to be touched
    // 2) Disconnected pads
    return (state.mTimestamp != 0) ? true : false;
}

// overly-simple dead-zoning for now.
static float MapCenteredAxis(AInputEvent* event, int32_t axis) {
    const float deadZone = (8689.0f / 32768.0f);
    float value = AMotionEvent_getAxisValue(event, axis, 0);
    if (value > deadZone)
        return (value - deadZone) / (1.0f - deadZone);
    else if (value < -deadZone)
        return (value + deadZone) / (1.0f - deadZone);
    else
        return 0.0f;
}

static float MapAxis(AInputEvent* event, int32_t axis) {
    const float deadZone = 0.1f;
    float value = AMotionEvent_getAxisValue(event, axis, 0);
    if (value > deadZone)
        return (value - deadZone) / (1.0f - deadZone);
    else
        return 0.0f;
}

static bool UpdateButtonMask(bool down, uint32_t& currentMask, uint32_t flag) {
    uint32_t oldMask = currentMask;
    if (down)
        currentMask |= flag;
    else
        currentMask &= (~flag);

    return (oldMask != currentMask) ? true : false;
}

void NvGamepadAndroid::ProcessDPAD(AInputEvent* event, State& state) {
    const float deadZone = 0.5f;
    float h = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);
    float v = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);

    if (h < -deadZone)
        state.mButtons = (state.mButtons & (~BUTTON_DPAD_RIGHT)) | BUTTON_DPAD_LEFT;
    else if (h > deadZone)
        state.mButtons = (state.mButtons & (~BUTTON_DPAD_LEFT)) | BUTTON_DPAD_RIGHT;
    else
        state.mButtons &= ~(BUTTON_DPAD_LEFT | BUTTON_DPAD_RIGHT);

    if (v < -deadZone)
        state.mButtons = (state.mButtons & (~BUTTON_DPAD_DOWN)) | BUTTON_DPAD_UP;
    else if (v > deadZone)
        state.mButtons = (state.mButtons & (~BUTTON_DPAD_UP)) | BUTTON_DPAD_DOWN;
    else
        state.mButtons &= ~(BUTTON_DPAD_DOWN | BUTTON_DPAD_UP);
}

bool NvGamepadAndroid::processEvent(AInputEvent* event, uint32_t& changedMask) {
    int32_t eventType = AInputEvent_getType(event);

    // Hack - pad 0 only for now...
    State& state = mStates[0];

    if (eventType == AINPUT_EVENT_TYPE_MOTION)
    {
        if (AINPUT_SOURCE_JOYSTICK == AInputEvent_getSource(event))
        {
            state.mThumbLX = MapCenteredAxis(event, AMOTION_EVENT_AXIS_X);
            state.mThumbLY = MapCenteredAxis(event, AMOTION_EVENT_AXIS_Y);
            state.mThumbRX = MapCenteredAxis(event, AMOTION_EVENT_AXIS_Z);
            state.mThumbRY = MapCenteredAxis(event, AMOTION_EVENT_AXIS_RZ);

            ProcessDPAD(event, state);

            // need to map bot pairs of axes as of Android L
            {
                float lTrigger = MapAxis(event, AMOTION_EVENT_AXIS_LTRIGGER);
                float brake = MapAxis(event, AMOTION_EVENT_AXIS_BRAKE);
                state.mLeftTrigger = (lTrigger > brake) ? lTrigger : brake;
            }
            {
                float gas = MapAxis(event, AMOTION_EVENT_AXIS_GAS);
                float rTrigger = MapAxis(event, AMOTION_EVENT_AXIS_RTRIGGER);
                state.mRightTrigger = (rTrigger > gas) ? rTrigger : gas;
            }

            state.mTimestamp++;
            changedMask |= 1;

            return true;
        }
    } else if (eventType == AINPUT_EVENT_TYPE_KEY) {
        int32_t source = AInputEvent_getSource(event);
        int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);
        int32_t action = AKeyEvent_getAction((const AInputEvent*)event);

        bool down = (action == AKEY_EVENT_ACTION_UP) ? false : true;
        uint32_t button = 0;

        switch (code) {
        case AKEYCODE_BUTTON_THUMBL:
            button = BUTTON_LEFT_THUMB;
            break;
        case AKEYCODE_BUTTON_THUMBR:
            button = BUTTON_RIGHT_THUMB;
            break;
        case AKEYCODE_DPAD_UP:
            button = BUTTON_DPAD_UP;
            break;
        case AKEYCODE_DPAD_DOWN:
            button = BUTTON_DPAD_DOWN;
            break;
        case AKEYCODE_DPAD_LEFT:
            button = BUTTON_DPAD_LEFT;
            break;
        case AKEYCODE_DPAD_RIGHT:
            button = BUTTON_DPAD_RIGHT;
            break;
        case AKEYCODE_DPAD_CENTER:
            button = BUTTON_CENTER;
            break;
        case AKEYCODE_BUTTON_L1:
            button = BUTTON_LEFT_SHOULDER;
            break;
        case AKEYCODE_BUTTON_R1:
            button = BUTTON_RIGHT_SHOULDER;
            break;
        case AKEYCODE_BUTTON_START:
            button = BUTTON_START;
            break;
        case AKEYCODE_BUTTON_A:
            button = BUTTON_A;
            break;
        case AKEYCODE_BUTTON_B:
            button = BUTTON_B;
            break;
        case AKEYCODE_BUTTON_X:
            button = BUTTON_X;
            break;
        case AKEYCODE_BUTTON_Y:
            button = BUTTON_Y;
            break;
        case AKEYCODE_MEDIA_PLAY_PAUSE:
            button = BUTTON_PLAY_PAUSE;
            break;
        default:
#ifdef _DEBUG
            __android_log_print(ANDROID_LOG_VERBOSE, "NvGamepad",  "unknown button 0x%0x", code);
#endif
            return false;
        };

        if (button) {
            if (UpdateButtonMask(down, state.mButtons, button)) {
                changedMask |= 1;
                state.mTimestamp++;
            }
            return true;
        }
    }

    return false;
}

void NvGamepadAndroid::setMaxGamepadCount(int32_t max) {
    /* */
}

int32_t NvGamepadAndroid::getMaxGamepadCount() {
    return MAX_GAMEPADS;
}
