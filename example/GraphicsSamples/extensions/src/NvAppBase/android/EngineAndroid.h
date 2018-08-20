//----------------------------------------------------------------------------------
// File:        NvAppBase\android/EngineAndroid.h
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
#ifndef ENGINE_ANDROID_H
#define ENGINE_ANDROID_H

//BEGIN_INCLUDE(all)
#include <NvSimpleTypes.h>
#include <jni.h>
#include <errno.h>
#include "NvAppBase/NvAppBase.h"

/**
 * Our saved state data.
 */
struct saved_state {
    float x;
    float y;
};

struct HDRDisplayState {
	enum Enum {
		UNCHECKED,
		COMPATIBLE,
		INCOMPATIBLE,
	};
};


struct AInputEvent;
struct android_app;
class NvGamepadAndroid;
class NvAndroidWinUtil;

/**
 * Shared state for our app.
 */
class Engine : public NvPlatformContext {
public:
    Engine(struct android_app* app);
    ~Engine();

    virtual bool isAppRunning();
    virtual void requestExit();
    virtual bool pollEvents(NvInputCallbacks* callbacks);
    virtual bool isContextLost();
    virtual bool isContextBound();
    virtual bool shouldRender();
    virtual bool hasWindowResized();
    virtual NvGamepad* getGamepad() { return (NvGamepad*)mGamepad; }
    virtual void setAppTitle(const char* title) { /* TODO if anything we can do... */ }

    virtual NvRedrawMode::Enum getRedrawMode();
    virtual void setRedrawMode(NvRedrawMode::Enum mode);
    virtual void requestRedraw();

    virtual void* getPlatformApp() { return (void*)mApp; }

    struct android_app* mApp;
    NvAppBase* mAppBase;

    int32_t mForceRender;
    bool mRedraw;
    bool mResizePending;
	NvAndroidWinUtil* mWin;
    struct saved_state mState;
    NvInputCallbacks* mCallbacks;

    NvGamepadAndroid* mGamepad;
    uint32_t mPadChangedMask;

    static int32_t handleInputThunk(struct android_app* app, AInputEvent* event);
    static void handleCmdThunk(struct android_app* app, int32_t cmd);

    int32_t handleInput(AInputEvent* event);
    void handleCommand(int32_t cmd);

    void requestForceRender() { mForceRender = 1; }

    bool useChoreographer(bool use);
	bool isDisplayHDRCompatible();

    NvRedrawMode::Enum mRedrawMode;
	HDRDisplayState::Enum mDisplayIsHDRCompatible;
	
};

#endif
