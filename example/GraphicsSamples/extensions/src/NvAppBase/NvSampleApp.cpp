//----------------------------------------------------------------------------------
// File:        NvAppBase/NvSampleApp.cpp
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
#include "NvAppBase/NvSampleApp.h"
#include "NV/NvLogs.h"
#include "NvAppBase/NvFramerateCounter.h"
#include "NvAppBase/NvInputTransformer.h"
#include "NvImage/NvImage.h"
#include "NvUI/NvGestureDetector.h"
#include "NvUI/NvTweakBar.h"
#include "NV/NvString.h"
#include "NV/NvTokenizer.h"
#include "NvAppBase/NvInputHandler.h"

#include <NsAllocator.h>
#include <NsIntrinsics.h>

#include <stdarg.h>
#include <sstream>

NvSampleApp::NvSampleApp() :
	NvAppBase()
	, mFramerate(0L)
	, mFrameDelta(0.0f)
	, mUIWindow(0L)
	, mFPSText(0L)
	, mEnableFPS(true)
	, mTweakBar(0L)
	, mTweakTab(0L)
	, m_desiredWidth(0)
	, m_desiredHeight(0)
	, mTestMode(false)
	, mTestDuration(0.0f)
	, mTestRepeatFrames(1)
	, m_testModeIssues(TEST_MODE_ISSUE_NONE)
	, mEnableInputCallbacks(true)
	, mUseRenderThread(false)
	, mThread(NULL)
	, mRenderThreadRunning(false)
	, mUseFBOPair(false)
    , m_fboWidth(0)
    , m_fboHeight(0)
	, m_inputHandler(NULL)
	, mLogFPS(false)
	, mTimeSinceFPSLog(0.0f)
{
    m_transformer = new NvInputTransformer;
    memset(mLastPadState, 0, sizeof(mLastPadState));

    mFrameTimer = createStopWatch();

    mEventTickTimer = createStopWatch();

    mAutoRepeatTimer = createStopWatch();
    mAutoRepeatButton = 0; // none yet! :)
    mAutoRepeatTriggered = false;

    const std::vector<std::string>& cmd = getCommandLine();
    std::vector<std::string>::const_iterator iter = cmd.begin();

    while (iter != cmd.end()) {
        if (0==(*iter).compare("-w")) {
            iter++;
            std::stringstream(*iter) >> m_desiredWidth;
        } else if (0==(*iter).compare("-h")) {
            iter++;
            std::stringstream(*iter) >> m_desiredHeight;
        } else if (0==(*iter).compare("-testmode")) {
            mTestMode = true;
            iter++;
            std::stringstream(*iter) >> mTestDuration;
            iter++;
            mTestName = (*iter); // both std::string
        } else if (0 == (*iter).compare("-repeat")) {
            iter++;
            std::stringstream(*iter) >> mTestRepeatFrames;
        } else if (0 == (*iter).compare("-fbo")) {
            mUseFBOPair = true;
            iter++;
            std::stringstream(*iter) >> m_fboWidth;
            iter++;
            std::stringstream(*iter) >> m_fboHeight;
		} else if (0 == (*iter).compare("-logfps")) {
			mLogFPS = true;
		}
		
        iter++;
    }

    nvidia::shdfnd::initializeNamedAllocatorGlobals();
    mThread = NULL;
    mRenderSync = new nvidia::shdfnd::Sync;
    mMainSync = new nvidia::shdfnd::Sync;
}

NvSampleApp::~NvSampleApp() 
{ 
    // clean up internal allocs
    delete mFrameTimer;
    delete mEventTickTimer;
    delete mAutoRepeatTimer;

    delete m_transformer;
}

bool NvSampleApp::baseInitRendering(void) {
    if (mUseFBOPair)
        mUseFBOPair = getAppContext()->useOffscreenRendering(m_fboWidth, m_fboHeight);

	getAppContext()->contextInitRendering();

	if (!platformInitRendering())
		return false;
    initRendering();
    baseInitUI();

	return true;
}

