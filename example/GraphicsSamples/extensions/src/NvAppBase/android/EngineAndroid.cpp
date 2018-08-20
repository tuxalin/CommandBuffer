//----------------------------------------------------------------------------------
// File:        NvAppBase\android/EngineAndroid.cpp
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
#include "EngineAndroid.h"
#include "NvGamepad/android/NvGamepadAndroid.h"
#include "NvAndroidNativeAppGlue.h"
#include "NvAndroidWinUtil.h"
#include "NV/NvLogs.h"
#include <assert.h>
#include <malloc.h>

Engine::Engine(struct android_app* app) {
    mApp = app;
    mAppBase = NULL;

    mForceRender = 0;
    mRedraw = false;
    mResizePending = false;
    
    mWin = NULL;

    memset( &mState, 0, sizeof(struct saved_state));

    mApp->userData = this;
    mApp->onAppCmd = handleCmdThunk;
    mApp->onInputEvent = handleInputThunk;

    mGamepad = new NvGamepadAndroid;

    if (mApp->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        mState = *(struct saved_state*)mApp->savedState;
    }

    mRedrawMode = NvRedrawMode::UNBOUNDED;
	mDisplayIsHDRCompatible = HDRDisplayState::UNCHECKED;
}

Engine::~Engine()
{
    delete mWin;
}

bool Engine::isAppRunning()
{
    return nv_app_status_running(mApp);
}

void Engine::requestExit()
{
    ANativeActivity_finish(mApp->activity);
}

bool Engine::pollEvents(NvInputCallbacks* callbacks)
{
    // Read all pending events.
    int32_t ident;
    int32_t events;
    struct android_poll_source* source;

    mCallbacks = callbacks;
    mPadChangedMask = 0;

    // We will block different lengths waiting for events,
    // depending on mode and availability of the Choreographer.
    while ((ident=ALooper_pollAll(nv_android_app_loop_wait(mApp), NULL, &events,
            (void**)&source)) >= 0) {

        // Process this event.
        if (source != NULL) {
            source->process(mApp, source);
        } else if (nv_app_is_redraw(ident)) {
            LOGI("Redraw event");
            int64_t time = nv_app_process_redraw(mApp);
            
            if (time)
                mRedraw = true;
        }

        if (!nv_app_status_running(mApp))
            break;

        if (mRedraw || mForceRender>0)
            break;
    }

    if (nv_android_app_free_redraw(mApp) && (mRedrawMode != NvRedrawMode::ON_DEMAND))
        mRedraw = true;

    if (mCallbacks && mPadChangedMask)
        mCallbacks->gamepadChanged(mPadChangedMask);

    mCallbacks = NULL;
}

bool Engine::shouldRender()
{
    if (nv_app_status_interactable(mApp))
    {
        if (!mWin->isReadyToRender(true))
            return false;

        bool redraw = mRedraw;
        if (mForceRender > 0)
        {
            mForceRender--;
            redraw = true;
        }
        mRedraw = false;
        return redraw;
    }
    else
    {
        mRedraw = false;
        // Even if we are not interactible, we may be visible, so we
        // HAVE to do any forced renderings if we can.  We must also
        // check for resize, since that may have been the point of the
        // forced render request in the first place!
        if ((mForceRender > 0) && mWin->isReadyToRender(false)) 
        {
            mForceRender--;
            return true;
        }
    }

    return false;
}

NvRedrawMode::Enum Engine::getRedrawMode()
{ 
    return mRedrawMode; 
}

void Engine::setRedrawMode(NvRedrawMode::Enum mode)
{
    switch (mode)
    {
        case NvRedrawMode::UNBOUNDED:
            useChoreographer(false);
            mRedrawMode = NvRedrawMode::UNBOUNDED;
            break;

        case NvRedrawMode::VSYNC:
            if (useChoreographer(true))
            {
                mRedrawMode = NvRedrawMode::VSYNC;
            }
            else
            {
                mRedrawMode = NvRedrawMode::UNBOUNDED;
            }
            break;

        case NvRedrawMode::ON_DEMAND:
            useChoreographer(false);
            mRedrawMode = NvRedrawMode::ON_DEMAND;
            break;
    };
}

void Engine::requestRedraw()
{ 
    if (mRedrawMode == NvRedrawMode::ON_DEMAND)
        mRedraw = true;
}

