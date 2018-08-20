//----------------------------------------------------------------------------------
// File:        NvAppBase\gl/NvSampleAppGL.cpp
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
#include "NvAppBase/gl/NvSampleAppGL.h"
#if defined(ANDROID)
#include "../../NvEGLUtil/NvEGLUtil.h"
#include "../../NvAppBase/android/NvEGLAppContext.h"
#else
#include "../glfw/NvGLFWPlatformContext.h"
#include "../glfw/NvGLFWContext.h"
#endif
#include "NV/NvLogs.h"
#include "NV/NvPlatformGL.h"
#include "NvAppBase/NvFramerateCounter.h"
#include "NvAppBase/NvInputTransformer.h"
#include "NvImage/NvImage.h"
#include "NvGLUtils/NvImageGL.h"
#include "NvGLUtils/NvSimpleFBO.h"
#include "NvGLUtils/NvTimers.h"
#include "NvUI/NvGestureDetector.h"
#include "NvUI/NvTweakBar.h"
#include "NV/NvString.h"
#include "NV/NvTokenizer.h"

#include <NsAllocator.h>
#include <NsIntrinsics.h>

#include <stdarg.h>
#include <sstream>

NvSampleAppGL::NvSampleAppGL() :
    NvSampleApp()
{
}

NvSampleAppGL::~NvSampleAppGL() 
{ 
}

bool NvSampleAppGL::platformInitRendering(void) {
    LOGI("GL_RENDERER   = %s", (char *)glGetString(GL_RENDERER));
    LOGI("GL_VERSION    = %s", (char *)glGetString(GL_VERSION));
    LOGI("GL_VENDOR     = %s", (char *)glGetString(GL_VENDOR));

    NvGPUTimer::globalInit(*getGLContext());

    LOGI("GL_EXTENSIONS =");

    // Break the extensions into lines without breaking extensions
    // (since unbroken line-wrap with extensions hurts search)
    {
        const int32_t lineMaxLen = 80;
        const char *s, *delimStr = " ";
        size_t ltl, delimLen = strlen(delimStr);
        std::string exbuf;
        NvTokenizer tok((const char*)glGetString(GL_EXTENSIONS));
        while (tok.readToken()) {
            ltl = tok.getLastTokenLen();
            s = tok.getLastTokenPtr();
            if (exbuf.length() + delimLen + ltl > lineMaxLen) { // adding would exceed max, print and clear
                if (exbuf.length())
                    LOGI("%s", exbuf.c_str());
                exbuf.clear();
                if (ltl > lineMaxLen) { // alone is too long, print alone and continue
                    LOGI("%s", s);
                }
                continue; //in either case, next...
            }
            exbuf.append(delimStr);
            exbuf.append(s);
        }
        if (exbuf.length()) // flush last line
            LOGI("%s\n\n", exbuf.c_str());
    }

    // check extensions and enable DXT expansion if needed
    bool hasDXT = getGLContext()->isExtensionSupported("GL_EXT_texture_compression_s3tc") ||
        getGLContext()->isExtensionSupported("GL_EXT_texture_compression_dxt1");
    if (!hasDXT) {
        LOGI("Device has no DXT texture support - enabling DXT expansion");
        NvImage::setDXTExpansion(true);

    }

	return true;
}

void NvSampleAppGL::platformInitUI(void) {
	mContext->initUI();
}

bool NvSampleAppGL::requireExtension(const char* ext, bool exitOnFailure) {
    if (!getGLContext()->isExtensionSupported(ext)) {
        if (exitOnFailure) {
            std::string caption = std::string("The current system does not appear to support the extension ")
                + std::string(ext) + std::string(", which is required by the sample.  "
                "This is likely because the system's GPU or driver does not support the extension.  "
                "Please see the sample's source code for details");
            errorExit(caption.c_str());
        }

        return false;
    }

    return true;
}