void NvSampleApp::baseInitUI(void) {
    // safe to now pass through title to platform layer...
    if (!mAppTitle.empty())
        mPlatform->setAppTitle(mAppTitle.c_str());
    
    platformInitUI();

    if (!mUIWindow) {
        const int32_t w = getAppContext()->width(), h = getAppContext()->height();

        mUIWindow = new NvUIWindow((float)w, (float)h);

		if (mEnableFPS)
		{
        mFPSText = new NvUIValueText("", NvUIFontFamily::SANS, w/40.0f, NvUITextAlign::RIGHT,
                                    0.0f, 1, NvUITextAlign::RIGHT);
		mFPSText->SetColor(NV_PACKED_COLOR(192, 192, 64, 255));
        mFPSText->SetShadow();
        mUIWindow->Add(mFPSText, (float)w-8, 0);
		}
        

        if (mTweakBar==NULL) {
            mTweakBar = NvTweakBar::CreateTweakBar(mUIWindow); // adds to window internally.
            mTweakBar->SetVisibility(false);

            if (!mAppTitle.empty()) {
                mTweakBar->addLabel(mAppTitle.c_str(), true);
                mTweakBar->addPadding();
            }

            // for now, app will own the tweakbar tab button
            float high = mTweakBar->GetDefaultLineHeight();
            NvUIElement *els[3];
            els[0] = new NvUIGraphic("arrow_blue.dds");
            els[0]->SetDimensions(high/2, high/2);
            els[1] = new NvUIGraphic("arrow_blue_left.dds");
            els[1]->SetDimensions(high/2, high/2);
            els[2] = NULL;

            mTweakTab = new NvUIButton(NvUIButtonType::CHECK, TWEAKBAR_ACTIONCODE_BASE, els);
            mTweakTab->SetHitMargin(high/2, high/2);
            mTweakTab->SetReactOnPress(true); // do its reaction on the press, eff ignoring release.
            mUIWindow->Add(mTweakTab, high*0.25f, mTweakBar->GetStartOffY()+high*0.125f);
        }
    }
    
    initUI();
}

void NvSampleApp::baseReshape(int32_t w, int32_t h) {
    getAppContext()->platformReshape(w, h);

    if ((w == m_width) && (h == m_height))
        return;

    m_width = w;
    m_height = h;

    if (mUIWindow)
        mUIWindow->HandleReshape((float)w, (float)h);
	if (m_inputHandler)
	{
		m_inputHandler->setScreenSize(w, h);
	}
	else
	{
		m_transformer->setScreenSize(w, h);
	}

    reshape(w, h);
}

void NvSampleApp::baseUpdate(void) {
    update();
}

void NvSampleApp::baseDraw(void) {
    draw();
}

void NvSampleApp::baseDrawUI(void) {

    if (mUIWindow && mUIWindow->GetVisibility()) {
        if (mFPSText) {
#if 1
            mFPSText->SetValue(mFramerate->getMeanFramerate());
#else
            char str[256];
            sprintf(str, "%.3f (%.3f)", mFramerate->getMeanFramerate(), mDrawRate); 
            mFPSText->SetString(str);
#endif
        }
        NvUST time = 0;
        NvUIDrawState ds(time, getAppContext()->width(), getAppContext()->height());
        mUIWindow->Draw(ds);
    }

    drawUI();
}

void NvSampleApp::baseHandleReaction() {
    NvUIEventResponse r;
    if (!mUIWindow) return;

    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    const NvUIReaction& react = NvUIElement::GetReaction();
    // we let the UI handle any reaction first, in case there
    // are interesting side-effects such as updating variables...
    r = mUIWindow->HandleReaction(react);
    // then the app is always given a look, even if already handled...
    //if (r==nvuiEventNotHandled)
    r = handleReaction(react);
}

void NvSampleApp::syncValue(NvTweakVarBase *var) {
    if (!mUIWindow) return;
    NvUIReaction &react = mUIWindow->GetReactionEdit(true);
    react.code = var->getActionCode();
    react.flags = NvReactFlag::FORCE_UPDATE;
    baseHandleReaction();
}