bool Engine::isContextLost()
{
    return !mWin->hasContext();
}

bool Engine::isContextBound()
{
    return mWin->isBound();
}

bool Engine::hasWindowResized()
{
    if (mWin->checkWindowResized())
    {
        requestForceRender();
        return true;
    }

    return false;
}

// If we cause an exception in JNI, we print the exception info to
// the log, we clear the exception to avoid a pending-exception
// crash, and we force the function to return.
#define EXCEPTION_RETURN(env) \
    if (env->ExceptionOccurred()) { \
        env->ExceptionDescribe(); \
        env->ExceptionClear(); \
        return false; \
    }

bool Engine::useChoreographer(bool use)
{
    jclass thisClass = mApp->appThreadEnv->GetObjectClass(mApp->appThreadThis);
    EXCEPTION_RETURN(mApp->appThreadEnv);

    jmethodID useChoreographer = mApp->appThreadEnv->GetMethodID(thisClass, 
        "useChoreographer", "(Z)Z");
    EXCEPTION_RETURN(mApp->appThreadEnv);

    bool success = mApp->appThreadEnv->CallBooleanMethod(mApp->appThreadThis, useChoreographer, use);
    EXCEPTION_RETURN(mApp->appThreadEnv);

    return true;
}


/**
 * Process the next input event.
 */
int32_t Engine::handleInputThunk(struct android_app* app, AInputEvent* event)
{
    Engine* engine = (Engine*)app->userData;
    if (!engine)
        return 0;

    return engine->handleInput(event);
}

/**
 * Process the next main command.
 */
void Engine::handleCmdThunk(struct android_app* app, int32_t cmd)
{
    Engine* engine = (Engine*)app->userData;
    if (!engine)
        return;

    engine->handleCommand(cmd);
}

static uint8_t mapAndroidCodeToChar(int32_t code) {
    // Only map alphanumeric characters (and only to caps) for now
    if (code >= AKEYCODE_A && code <= AKEYCODE_Z)
        return code - AKEYCODE_A + 'A';
    if (code >= AKEYCODE_0 && code <= AKEYCODE_9)
        return code - AKEYCODE_0 + '0';
    return 0;
}

/**
 * Helpers to match missing, newer motion processing functions in java.
 */
int32_t amotion_getActionMasked(AInputEvent* event) {
    int32_t action_raw = AMotionEvent_getAction(event);
    return (action_raw & AMOTION_EVENT_ACTION_MASK);
}

int32_t amotion_getActionIndex(AInputEvent* event) {
    int32_t action_raw = AMotionEvent_getAction(event);
       int32_t pointerIndex = (action_raw & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK);
    pointerIndex >>= AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    return pointerIndex;
}

/**
 * Process the next input event.
 */
int32_t Engine::handleInput(AInputEvent* event) {
	if (mCallbacks && mGamepad->processEvent(event, mPadChangedMask))
        return true;
    else if (NULL==mCallbacks)
        return false;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        bool handled = false;
        int32_t pointerCount = AMotionEvent_getPointerCount(event);
        int32_t action = amotion_getActionMasked(event);
        int32_t pointerIndex = amotion_getActionIndex(event);

        float x=0, y=0;
        NvPointerEvent p[20]; // !!!!!TBD  should use linkedlist or app-member struct or something?  TODO
        NvInputDeviceType::Enum dev = NvInputDeviceType::TOUCH;
        // loop over pointercount and copy data
        // !!!!TBD TODO, might need to ensure that p[0] is always the data from the first-finger-touched ID...
        for (int i=0; i<pointerCount; i++) {
            x = AMotionEvent_getX(event, i);
            y = AMotionEvent_getY(event, i);
            p[i].m_x = x;
            p[i].m_y = y;
            p[i].m_id = AMotionEvent_getPointerId(event, i);
            // then figure out tool/device enum...
            int32_t tool = AMotionEvent_getToolType(event, i);
            if (tool==AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                tool==AMOTION_EVENT_TOOL_TYPE_ERASER)//!!!!TBD TODO
                dev = NvInputDeviceType::STYLUS;
            else if (tool==AMOTION_EVENT_TOOL_TYPE_MOUSE)
                dev = NvInputDeviceType::MOUSE;
            else
                dev = NvInputDeviceType::TOUCH;
            // else we assume == FINGER... if unknown, treat as FINGER. TODO.
            p[i].m_device = dev;
        }

        mState.x = x;
        mState.y = y;
    
        NvPointerActionType::Enum pact;

        switch(action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            pact = NvPointerActionType::DOWN;
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            pact = NvPointerActionType::EXTRA_DOWN;
            break;
        case AMOTION_EVENT_ACTION_UP:
            pact = NvPointerActionType::UP;
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
            pact = NvPointerActionType::EXTRA_UP;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            pact = NvPointerActionType::MOTION;
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            pact = NvPointerActionType::UP;
            pointerCount = 0; // clear.
            break;
        }

        const int64_t timestamp = AMotionEvent_getEventTime((const AInputEvent*)event);
        handled = mCallbacks->pointerInput(dev, pact, 0, pointerCount, p, timestamp);
        // return code handling...
        if (pact==NvPointerActionType::UP)
            return 1;
        return handled ? 1 : 0;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        bool handled = false;
        int32_t source = AInputEvent_getSource(event);
        int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);
        int32_t action = AKeyEvent_getAction((const AInputEvent*)event);

        bool down = (action != AKEY_EVENT_ACTION_UP) ? true : false;

        if (mCallbacks) {
            handled = mCallbacks->keyInput(code, 
                down ? NvKeyActionType::DOWN : NvKeyActionType::UP);

            if (!handled && down) {
                uint8_t c = mapAndroidCodeToChar(code);
                if (c)
                    handled = mCallbacks->characterInput(c);
            }
        }

        return handled ? 1 : 0;
    }

    return 0;
}