bool NvSampleAppGL::requireMinAPIVersion(const NvGLAPIVersion& minApi, bool exitOnFailure) {
    const NvGLAPIVersion api = getGLContext()->getConfiguration().apiVer;
    if (api < minApi) {
        if (exitOnFailure) {
            char caption [1024];
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
            sprintf(caption, "The current system does not appear to support the minimum GL API required "
                "by the sample (requested: %s %d.%d, got: %s %d.%d).  This is likely because the system's GPU or driver "
                "does not support the API.  Please see the sample's source code for details", 
                (minApi.api == NvGLAPI::GL) ? "GL" : "GLES", 
                minApi.majVersion, minApi.minVersion,
                (api.api == NvGLAPI::GL) ? "GL" : "GLES", api.majVersion, api.minVersion);
#ifdef _WIN32
#pragma warning( pop )
#endif
            errorExit(caption);
        }

        return false;
    }

    return true;
}

void NvSampleAppGL::platformLogTestResults(float frameRate, int32_t frames) {
    writeLogFile(mTestName, true, "GL_VENDOR %s\n", glGetString(GL_VENDOR));
    writeLogFile(mTestName, true, "GL_RENDERER %s\n", glGetString(GL_RENDERER));
    writeLogFile(mTestName, true, "GL_EXTENSIONS %s\n", glGetString(GL_EXTENSIONS));

    if (m_testModeIssues != TEST_MODE_ISSUE_NONE) {
        writeLogFile(mTestName, true, "\nWARNING - there were potential test mode anomalies\n");

        if (m_testModeIssues & TEST_MODE_FBO_ISSUE) {
            writeLogFile(mTestName, true, "\tThe application appears to have explicitly bound the onscreen framebuffer\n"
                "\tSince the test was being run in offscreen rendering mode, this could invalidate results\n"
                "\tThe application should be checked for glBindFramebuffer of 0\n\n");
        }
    }
}

static const int32_t uniqueTypeID = 0x20000001;

int32_t NvSampleAppGL::getUniqueTypeID() {
	return uniqueTypeID;
}

bool NvSampleAppGL::isType(NvAppBase* app) {
	return app->getUniqueTypeID() == uniqueTypeID;
}

static NvGLConfiguration sDefaultConfig(NvGLAPIVersionGL4_4(), 8, 8, 8, 8, 16, 0, 0);

