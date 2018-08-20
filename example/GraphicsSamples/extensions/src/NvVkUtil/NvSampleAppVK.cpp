//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvSampleAppVK.cpp
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
#include "NvSampleAppVK.h"
#include "NvAppWrapperContextVK.h"
#include "NV/NvLogs.h"
#include "NvAppBase/NvFramerateCounter.h"
#include "NvAppBase/NvInputTransformer.h"
#include "NvImage/NvImage.h"
#include "NvUI/NvGestureDetector.h"
#include "NvUI/NvTweakBar.h"
#include "NV/NvString.h"
#include "NV/NvTokenizer.h"

#include <NsAllocator.h>
#include <NsIntrinsics.h>

#include <stdarg.h>
#include <sstream>

#if defined(ANDROID)
#include "../NvEGLUtil/NvEGLUtil.h"
#include "../NvAppBase/android/NvEGLAppContext.h"
#include "NvAndVkWinUtil.h"
#include "NvAppContextAndVK.h"
#else
#include "../NvAppBase/glfw/NvGLFWPlatformContext.h"
#include "../NvAppBase/glfw/NvGLFWContext.h"
#include "NvGLFWContextVK.h"
#endif

NvSampleAppVK::NvSampleAppVK() :
	  mUseWSI(true)
	, mUseGLUI(false)
	, mWrapperContext(NULL)
	, mGLCallbacks(NULL)
{
}

NvSampleAppVK::~NvSampleAppVK()
{
}

bool NvSampleAppVK::platformInitRendering(void) {
	if (!mContext->initialize()) {
		showDialog("Fatal: Vulkan Not Available", 
			"Cannot find required Vulkan components.\nPlease visit http://developer.nvidia.com\n"
			" to locate Vulkan-compatible drivers and OS images\n"
			"The app will now exit", true);
		m_requestedExit = true;
		return false;
	}

	if (!mUseWSI && mGLSupported) {
		if (!strstr((const char*)glGetString(GL_EXTENSIONS), "GL_NV_draw_vulkan_image")) {
			mGLSupported = false;
		}
	}

	return true;
}

void NvSampleAppVK::platformInitUI(void) {
	mContext->initUI();
}