/**
 * Process the next main command.
 */
void Engine::handleCommand(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_START:
            break;

        // The window is being shown, get it ready.
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_WINDOW_RESIZED:
            mWin->setWindow(mApp->window);
            break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            mWin->setWindow(NULL);
            break;

        case APP_CMD_GAINED_FOCUS:
            if (mAppBase) 
                mAppBase->focusChanged(true);
            requestForceRender();
            break;

        case APP_CMD_LOST_FOCUS:
            if (mAppBase) 
                mAppBase->focusChanged(false);
            requestForceRender();
            break;

        case APP_CMD_PAUSE:
            requestForceRender();
            break;

        // ICS does not appear to require this, but on GB phones,
        // not having this causes rotation changes to be delayed or
        // ignored when we're in a non-rendering mode like autopause.
        // The forced renders appear to allow GB to process the rotation
        case APP_CMD_CONFIG_CHANGED:
            requestForceRender();
            break;

        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            mApp->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)mApp->savedState) = mState;
            mApp->savedStateSize = sizeof(struct saved_state);
            break;

        case APP_CMD_DESTROY:
            break;
    }
}

bool Engine::isDisplayHDRCompatible() {
	if (mDisplayIsHDRCompatible == HDRDisplayState::Enum::UNCHECKED)
	{
		assert(mApp != NULL);

		jmethodID mid = 0;
		jclass thisClass = mApp->appThreadEnv->GetObjectClass(mApp->appThreadThis);
		if (mApp->appThreadEnv->ExceptionOccurred()) {
			mApp->appThreadEnv->ExceptionDescribe();
			mApp->appThreadEnv->ExceptionClear();
			return false;
		}

		if (thisClass != 0) {
			mid = mApp->appThreadEnv->GetMethodID(thisClass, "JavaCallback_isDisplayHDRCompatible", "()Z");
			if (mApp->appThreadEnv->ExceptionOccurred()) {
				mApp->appThreadEnv->ExceptionDescribe();
				mApp->appThreadEnv->ExceptionClear();
				return false;
			}

			mApp->appThreadEnv->DeleteLocalRef(thisClass);
		}

		jboolean result = mApp->appThreadEnv->CallBooleanMethod(mApp->appThreadThis, mid);

		if ((bool)result == true) {
			mDisplayIsHDRCompatible = HDRDisplayState::Enum::COMPATIBLE;
			LOGI("Connected Display is HDR Compatible")
		}
		else {
			mDisplayIsHDRCompatible = HDRDisplayState::Enum::INCOMPATIBLE;
			LOGI("Connected Display is not HDR Compatible")
		}
		return (bool)result;
	}
	else if (mDisplayIsHDRCompatible == HDRDisplayState::Enum::COMPATIBLE)
		return true;
	
	return false;
}