#ifdef ANDROID
static bool localEGLChooser(EGLDisplay disp, EGLint apiSupport, EGLConfig& bestConfig, EGLint& api, EGLint& apiMajVer, EGLint& apiMinVer)
{

	EGLint count = 0;
	if (!eglGetConfigs(disp, NULL, 0, &count))
	{
		EGL_ERROR_LOG("defaultEGLChooser cannot query count of all configs");
		return false;
	}

	LOGI("Config count = %d", count);

	EGLConfig* configs = new EGLConfig[count];
	if (!eglGetConfigs(disp, configs, count, &count))
	{
		EGL_ERROR_LOG("defaultEGLChooser cannot query all configs");
		return false;
	}

	// Force a fall-back if needed.
	if ((sDefaultConfig.apiVer.api == NvGLAPI::GL) &&
		((apiSupport & EGL_OPENGL_BIT) == 0)) {
		sDefaultConfig.apiVer.api = NvGLAPI::GLES;
		sDefaultConfig.apiVer.majVersion = 3;
		sDefaultConfig.apiVer.minVersion = 0;
	}

	int32_t matchRenderableFlags = (sDefaultConfig.apiVer.api == NvGLAPI::GL)
		? EGL_OPENGL_BIT : EGL_OPENGL_ES2_BIT;

	int32_t bestMatch = 1 << 30;
	int32_t bestIndex = -1;

	int32_t i;
	for (i = 0; i < count; i++)
	{
		EGLint surfaceType = 0;
		EGLint blueBits = 0;
		EGLint greenBits = 0;
		EGLint redBits = 0;
		EGLint alphaBits = 0;
		EGLint depthBits = 0;
		EGLint stencilBits = 0;
		EGLint renderableFlags = 0;
		EGLint msaaSamples = 0;

		eglGetConfigAttrib(disp, configs[i], EGL_SURFACE_TYPE, &surfaceType);
		eglGetConfigAttrib(disp, configs[i], EGL_BLUE_SIZE, &blueBits);
		eglGetConfigAttrib(disp, configs[i], EGL_GREEN_SIZE, &greenBits);
		eglGetConfigAttrib(disp, configs[i], EGL_RED_SIZE, &redBits);
		eglGetConfigAttrib(disp, configs[i], EGL_ALPHA_SIZE, &alphaBits);
		eglGetConfigAttrib(disp, configs[i], EGL_DEPTH_SIZE, &depthBits);
		eglGetConfigAttrib(disp, configs[i], EGL_STENCIL_SIZE, &stencilBits);
		eglGetConfigAttrib(disp, configs[i], EGL_RENDERABLE_TYPE, &renderableFlags);
		eglGetConfigAttrib(disp, configs[i], EGL_SAMPLES, &msaaSamples);

		// Currently, this NEEDS to be a window-compatible config
		if ((surfaceType & EGL_WINDOW_BIT) == 0)
			continue;

		// Require the requested renderable APIs
		if ((renderableFlags & matchRenderableFlags) == 0)
			continue;

		// Used when a feature greatly under-represents the requested values, but exists
		const int32_t MAJOR_PENALTY = 1 << 8;

		// Used when a feature is over-spec'ed, and thus wasteful
		const int32_t MINOR_PENALTY = 1;

		// The fuzzy comparisons come next, and we use a penalty system for them
		int32_t penalty = 0;

		if (sDefaultConfig.depthBits > 0)
		{
			// If the user requests a depth buffer, then we want to match what they
			// request _or_ better.  Larger will be a minor penalty, smaller is major,
			// none is a showstopper.  Same for stencil.
			if (depthBits == 0)
				continue;

			if (depthBits < sDefaultConfig.depthBits)
			{
				penalty += MAJOR_PENALTY;
			}
			else if (depthBits > sDefaultConfig.depthBits)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * ((depthBits - sDefaultConfig.depthBits) >> 3);
			}
		}
		else
		{
			// If the user requests _no_ depth buffer, then it is a minor penalty to have
			// one, based on size of it
			if (depthBits > 0)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * (depthBits >> 3);
			}
		}

		if (sDefaultConfig.stencilBits > 0)
		{
			// If the user requests a stencil buffer, then we want to match what they
			// request _or_ better.  Larger will be a minor penalty, smaller is major,
			// none is a showstopper.
			if (stencilBits == 0)
				continue;

			if (stencilBits < sDefaultConfig.stencilBits)
			{
				penalty += MAJOR_PENALTY;
			}
			else if (stencilBits > sDefaultConfig.stencilBits)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * ((stencilBits - sDefaultConfig.stencilBits) >> 3);
			}
		}
		else
		{
			// If the user requests _no_ stencil buffer, then it is a minor penalty to have
			// one, based on size of it
			if (stencilBits > 0)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * (stencilBits >> 3);
			}
		}

		// MSAA cannot be a complete requirement, so we never filter out.  But asking
		// for it should net _some_ # of samples
		if (sDefaultConfig.msaaSamples > 1)
		{
			if (msaaSamples <= 1)
			{
				// We wanted MSAA, we got none...
				penalty += MAJOR_PENALTY;
			}
			else if (msaaSamples < sDefaultConfig.msaaSamples)
			{
				// fewer samples than we want; 2 minor penalties
				penalty += 2 * MINOR_PENALTY;
			}
			else if (msaaSamples > sDefaultConfig.msaaSamples)
			{
				// more samples than we want; minor penalty
				penalty += MINOR_PENALTY;
			}
		}
		else
		{
			if (msaaSamples > sDefaultConfig.msaaSamples)
			{
				// More samples than we want; blanket penalty
				penalty += MINOR_PENALTY;
			}
		}

		// Color is handled as one item, so as not to overwhelm, except for
		// destination alpha, which has its own penalty
		const int requestedRGB = sDefaultConfig.redBits + sDefaultConfig.greenBits + sDefaultConfig.blueBits;
		const int RGB = redBits + greenBits + blueBits;
		if (requestedRGB > RGB)
		{
			// major penalty for having fewer bits than requested
			penalty += MAJOR_PENALTY;
		}
		else if (requestedRGB < RGB)
		{
			// minor penalty for having more bits than requested
			penalty += MINOR_PENALTY;
		}

		// Now handle alpha, as this is an important "feature" if requested
		if (sDefaultConfig.alphaBits > alphaBits)
		{
			// major penalty for having fewer bits than requested
			penalty += MAJOR_PENALTY;
		}
		else if (sDefaultConfig.alphaBits < alphaBits)
		{
			// minor penalty for having more bits than requested
			penalty += MINOR_PENALTY;
		}

		LOGI("Config[%d]: R%dG%dB%dA%d D%dS%d MSAA=%d  Type=%04x Render=%04x (penalties: %d)",
			i, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, msaaSamples, surfaceType, renderableFlags, penalty);

		if ((penalty < bestMatch) || (bestIndex == -1))
		{
			bestMatch = penalty;
			bestIndex = i;
			LOGI("Config[%d] is the new best config", i, configs[i]);
		}
	}

	if (bestIndex < 0)
	{
		delete[] configs;
		return false;
	}

	bestConfig = configs[bestIndex];
	delete[] configs;

	api = (sDefaultConfig.apiVer.api == NvGLAPI::GL) ? EGL_OPENGL_API : EGL_OPENGL_ES_API;
	apiMajVer = sDefaultConfig.apiVer.majVersion;
	apiMinVer = sDefaultConfig.apiVer.minVersion;

	return true;
}