bool NvSampleApp::handleGestureEvents()
{
    bool wasHandled = false;
    if (!mUIWindow) return wasHandled;

    NvGestureQueueEvent *gqe;
    while (NULL != (gqe = NvGestureGetNextEvent())) {
        NvUIEventResponse r = mUIWindow->HandleEvent(gqe->gesture, gqe->time, NULL);
        NvFreeGQEvent(gqe); // must free after processing.

        // if we handled an event
        if (r&nvuiEventHandled) {
            // flag that we did...
            wasHandled = true;
            if (r&nvuiEventHadReaction)
                baseHandleReaction();
        }
    }

    return wasHandled;
}

bool NvSampleApp::pointerInput(NvInputDeviceType::Enum device, NvPointerActionType::Enum action, 
    uint32_t modifiers, int32_t count, NvPointerEvent* points, int64_t timestamp) {

    // In on-demand rendering mode, we trigger a redraw on any input
    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    // if have window, setup up and pass input events along as processed gestures
    if (mUIWindow!=NULL) {
        static bool isDown = false;
        bool isButtonEvent = (action==NvPointerActionType::DOWN)||(action==NvPointerActionType::UP);
        if (isButtonEvent)
            isDown = (action==NvPointerActionType::DOWN);

        NvInputEventClass::Enum giclass = NvInputEventClass::MOUSE; // default to mouse
        if (device==NvInputDeviceType::STYLUS) giclass = NvInputEventClass::STYLUS;
        else if (device==NvInputDeviceType::TOUCH) giclass = NvInputEventClass::TOUCH;
        
        // need a better way to do this, should be part of the input, in case of multiple inputs at same time.
        NvGestureSetInputType(giclass);
        
        // remap nvpointer to nvgesture.  in the future, we should do higher up in framework.
        NvGestureInputData gidata;
        memset(&gidata, 0, sizeof(gidata));
        if (isDown) gidata.countDown = (uint8_t)count;
        gidata.x = (int32_t)points[0].m_x;
        gidata.y = (int32_t)points[0].m_y;
        if (count>1) {
            gidata.x2 = (int32_t)points[1].m_x;
            gidata.y2 = (int32_t)points[1].m_y;
        }

        // reset our tick timer, which allows us to generate gestures in mainloop.
        mEventTickTimer->start();

        // add current input to gesture system
        NvGestureAddInput(timestamp, &gidata);

        // let the UI system try to handle...
        if (handleGestureEvents())
            return true; // we handled something, so don't pass pointer to app/framework.
    }

    // if UI system didn't handle, we pass pointer inside the app framework.
    // TODO: might add support for passing gesture events instead.
    if (handlePointerInput(device, action, modifiers, count, points))
        return true;
	if (m_inputHandler)
	{
		return m_inputHandler->processPointer(device, action, modifiers, count, points);
	}
	else
	{
		return m_transformer->processPointer(device, action, modifiers, count, points);
	}
}


void NvSampleApp::addTweakKeyBind(NvTweakVarBase *var, uint32_t incKey, uint32_t decKey/*=0*/) {
    mKeyBinds[incKey] = NvTweakBind(NvTweakCmd::INCREMENT, var);
    if (decKey)
        mKeyBinds[decKey] = NvTweakBind(NvTweakCmd::DECREMENT, var);
}

