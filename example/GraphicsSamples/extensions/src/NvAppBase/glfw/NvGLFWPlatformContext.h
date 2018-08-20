//----------------------------------------------------------------------------------
// File:        NvAppBase\glfw/NvGLFWPlatformContext.h
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
#ifndef NV_GLFW_PLATFORM_CONTEXT_H
#define NV_GLFW_PLATFORM_CONTEXT_H
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <GLFW/glfw3.h>

#include <NvGamepad/NvGamepad.h>

// this needs to be global so inputcallbacksglfw can access...
extern NvInputCallbacks* sCallbacks;
extern void setInputCallbacksGLFW(GLFWwindow *window);
#define NV_SAMPLE_WINDOW_TITLE_PREFIX "NVIDIA GameWorks Graphics Sample - "
class NvGLFWPlatformContext : public NvPlatformContext {
public:
	NvGLFWPlatformContext() :
		mWindow(NULL),
		mGamepad(NULL),
		mRenderOnDemand(false),
		mRenderRequested(false),
		mHasResized(true),
		mWindowIsFocused(true),
		mForcedRenderCount(0) {

		if (!glfwInit())
		{ // !!!!TBD TODO would be nice if showDialog could do these bits.
//			LOGE("Failed to initialize GLFW\n");
			exit(EXIT_FAILURE);
		}

		mGamepad = NvGamepad::createDefaultGamePad();
	}
	~NvGLFWPlatformContext() { 
		// Terminate GLFW
		glfwTerminate();

		delete mGamepad; 
	}

	void createWindow(uint32_t width, uint32_t height, bool fullscreen = false) {
		GLFWmonitor* monitor = NULL;
		if (fullscreen) {
			const GLFWvidmode* mode;

			monitor = glfwGetPrimaryMonitor();
			mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			width = mode->width;
			height = mode->height;
		}

		mWindow = glfwCreateWindow(width, height, NV_SAMPLE_WINDOW_TITLE_PREFIX, monitor, NULL);
		if (!mWindow)
		{
//			LOGE("Failed to open GLFW window\n");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		setInputCallbacksGLFW(mWindow);
		glfwSetWindowUserPointer(mWindow, this);

		// Set callback functions
		glfwSetFramebufferSizeCallback(mWindow, reshape);
		glfwSetWindowFocusCallback(mWindow, focus);
	}

	void destroyWindow() {
		if (mWindow)
		{
			glfwDestroyWindow(mWindow);
			mWindow = NULL;
		}
	}

	virtual bool isAppRunning();
	virtual void requestExit() { glfwSetWindowShouldClose(mWindow, 1); }
	virtual bool pollEvents(NvInputCallbacks* callbacks);
	virtual bool isContextLost() { return false; }
	virtual bool isContextBound() { return glfwGetCurrentContext() != NULL; }
	virtual bool shouldRender();
	virtual bool hasWindowResized();
	virtual NvGamepad* getGamepad() { return mGamepad; }

	virtual void setAppTitle(const char* title) {
		if (mWindow) {
			std::string windowTitle(NV_SAMPLE_WINDOW_TITLE_PREFIX);
			windowTitle += title;
			glfwSetWindowTitle(mWindow, windowTitle.c_str());
		}
	}

	virtual NvRedrawMode::Enum getRedrawMode() { return mRenderOnDemand ? NvRedrawMode::ON_DEMAND : NvRedrawMode::UNBOUNDED; }
	virtual void setRedrawMode(NvRedrawMode::Enum mode) {
		mRenderOnDemand = (mode == NvRedrawMode::ON_DEMAND);
		if (mRenderOnDemand)
			requestRedraw();
	}
	virtual void requestRedraw() { mRenderRequested = true; }

	void updateWindowSize() {
		int32_t width = 0;
		int32_t height = 0;
		glfwGetFramebufferSize(mWindow, &width, &height);
		reshape(mWindow, width, height);
	}

	GLFWwindow* getGLFWWindow() { return mWindow; }

	void setWindowResized(bool resized = true);

protected:
	static void reshape(GLFWwindow* window, int32_t width, int32_t height);
	static void focus(GLFWwindow* window, int32_t focused);

	GLFWwindow* mWindow;
	NvGamepad* mGamepad;
	bool mRenderOnDemand;
	bool mRenderRequested;
	bool mHasResized;
	bool mWindowIsFocused;
	int32_t mForcedRenderCount;
};


inline bool NvGLFWPlatformContext::isAppRunning() {
	return !glfwWindowShouldClose(mWindow);
}

inline bool NvGLFWPlatformContext::pollEvents(NvInputCallbacks* callbacks) {
	sCallbacks = callbacks;
	glfwPollEvents();

	if (mGamepad) {
		uint32_t changed = mGamepad->pollGamepads();
		if (callbacks && changed)
			callbacks->gamepadChanged(changed);
	}

	sCallbacks = NULL;

	return true;
}

inline bool NvGLFWPlatformContext::shouldRender() {
	// For now, do not stop rendering when focus is lost; this is screwing up test mode
	/*if (sWindowIsFocused || (sForcedRenderCount > 0))*/ {
		if (mForcedRenderCount > 0)
			mForcedRenderCount--;

		if (!mRenderOnDemand || mRenderRequested) {
			mRenderRequested = false;
			return true;
		}
		else {
			return false;
		}
	}
	//return false;
}

bool NvGLFWPlatformContext::hasWindowResized() {
	if (mHasResized) {
		mHasResized = false;
		return true;
	}
	return false;
}

void NvGLFWPlatformContext::setWindowResized(bool resized) {
	mHasResized = resized;
}

// window size callback
void NvGLFWPlatformContext::reshape(GLFWwindow* window, int32_t width, int32_t height)
{
	NvGLFWPlatformContext* pThis = (NvGLFWPlatformContext*)glfwGetWindowUserPointer(window);
	pThis->mHasResized = true;
	pThis->mForcedRenderCount += 2;
}

void NvGLFWPlatformContext::focus(GLFWwindow* window, int32_t focused)
{
	NvGLFWPlatformContext* pThis = (NvGLFWPlatformContext*)glfwGetWindowUserPointer(window);
	pThis->mWindowIsFocused = focused != 0;
	// HACK - takes advantage of the fact that focus is only handled on Android...
//	sApp->focusChanged(pThis->mWindowIsFocused);
	pThis->mForcedRenderCount += 2;
}




#endif