static bool hdrConfigChooser(EGLDisplay disp, EGLint apiSupport, EGLConfig& bestConfig, EGLint& api, EGLint& apiMajVer, EGLint& apiMinVer)
{
	if (!(apiSupport & EGL_OPENGL_ES_BIT))
		return false;

	const  int EGLMinRedBits = 5;
	const  int EGLMinGreenBits = 6;
	const  int EGLMinBlueBits = 5;
	const  int EGLMinAlphaBits = 0;
	const  int EGLMinDepthBits = 16;
	const  int EGLMinStencilBits = 0;
	const  int EGLMinSampleBuffers = 0;
	const  int EGLMinSampleSamples = 0;


	const EGLint AttributesHDR[] = {
		EGL_RED_SIZE, EGLMinRedBits,
		EGL_GREEN_SIZE, EGLMinGreenBits,
		EGL_BLUE_SIZE, EGLMinBlueBits,
		EGL_ALPHA_SIZE, EGLMinAlphaBits,
		EGL_DEPTH_SIZE, EGLMinDepthBits,
		EGL_STENCIL_SIZE, EGLMinStencilBits,
		EGL_SAMPLE_BUFFERS, EGLMinSampleBuffers,
		EGL_SAMPLES, EGLMinSampleSamples,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_CONFIG_CAVEAT, EGL_NONE,
		EGL_COLOR_COMPONENT_TYPE_EXT, EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
		EGL_NONE
	};

	EGLint count = 0;
	if (!eglChooseConfig(disp, AttributesHDR, NULL, 0, &count))
	{
		EGL_ERROR_LOG("hdrConfigChooser cannot query count of hdr configs");
		return false;
	}

	LOGI("Config count = %d", count);

	EGLConfig* configs = new EGLConfig[count];
	if (!eglChooseConfig(disp, AttributesHDR, configs, count, &count))
	{
		EGL_ERROR_LOG("hdrConfigChooser cannot query hdr configs");
		return false;
	}

	// Force a fall-back if needed.
	if ((sDefaultConfig.apiVer.api == NvGLAPI::GL) &&
		((apiSupport & EGL_OPENGL_BIT) == 0)) {
		sDefaultConfig.apiVer.api = NvGLAPI::GLES;
		sDefaultConfig.apiVer.majVersion = 3;
		sDefaultConfig.apiVer.minVersion = 0;
	}

	int32_t matchRenderableFlags = (sDefaultConfig.apiVer.api == NvGLAPI::GL)
		? EGL_OPENGL_BIT : EGL_OPENGL_ES2_BIT;

	int32_t bestMatch = 1 << 30;
	int32_t bestIndex = -1;

	int32_t i;
	for (i = 0; i < count; i++)
	{
		EGLint surfaceType = 0;
		EGLint blueBits = 0;
		EGLint greenBits = 0;
		EGLint redBits = 0;
		EGLint alphaBits = 0;
		EGLint depthBits = 0;
		EGLint stencilBits = 0;
		EGLint renderableFlags = 0;
		EGLint msaaSamples = 0;

		eglGetConfigAttrib(disp, configs[i], EGL_SURFACE_TYPE, &surfaceType);
		eglGetConfigAttrib(disp, configs[i], EGL_BLUE_SIZE, &blueBits);
		eglGetConfigAttrib(disp, configs[i], EGL_GREEN_SIZE, &greenBits);
		eglGetConfigAttrib(disp, configs[i], EGL_RED_SIZE, &redBits);
		eglGetConfigAttrib(disp, configs[i], EGL_ALPHA_SIZE, &alphaBits);
		eglGetConfigAttrib(disp, configs[i], EGL_DEPTH_SIZE, &depthBits);
		eglGetConfigAttrib(disp, configs[i], EGL_STENCIL_SIZE, &stencilBits);
		eglGetConfigAttrib(disp, configs[i], EGL_RENDERABLE_TYPE, &renderableFlags);
		eglGetConfigAttrib(disp, configs[i], EGL_SAMPLES, &msaaSamples);
		LOGI("Config[%d]: R%dG%dB%dA%d D%dS%d Type=%04x Render=%04x",
			i, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, surfaceType, renderableFlags);

		// Currently, this NEEDS to be a window-compatible config
		if ((surfaceType & EGL_WINDOW_BIT) == 0)
			continue;

		// Require the requested renderable APIs
		if ((renderableFlags & matchRenderableFlags) == 0)
			continue;

		// Used when a feature greatly under-represents the requested values, but exists
		const int32_t MAJOR_PENALTY = 1 << 8;

		// Used when a feature is over-spec'ed, and thus wasteful
		const int32_t MINOR_PENALTY = 1;

		// The fuzzy comparisons come next, and we use a penalty system for them
		int32_t penalty = 0;

		if (sDefaultConfig.depthBits > 0)
		{
			// If the user requests a depth buffer, then we want to match what they
			// request _or_ better.  Larger will be a minor penalty, smaller is major,
			// none is a showstopper.  Same for stencil.
			if (depthBits == 0)
				continue;

			if (depthBits < sDefaultConfig.depthBits)
			{
				penalty += MAJOR_PENALTY;
			}
			else if (depthBits > sDefaultConfig.depthBits)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * ((depthBits - sDefaultConfig.depthBits) >> 3);
			}
		}
		else
		{
			// If the user requests _no_ depth buffer, then it is a minor penalty to have
			// one, based on size of it
			if (depthBits > 0)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * (depthBits >> 3);
			}
		}

		if (sDefaultConfig.stencilBits > 0)
		{
			// If the user requests a stencil buffer, then we want to match what they
			// request _or_ better.  Larger will be a minor penalty, smaller is major,
			// none is a showstopper.
			if (stencilBits == 0)
				continue;

			if (stencilBits < sDefaultConfig.stencilBits)
			{
				penalty += MAJOR_PENALTY;
			}
			else if (stencilBits > sDefaultConfig.stencilBits)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * ((stencilBits - sDefaultConfig.stencilBits) >> 3);
			}
		}
		else
		{
			// If the user requests _no_ stencil buffer, then it is a minor penalty to have
			// one, based on size of it
			if (stencilBits > 0)
			{
				// buffer deeper than we want - penalty point for each extra 8 bits
				penalty += MINOR_PENALTY * (stencilBits >> 3);
			}
		}

		// MSAA cannot be a complete requirement, so we never filter out.  But asking
		// for it should net _some_ # of samples
		if (sDefaultConfig.msaaSamples > 1)
		{
			if (msaaSamples <= 1)
			{
				// We wanted MSAA, we got none...
				penalty += MAJOR_PENALTY;
			}
			else if (msaaSamples < sDefaultConfig.msaaSamples)
			{
				// fewer samples than we want; 2 minor penalties
				penalty += 2 * MINOR_PENALTY;
			}
			else if (msaaSamples > sDefaultConfig.msaaSamples)
			{
				// more samples than we want; minor penalty
				penalty += MINOR_PENALTY;
			}
		}
		else
		{
			if (msaaSamples > sDefaultConfig.msaaSamples)
			{
				// More samples than we want; blanket penalty
				penalty += MINOR_PENALTY;
			}
		}

		// Color is handled as one item, so as not to overwhelm, except for
		// destination alpha, which has its own penalty
		const int requestedRGB = sDefaultConfig.redBits + sDefaultConfig.greenBits + sDefaultConfig.blueBits;
		const int RGB = redBits + greenBits + blueBits;
		if (requestedRGB > RGB)
		{
			// major penalty for having fewer bits than requested
			penalty += MAJOR_PENALTY;
		}
		else if (requestedRGB < RGB)
		{
			// minor penalty for having more bits than requested
			penalty += MINOR_PENALTY;
		}

		// Now handle alpha, as this is an important "feature" if requested
		if (sDefaultConfig.alphaBits > alphaBits)
		{
			// major penalty for having fewer bits than requested
			penalty += MAJOR_PENALTY;
		}
		else if (sDefaultConfig.alphaBits < alphaBits)
		{
			// minor penalty for having more bits than requested
			penalty += MINOR_PENALTY;
		}

		LOGI("Config[%d]: R%dG%dB%dA%d D%dS%d MSAA=%d  Type=%04x Render=%04x (penalties: %d)",
			i, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, msaaSamples, surfaceType, renderableFlags, penalty);

		if ((penalty < bestMatch) || (bestIndex == -1))
		{
			bestMatch = penalty;
			bestIndex = i;
			LOGI("Config[%d] is the new best config", i, configs[i]);
		}
	}

	if (bestIndex < 0)
	{
		delete[] configs;
		return false;
	}

	bestConfig = configs[bestIndex];
	delete[] configs;

	api = EGL_OPENGL_ES_API;
	apiMajVer = 3;
	apiMinVer = 0;

	return true;
}