bool NvSampleApp::keyInput(uint32_t code, NvKeyActionType::Enum action) {
    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    // only do down and repeat for now.
    if (NvKeyActionType::UP!=action) {
        NvAppKeyBind::const_iterator bind = mKeyBinds.find(code);
        if (bind != mKeyBinds.end()) {
            // we have a binding.  do something with it.
            NvTweakVarBase *var = bind->second.mVar;
            if (var) {
                switch (bind->second.mCmd) {
                    case NvTweakCmd::RESET:
                        var->reset();
                        break;
                    case NvTweakCmd::INCREMENT:
                        var->increment();
                        break;
                    case NvTweakCmd::DECREMENT:
                        var->decrement();
                        break;
                    default:
                        return false;
                }

                syncValue(var);
                // we're done.
                return true;
            }
        }
    }

    if (mTweakBar && NvKeyActionType::UP!=action) // handle down+repeat as needed
    {
        // would be nice if this was some pluggable class we could add/remove more easily like inputtransformer.
        NvUIEventResponse r = nvuiEventNotHandled;
        switch(code)
        {
            case NvKey::K_TAB: {
                if (!mUIWindow || NvKeyActionType::DOWN!=action) break; // we don't want autorepeat...
                NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                react.code = TWEAKBAR_ACTIONCODE_BASE;
                react.state = mTweakBar->GetVisibility() ? 0 : 1;
                r = nvuiEventHandledReaction;
                break;
            }
            case NvKey::K_ARROW_DOWN: {
                if (!mUIWindow || NvKeyActionType::DOWN!=action) break; // we don't want autorepeat...
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::MOVE_DOWN);
                break;
            }
            case NvKey::K_ARROW_UP: {
                if (!mUIWindow || NvKeyActionType::DOWN!=action) break; // we don't want autorepeat...
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::MOVE_UP);
                break;
            }
            case NvKey::K_ENTER: {
                if (!mUIWindow || NvKeyActionType::DOWN!=action) break; // we don't want autorepeat...
                // handle ATV support...
                if (mTweakBar && !mTweakBar->GetVisibility()) {
                    // then show tweakbar.
                    NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                    react.code = TWEAKBAR_ACTIONCODE_BASE;
                    react.state = 1;
                    r = nvuiEventHandledReaction;
                } else {
                    // handle as normal in UI system...
                    r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_PRESS);
                }
                break;
            }
            case NvKey::K_BACK: // fall through
            case NvKey::K_BACKSPACE: {
                if (!mUIWindow || NvKeyActionType::DOWN!=action) break; // we don't want autorepeat...
                // handle ATV support...
                if (mUIWindow->HasFocus()) {
                    r = mUIWindow->HandleFocusEvent(NvFocusEvent::FOCUS_CLEAR);
                } else if (mTweakBar && mTweakBar->GetVisibility()) {
                    // then hide tweakbar.
                    NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                    react.code = TWEAKBAR_ACTIONCODE_BASE;
                    react.state = 0;
                    r = nvuiEventHandledReaction;
                } else if (NvKey::K_BACK==code) {
                    // explicit exit for the BACK button.
                    r = nvuiEventHandled;
                    appRequestExit();
                }
                break;
            }
            case NvKey::K_ARROW_LEFT: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_DEC);
                break;
            }
            case NvKey::K_ARROW_RIGHT: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_INC);
                break;
            }
            default:
                break;
        }

        if (r&nvuiEventHandled) 
        {
            if (r&nvuiEventHadReaction)
                baseHandleReaction();
            return true;
        }
    }
        
    if (handleKeyInput(code, action))
        return true;

	// give last shot to transformer.
	if (m_inputHandler)
	{
		return m_inputHandler->processKey(code, action);
	}
	else
	{
		return m_transformer->processKey(code, action);
	}
}

bool NvSampleApp::characterInput(uint8_t c) {
    // In on-demand rendering mode, we trigger a redraw on any input
    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    if (handleCharacterInput(c))
        return true;
    return false;
}

void NvSampleApp::addTweakButtonBind(NvTweakVarBase *var, uint32_t incBtn, uint32_t decBtn/*=0*/) {
    mButtonBinds[incBtn] = NvTweakBind(NvTweakCmd::INCREMENT, var);
    if (decBtn)
        mButtonBinds[decBtn] = NvTweakBind(NvTweakCmd::DECREMENT, var);
}


