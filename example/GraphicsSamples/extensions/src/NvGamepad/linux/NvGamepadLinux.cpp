//----------------------------------------------------------------------------------
// File:        NvGamepad\linux/NvGamepadLinux.cpp
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
#include "NvGamepadLinux.h"
#include <linux/input.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef LINUX
#include <string.h>
#endif

NvGamepad* NvGamepad::createDefaultGamePad() {
//	return new NvGamepadLinux;
	return NULL;
}

char cJoyDevPath[64] = "/dev/input/js0"; // vs /eventX interface... tbd.

NvGamepadLinux::NvGamepadLinux()
: mJoyFile(-1)
, mStickDeadzone(8000)
, mTriggerDeadzone(400)
{
    mStates = new State[MAX_GAMEPADS];
    
    memset(mStates, 0, MAX_GAMEPADS * sizeof(State));

    // opening the joystick...  find ONE for now !!!!TBD can find and store N in the future.
    for (int i=0; i<4; i++) {
        int j = strlen(cJoyDevPath);
        cJoyDevPath[j-1] = '0'+i;
        mJoyFile = open(cJoyDevPath, O_RDONLY | O_NONBLOCK);
        if (mJoyFile==-1) {
            // print error
            printf("!> Failed to open joystick at %s\n", cJoyDevPath);
        } else {
            printf("!> Opened joystick at %s\n", cJoyDevPath);
            break;
        }
    }

    if (mJoyFile==-1) {
        // print error
        printf("!> Failed to detect and open any attached joystick device.");
    }
}


NvGamepadLinux::~NvGamepadLinux() {
    if (mJoyFile>=0)
        close(mJoyFile);
    mJoyFile = -1;
    delete[] mStates;
}


bool NvGamepadLinux::getState(int32_t padID, State& state) {
    state = mStates[padID];
    
    // For now, we assume that any pad that has sent us an event is connected.
    // This is obviously a hack, since it does not include:
    // 1) Connected pads that have yet to be touched
    // 2) Disconnected pads
    return (state.mTimestamp != 0) ? true : false;
}

// overly-simple dead-zoning for now.
// really should wait until polling time, and then use
// the matching axes per stick and magnitude checks like XINPUT gamepad does.
static float MapCenteredAxis(int valueS16, int deadi) {
    const float value = (valueS16 / 32767.0f);
    const float deadZone = (deadi / 32767.0f); // can't we make this a mul??? !!!!TBD TODO
    if (value > deadZone)
        return (value - deadZone) / (1.0f - deadZone);
    else if (value < -deadZone)
        return (value + deadZone) / (1.0f - deadZone);
    else
        return 0.0f;
}

static float MapAxis(int valueS16, int deadi) {
    const float value = (valueS16 / 32767.0f);
    const float deadZone = (deadi / 32767.0f); // can't we make this a mul??? !!!!TBD TODO
    if (value > deadZone)
        return (value - deadZone) / (1.0f - deadZone);
    else
        return 0.0f;
}

static bool UpdateButtonMask(bool down, uint32_t& currentMask, uint32_t flag) {
    if (flag==0) return false;

    uint32_t oldMask = currentMask;
    if (down)
        currentMask |= flag;
    else
        currentMask &= (~flag);

    return (oldMask != currentMask) ? true : false;
}


static void ProcessDPAD(const struct js_event& ev, NvGamepad::State& state) {
    const float deadZone = 0.5f;
    if (ev.number==6) {// dpad X axis
        if (ev.value < -deadZone)
            state.mButtons = (state.mButtons & (~NvGamepad::BUTTON_DPAD_RIGHT)) | NvGamepad::BUTTON_DPAD_LEFT;
        else if (ev.value > deadZone)
            state.mButtons = (state.mButtons & (~NvGamepad::BUTTON_DPAD_LEFT)) | NvGamepad::BUTTON_DPAD_RIGHT;
        else
            state.mButtons &= ~(NvGamepad::BUTTON_DPAD_LEFT | NvGamepad::BUTTON_DPAD_RIGHT);
    } else if (ev.number==7) {
        if (ev.value < -deadZone)
            state.mButtons = (state.mButtons & (~NvGamepad::BUTTON_DPAD_DOWN)) | NvGamepad::BUTTON_DPAD_UP;
        else if (ev.value > deadZone)
            state.mButtons = (state.mButtons & (~NvGamepad::BUTTON_DPAD_UP)) | NvGamepad::BUTTON_DPAD_DOWN;
        else
            state.mButtons &= ~(NvGamepad::BUTTON_DPAD_DOWN | NvGamepad::BUTTON_DPAD_UP);
    }
}


uint32_t NvGamepadLinux::pollGamepads() {
    if (!isReady()) return 0;

    // Hack - pad 0 only for now...
    State& state = mStates[0];
    uint32_t mask = 0;

    // read up to 255 entries in a go...
	struct js_event evBuf[255];
	int evCount = read(mJoyFile, evBuf, sizeof(evBuf));
    if (evCount <= 0)
        return mask;
    evCount /= sizeof(js_event); // convert bytes to records...

    for (int i=0; i<evCount; i++) {
        const struct js_event& ev = evBuf[i];

        //if (ev.type & JS_EVENT_INIT)
        //    continue; // ignore.
        // actually, we want the init events, so our data starts off w/correct values.

        if (ev.type & JS_EVENT_AXIS) {
            //printf("AXIS %d value %d\n", ev.number, ev.value);
            float value = 0;
            switch(ev.number) {
            case 0:
                state.mThumbLX = MapCenteredAxis(ev.value, mStickDeadzone);
                break;
            case 1:
                state.mThumbLY = MapCenteredAxis(ev.value, mStickDeadzone);
                break;
            case 2:
                state.mLeftTrigger = MapAxis(ev.value, mTriggerDeadzone);
                break;
            case 3:
                state.mThumbRX = MapCenteredAxis(ev.value, mStickDeadzone);
                break;
            case 4:
                state.mThumbRY = MapCenteredAxis(ev.value, mStickDeadzone);
                break;
            case 5:
                state.mRightTrigger = MapAxis(ev.value, mTriggerDeadzone);
                break;
            case 6:
            case 7:
                ProcessDPAD(ev, state);
                break;
            }

            mask |= 1;
            state.mTimestamp++;
        }

        if (ev.type & JS_EVENT_BUTTON) {
            if (UpdateButtonMask(ev.value, state.mButtons, LinuxButtonToNV(ev.number))) {
                //printf("Button %d state %d\n", ev.number, ev.value);
                mask |= 1;
                state.mTimestamp++;
            }
        }
	}

    return mask;
}

void NvGamepadLinux::setMaxGamepadCount(int32_t max) {
    /* */
}

int32_t NvGamepadLinux::getMaxGamepadCount() {
    return MAX_GAMEPADS;
}