bool NvSampleAppVK::initialize(const NvPlatformInfo& platform, int32_t width, int32_t height) {
	NvVKConfiguration vkConfig;
	configurationCallback(vkConfig);

	const std::vector<std::string>& cmd = getCommandLine();
	std::vector<std::string>::const_iterator iter = cmd.begin();

    bool useValidation = false;
    bool useLoaderDebug = false;
    bool useApiDump = false;
	bool forceWSI = false;

    for (std::vector<std::string>::const_iterator iter = cmd.begin(); iter != cmd.end(); ++iter)
    {
		if (*iter == "-wsi") {
			forceWSI = true;
			mUseWSI = true;
		}
		else if (*iter == "-vkongl")
			mUseWSI = false;
		else if (*iter == "-vkonglui") {
			mUseWSI = false;
			mUseGLUI = true;
		}
		else if (*iter == "-validation")
			useValidation = true;
		else if (*iter == "-loaderdebug")
			useLoaderDebug = true;
		else if (*iter == "-apidump")
			useApiDump = true;
    }

#if defined(_WIN32) || defined(LINUX)
	if (useLoaderDebug)
	{
		putenv("VK_LOADER_DEBUG=all");
	}
#endif

	mGLSupported = false;
	if (mGLCallbacks && !forceWSI) {
		mUseWSI = false;
		mUseGLUI = true;
	}

	if (forceWSI)
		mUseWSI = true;

#if defined(_WIN32) || defined(LINUX)
	NvGLConfiguration glConfig(NvGLAPIVersionGL4(), 8, 8, 8, 8, 16, 0);

	NvGLFWPlatformContext* glfwPlat = new NvGLFWPlatformContext;

	mPlatform = glfwPlat;

	mContext = NULL;

	if (!mUseWSI) {
		NvGLFWContextGL* context = new NvGLFWContextGL(glConfig, NvPlatformCategory::PLAT_DESKTOP,
			NvPlatformOS::OS_WINDOWS);

		glfwPlat->createWindow(width, height);
		GLFWwindow* window = glfwPlat->getGLFWWindow();
		glfwMakeContextCurrent(window);

		if (strstr((const char*)glGetString(GL_EXTENSIONS), "GL_NV_draw_vulkan_image")) {
			context->setWindow(window);
			context->setConfiguration(glConfig);

			// Hack - we should use a factory so we can switch to WSI
			mWrapperContext = new NvAppWrapperContextVK(context, vkConfig, getAppTitle(), platform, mUseGLUI);
			mWrapperContext->useApiDump(useApiDump);
			mWrapperContext->useLoaderDebug(useLoaderDebug);
			mWrapperContext->useValidation(useValidation);
			mWrapperContext->bindContext();
			mContext = mWrapperContext;

			mGLSupported = true;
			if (mGLCallbacks)
				mWrapperContext->setGLDrawCallbacks(mGLCallbacks);

			glfwPlat->updateWindowSize();
		}
		else {
			// AMD drivers did NOT like setting up and tearing down GL and then trying
			// to set up VK.  WAR is to just inform user of the command line option
			/*
			glfwPlat->destroyWindow();
			delete context;
			mUseWSI = true;
			mUseGLUI = false;
			*/
			showDialog("Fatal: Vulkan on GL Not Available",
				"The application includes mixed GL and VK features that the\n"
				"current HW driver cannot support.  Please re-lainch the app\n"
				"with the command-line argument -wsi to force the app to use\n"
				"the native VK windowing system and disable the app's GL features.\n"
				"The app will now exit", true);
			m_requestedExit = true;
		}
	}

	if (!mContext) {
		NvGLFWContextVK* context = new NvGLFWContextVK(vkConfig, NvPlatformCategory::PLAT_DESKTOP, NvPlatformOS::OS_WINDOWS);
        
        context->useApiDump(useApiDump);
        context->useLoaderDebug(useLoaderDebug);
        context->useValidation(useValidation);

		mContext = context;

		glfwPlat->createWindow(width, height);
		GLFWwindow* window = glfwPlat->getGLFWWindow();
		context->setWindow(window);

		glfwPlat->updateWindowSize();
	}
	else {
	}
#elif defined(ANDROID)

	bool success = true;

	if (mUseWSI) {
		NvAndVkWinUtil* win = NvAndVkWinUtil::create();
		NvAppContextAndVK* context = new NvAppContextAndVK(vkConfig, win, NvPlatformInfo(NvPlatformCategory::PLAT_MOBILE, NvPlatformOS::OS_ANDROID));

		if (!context->initialize()) {
			success = false;
		}
		else
		{
			mContext = context;
		}
	}
	else {
		NvEGLWinUtil* eglWin = NvEGLWinUtil::create();
		if (!eglWin) {
			success = false;
		}
		else {
			NvEGLAppContext* context = new NvEGLAppContext(eglWin);
			mContext = context;

			// Hack - we should use a factory so we can switch to WSI
			mWrapperContext = new NvAppWrapperContextVK(context, vkConfig, getAppTitle(), platform, mUseGLUI);
			mWrapperContext->bindContext();
			mContext = mWrapperContext;
			mGLSupported = true;

			if (mGLCallbacks)
				mWrapperContext->setGLDrawCallbacks(mGLCallbacks);
		}
	}

	if (!success) {
		showDialog("Fatal: Vulkan Not Available",
			"Cannot find required Vulkan components.\nPlease visit http://developer.nvidia.com\n"
			" to locate Vulkan-compatible drivers and OS images\n"
			"The app will now exit", true);
		m_requestedExit = true;
		return false;
	}
#endif
	return true;
}

void NvSampleAppVK::platformLogTestResults(float frameRate, int32_t frames) {
}


static const int32_t uniqueTypeID = 0x20000002;

int32_t NvSampleAppVK::getUniqueTypeID() {
	return uniqueTypeID;
}

bool NvSampleAppVK::isType(NvAppBase* app) {
	return app->getUniqueTypeID() == uniqueTypeID;
}

bool NvSampleAppVK::setGLDrawCallbacks(NvGLDrawCallbacks *cb){
	mGLCallbacks = cb;
	return true;
}