bool NvSampleApp::gamepadButtonChanged(uint32_t button, bool down) {
    if (mAutoRepeatButton == button && !down) {
        mAutoRepeatButton = 0;
        mAutoRepeatTriggered = false;
        mAutoRepeatTimer->stop();
    }

    // In on-demand rendering mode, we trigger a redraw on any input
    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    // currently, we only react on the button DOWN
    if (down) {
        NvAppButtonBind::const_iterator bind = mButtonBinds.find(button);
        if (bind != mButtonBinds.end()) {
            // we have a binding.  do something with it.
            NvTweakVarBase *var = bind->second.mVar;
            if (var) {
                switch (bind->second.mCmd) {
                    case NvTweakCmd::RESET:
                        var->reset();
                        break;
                    case NvTweakCmd::INCREMENT:
                        var->increment();
                        break;
                    case NvTweakCmd::DECREMENT:
                        var->decrement();
                        break;
                    default:
                        return false;
                }

                syncValue(var);
                // we're done.
                return true;
            }
        }
    }

    if (down && mTweakBar) {
        // would be nice if this was some pluggable class we could add/remove more easily like inputtransformer.
        NvUIEventResponse r = nvuiEventNotHandled;
        switch(button) {
            case NvGamepad::BUTTON_PLAY_PAUSE: // fall through; ATV handles playpause==start
            case NvGamepad::BUTTON_START: {
                if (!mUIWindow) break;
                NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                react.code = TWEAKBAR_ACTIONCODE_BASE;
                react.state = mTweakBar->GetVisibility() ? 0 : 1;
                r = nvuiEventHandledReaction;
                break;
            }
            case NvGamepad::BUTTON_DPAD_DOWN: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::MOVE_DOWN);
                break;
            }
            case NvGamepad::BUTTON_DPAD_UP: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::MOVE_UP);
                break;
            }
            case NvGamepad::BUTTON_CENTER: // fall through; ATV handles CENTER==A
            case NvGamepad::BUTTON_A: {
                if (!mUIWindow) break;
                // handle ATV support...
                if (mTweakBar && !mTweakBar->GetVisibility()) {
                    // then show tweakbar.
                    NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                    react.code = TWEAKBAR_ACTIONCODE_BASE;
                    react.state = 1;
                    r = nvuiEventHandledReaction;
                } else {
                    r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_PRESS);
                }
                break;
            }
            case NvGamepad::BUTTON_BACK: // fall through
            case NvGamepad::BUTTON_B: {
                if (!mUIWindow) break;
                // handle ATV support...
                if (mUIWindow->HasFocus()) {
                    r = mUIWindow->HandleFocusEvent(NvFocusEvent::FOCUS_CLEAR);
                } else if (mTweakBar && mTweakBar->GetVisibility()) {
                    // then hide tweakbar.
                    NvUIReaction &react = mUIWindow->GetReactionEdit(true);
                    react.code = TWEAKBAR_ACTIONCODE_BASE;
                    react.state = 0;
                    r = nvuiEventHandledReaction;
                } else if (NvGamepad::BUTTON_BACK==button) {
                    // explicit exit for the BACK button.
                    r = nvuiEventHandled;
                    appRequestExit();
                }
                break;
            }
            case NvGamepad::BUTTON_DPAD_LEFT: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_DEC);
                mAutoRepeatTimer->start();
                mAutoRepeatButton = button;
                break;
            }
            case NvGamepad::BUTTON_DPAD_RIGHT: {
                if (!mUIWindow) break;
                r = mUIWindow->HandleFocusEvent(NvFocusEvent::ACT_INC);
                mAutoRepeatTimer->start();
                mAutoRepeatButton = button;
                break;
            }
            default: break;
        }

        if (r&nvuiEventHandled) 
        {
            if (r&nvuiEventHadReaction)
                baseHandleReaction();
            return true;
        }
    }

	// let apps have a shot AFTER we intercept framework controls.
	if (handleGamepadButtonChanged(button, down))
		return true;

    return false;
}

bool NvSampleApp::gamepadChanged(uint32_t changedPadFlags) {
    // In on-demand rendering mode, we trigger a redraw on any input
    if (mPlatform->getRedrawMode() == NvRedrawMode::ON_DEMAND)
        mPlatform->requestRedraw();

    if (handleGamepadChanged(changedPadFlags))
        return true;

    if (!changedPadFlags)
        return false;

    NvGamepad* pad = getPlatformContext()->getGamepad();
    if (!pad) return false;

    NvGamepad::State state;
    int32_t i, j;
    uint32_t button;
    bool buttonDown;
    for (i = 0; i < NvGamepad::MAX_GAMEPADS; i++) {
        if (changedPadFlags & (1<<i)) {
            pad->getState(i, state);
            if (state.mButtons != mLastPadState[i].mButtons) {
                // parse through the buttons and send events.
                for (j=0; j<32; j++) { // iterate button bits
                    button = 1<<j;
                    buttonDown = (button & state.mButtons)>0;
                    if (buttonDown != ((button & mLastPadState[i].mButtons)>0))
                        gamepadButtonChanged(button, buttonDown);
                }
            }
            // when done processing a gamepad, copy off the state.
            memcpy(mLastPadState+i, &state, sizeof(state));
        }
    }

	// give last shot to transformer.  not sure how we 'consume' input though.
	if (m_inputHandler)
	{
		return m_inputHandler->processGamepad(changedPadFlags, *pad);
	}
	else
	{
		return m_transformer->processGamepad(changedPadFlags, *pad);
	}
}