#endif

bool NvSampleAppGL::initialize(const NvPlatformInfo& platform, int32_t width, int32_t height) {
	configurationCallback(sDefaultConfig);

#if !defined(ANDROID) // all GLFW platforms
	NvGLFWPlatformContext* glfwPlat = new NvGLFWPlatformContext;

	mPlatform = glfwPlat;

	NvGLFWContextGL* contextGL = new NvGLFWContextGL(sDefaultConfig, NvPlatformCategory::PLAT_DESKTOP,
		NvPlatformOS::OS_WINDOWS);

	glfwPlat->createWindow(width, height);
	GLFWwindow* window = glfwPlat->getGLFWWindow();
	contextGL->setWindow(window);

	contextGL->setConfiguration(sDefaultConfig);

	contextGL->bindContext();
	mContext = contextGL;

	glfwPlat->updateWindowSize();
#else
	NvEGLWinUtil* eglWin = nullptr;
	if (eglWin->getHDRAppFlag() && eglWin->getHDRSinkFlag()) {
		eglWin = NvEGLWinUtil::create(hdrConfigChooser);
	}
	else {
		eglWin = NvEGLWinUtil::create(localEGLChooser);
	}
	
	if (!eglWin)
		return false;
	NvEGLAppContext* context = new NvEGLAppContext(eglWin);
	mContext = context;
#endif
	return true;
}