void NvSampleApp::initRenderLoopObjects() {
    mTestModeTimer = createStopWatch();
    mTestModeFrames = -TESTMODE_WARMUP_FRAMES;
    mTotalTime = -1e6f; // don't exit during startup

    mFramerate = new NvFramerateCounter(this);

    mFrameTimer->start();

    mSumDrawTime = 0.0f;
    mDrawTimeFrames = 0;
    mDrawRate = 0.0f;
    mDrawTime = createStopWatch();
}

void NvSampleApp::shutdownRenderLoopObjects() {
    if (mHasInitializedRendering) {
        baseShutdownRendering();
        mHasInitializedRendering = false;
    }

    delete mFramerate;
    mFramerate = NULL;
}

void NvSampleApp::renderLoopRenderFrame() {
    mFrameTimer->stop();

    if (mTestMode) {
        // Simulate 60fps
        mFrameDelta = 1.0f / 60.0f;

        // just an estimate
        mTotalTime += mFrameTimer->getTime();
    } else {
        mFrameDelta = mFrameTimer->getTime();
        // just an estimate
        mTotalTime += mFrameDelta;
    }
	if (m_inputHandler)
	{
		m_inputHandler->update(mFrameDelta);
	}
	else
	{
		m_transformer->update(mFrameDelta);
	}
	mFrameTimer->reset();
	
	if (m_width == 0 || m_height == 0) {
		NvThreadManager* thread = getThreadManagerInstance();

		if (thread)
			thread->sleepThread(200);

		return;
	}
	
	// initialization may cause the app to want to exit
    if (!isExiting()) {
        mFrameTimer->start();

        if (mEventTickTimer->getTime()>=0.05f) {
            mEventTickTimer->start(); // reset and continue...
            if (NvGestureTick(NvTimeGetTime()))
                handleGestureEvents();
        }

        // Handle automatic repeating buttons.
        if (mAutoRepeatButton) {
            const float elapsed = mAutoRepeatTimer->getTime();
            if ( (!mAutoRepeatTriggered && elapsed >= 0.5f) ||
                    (mAutoRepeatTriggered && elapsed >= 0.04f) ) { // 25hz repeat
                mAutoRepeatTriggered = true;
                gamepadButtonChanged(mAutoRepeatButton, true);
            }
        }

        mDrawTime->start();

		getAppContext()->beginFrame();

		getAppContext()->beginScene();
		baseDraw();

        if (mTestMode && (mTestRepeatFrames > 1)) {
            // repeat frame so that we can simulate a heavier workload
            for (int i = 1; i < mTestRepeatFrames; i++) {
                baseUpdate();
				if (m_inputHandler)
				{
					m_inputHandler->update(mFrameDelta);
				}
				else
				{
					m_transformer->update(mFrameDelta);
				}
				baseDraw();
            }
        }

		getAppContext()->endScene();

		if (!mTestMode) {
			baseDrawUI();
		}

		if (mTestMode && mUseFBOPair) {
			// Check if the app bound FBO 0 in FBO mode
			if (getAppContext()->isRenderingToMainScreen())
				m_testModeIssues |= TEST_MODE_FBO_ISSUE;
		}

		getAppContext()->endFrame();

        mDrawTime->stop();
        mSumDrawTime += mDrawTime->getTime();
        mDrawTime->reset();

        mDrawTimeFrames++;
        if (mDrawTimeFrames > 10) {
            mDrawRate = mDrawTimeFrames / mSumDrawTime;
            mDrawTimeFrames = 0;
            mSumDrawTime = 0.0f;
        }

		mFramerate->nextFrame();

		if (mLogFPS) {
			// wall time - not (possibly) simulated time
			mTimeSinceFPSLog += mFrameTimer->getTime();

			if (mTimeSinceFPSLog > 1.0f) {
				LOGI("fps: %.2f", mFramerate->getMeanFramerate());
				mTimeSinceFPSLog = 0.0f;
			}
        }
    }

    if (mTestMode) {
        mTestModeFrames++;
        // if we've come to the end of the warm-up, start timing
        if (mTestModeFrames == 0) {
            mTotalTime = 0.0f;
            mTestModeTimer->start();
        }

        if (mTotalTime > mTestDuration) {
            mTestModeTimer->stop();
            double frameRate = mTestModeFrames / mTestModeTimer->getTime();
            logTestResults((float)frameRate, mTestModeFrames);
            exit(0);
//                    appRequestExit();
        }
    }
}

bool NvSampleApp::haltRenderingThread() {
    // DO NOT test whether we WANT threading - the app may have just requested
    // threaded rendering to be disabled.
    // If threaded:
    // 1) Signal the rendering thread to exit
    if (mThread) {
        mRenderSync->set();
        mThread->signalQuit();
        // 2) Wait for the thread to complete (it will unbind the context), if it is running
        if (mThread->waitForQuit()) {
            // 3) Bind the context (unless it is lost?)
            getAppContext()->bindContext();
        }
        NV_DELETE_AND_RESET(mThread);
    }

    return true;
}

void* NvSampleApp::renderThreadThunk(void* thiz) {
    ((NvSampleApp*)thiz)->renderThreadFunc();
    return NULL;
}

void NvSampleApp::renderThreadFunc() {
    getAppContext()->prepThreadForRender();

    getAppContext()->bindContext();

    nvidia::shdfnd::memoryBarrier();
    mMainSync->set();

    while (mThread && !mThread->quitIsSignalled()) {
        renderLoopRenderFrame();

        // if we are not in full-bore rendering mode, wait to be triggered
        if (getPlatformContext()->getRedrawMode() != NvRedrawMode::UNBOUNDED) {
            mRenderSync->wait();
            mRenderSync->reset();
        }
    }

    getAppContext()->unbindContext();
    mRenderThreadRunning = false;
}

bool NvSampleApp::conditionalLaunchRenderingThread() {
    if (mUseRenderThread) {
        if (!mRenderThreadRunning) {
            // If threaded and the render thread is not running:
            // 1) Unbind the context
            getAppContext()->unbindContext();
            // 2) Call the thread launch function (which will bind the context)
            mRenderThreadRunning = true;
            mThread = NV_NEW(nvidia::shdfnd::Thread)(renderThreadThunk, this);

            // 3) WAIT for the rendering thread to bind or fail
            mMainSync->wait();
            mMainSync->reset();
        }

        // In any of the "triggered" modes, trigger the rendering thread loop
        if (getPlatformContext()->getRedrawMode() != NvRedrawMode::UNBOUNDED) {
            mRenderSync->set();
        }
        return true;
    } else {
        haltRenderingThread();

        // return false if we are not running in threaded mode or
        // _CANNOT_ support threading
        return false;
    }
}


void NvSampleApp::mainThreadRenderStep() {
    NvPlatformContext* ctx = getPlatformContext();
    bool needsReshape = false;

    // If the context has been lost and graphics resources are still around,
    // signal for them to be deleted
    if (ctx->isContextLost()) {
        if (mHasInitializedRendering) {
            haltRenderingThread();

            baseShutdownRendering();
            mHasInitializedRendering = false;
        }
    }

    // If we're ready to render (i.e. the GL is ready and we're focused), then go ahead
    if (ctx->shouldRender()) {
        // If we've not (re-)initialized the resources, do it
        if (!mHasInitializedRendering && !isExiting()) {
			mHasInitializedRendering = baseInitRendering();
            needsReshape = true;

            // In test mode, disable VSYNC if possible
            if (mTestMode)
                getAppContext()->setSwapInterval(0);
        } else if (ctx->hasWindowResized()) {
            haltRenderingThread();

            if (mUIWindow) {
                const int32_t w = getAppContext()->width(), h = getAppContext()->height();
                mUIWindow->HandleReshape((float)w, (float)h);
            }

            needsReshape = true;
        }

        // initialization may cause the app to want to exit, so test exiting
        if (needsReshape && !isExiting()) {
            baseReshape(getAppContext()->width(), getAppContext()->height());
        }

        // if we're not threaded or if the thread failed to launch - render here
        if (!conditionalLaunchRenderingThread())
            renderLoopRenderFrame();
    }
}

void NvSampleApp::requestThreadedRendering(bool threaded) {
    mUseRenderThread = threaded;
}

bool NvSampleApp::isRenderThreadRunning() {
    return mRenderThreadRunning;
}

void NvSampleApp::mainLoop() {

    if (mTestMode) {
        writeLogFile(mTestName, false, "*** Starting Test\n");
    }

    mHasInitializedRendering = false;

    initRenderLoopObjects();

    // TBD - WAR for Android lifecycle change; this will be reorganized in the next release
#ifdef ANDROID
    while (getPlatformContext()->isAppRunning()) {
#else
    while (getPlatformContext()->isAppRunning() && !isExiting()) {
#endif
        getPlatformContext()->pollEvents(isAppInputHandlingEnabled() ? this : NULL);

        baseUpdate();

        mainThreadRenderStep();
    }

    haltRenderingThread();

    shutdownRenderLoopObjects();

    // mainloop exiting, clean up things created in mainloop lifespan.
}

void NvSampleApp::errorExit(const char* errorString) {
    if (mTestMode) {
        writeLogFile(mTestName, true, "Fatal Error from app\n");
        writeLogFile(mTestName, true, errorString);
        appRequestExit();
    } else {
        // we set the flag here manually.  The exit will not happen until
        // the user closes the dialog.  But we want to act as if we are
        // already exiting (which we are), so we do not render
        m_requestedExit = true;
        showDialog("Fatal Error", errorString, true);
    }
}

bool NvSampleApp::getRequestedWindowSize(int32_t& width, int32_t& height) {
    bool changed = false;
    if (m_desiredWidth != 0) {
        width = m_desiredWidth;
        changed = true;
    }

    if (m_desiredHeight != 0) {
        height = m_desiredHeight;
        changed = true;
    }

    return changed;
}

void NvSampleApp::baseShutdownRendering(void) {
    platformShutdownRendering();

    // clean up UI elements.
    delete mUIWindow; // note it holds all our UI, so just null other ptrs.
    mUIWindow = NULL;
    mFPSText = NULL;
    mTweakBar = NULL;
    mTweakTab = NULL;

    shutdownRendering();
}

void NvSampleApp::logTestResults(float frameRate, int32_t frames) {
    LOGI("Test Frame Rate = %lf (frames = %d) (%d x %d)\n", frameRate, frames, m_width, m_height);
    writeLogFile(mTestName, true, "\n%s %lf fps (%d frames) (%d x %d)\n", mTestName.c_str(), 
		frameRate, frames, m_width, m_height);

    if (m_testModeIssues != TEST_MODE_ISSUE_NONE) {
        writeLogFile(mTestName, true, "\nWARNING - there were potential test mode anomalies\n");

        if (m_testModeIssues & TEST_MODE_FBO_ISSUE) {
            writeLogFile(mTestName, true, "\tThe application appears to have explicitly bound the onscreen framebuffer\n"
                "\tSince the test was being run in offscreen rendering mode, this could invalidate results\n"
                "\tThe application should be checked for glBindFramebuffer of 0\n\n");
        }
    }
    platformLogTestResults(frameRate, frames);

    int32_t w = 1, h = 1;

    if (!getAppContext()->readFramebufferRGBX32(NULL, w, h)) {
        writeLogFile(mTestName, true, "\tThe application appears to have written to the onscreen framebuffer\n"
            "\tSince the test was being run in offscreen rendering mode, this could invalidate results\n"
            "\tThe application should be checked for glBindFramebuffer of 0\n\n");
    }

    uint8_t* data = new uint8_t[4 * m_width * m_height];

    getAppContext()->readFramebufferRGBX32(data, w, h);

    writeScreenShot(m_width, m_height, data, mTestName);
    writeLogFile(mTestName, true, "Test Complete!");

    delete[] data;
}
