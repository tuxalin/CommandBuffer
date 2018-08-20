//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppWrapperContextVK.cpp
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
#include "NvAppWrapperContextVK.h"
#include "NvAppBase/gl/NvAppContextGL.h"
#include "NvImage/NvImage.h"
#include "NvVkUtil/NvSampleAppVK.h"
#include "NvVkUtil/NvVkUtil.h"
#include "NV/NvLogs.h"
#include "NvVkRenderTargetImpls.h"

extern void NvUIUseGL();
extern bool NvUIIsGL();
extern void NvUIUseVK(NvVkContext* context);

#ifndef ANDROID
#include <GL/glew.h>
#else
#include <GLES/gl.h>
typedef uint64_t GLuint64;
#endif

#include <vulkan/vulkan.h>

#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLEXT_APIENTRY __stdcall
#  elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define GLEXT_APIENTRY __stdcall
#  else
#    define GLEXT_APIENTRY
#  endif

typedef void (GLEXT_APIENTRY * PFN_glWaitVkSemaphoreNV) (GLuint64 vkSemaphore);
typedef void (GLEXT_APIENTRY * PFN_glSignalVkSemaphoreNV) (GLuint64 vkSemaphore);
typedef void (GLEXT_APIENTRY * PFN_glSignalVkFenceNV) (GLuint64 vkFence);
typedef void (GLEXT_APIENTRY * PFN_glDrawVkImageNV) (GLuint64 vkImage, GLuint sampler, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1);

PFN_glWaitVkSemaphoreNV         __nvkglWaitVkSemaphoreNV = 0;
PFN_glSignalVkSemaphoreNV       __nvkglSignalVkSemaphoreNV = 0;
PFN_glSignalVkFenceNV           __nvkglSignalVkFenceNV = 0;
PFN_glDrawVkImageNV             __nvkglDrawVkImageNV = 0;

void glWaitVkSemaphoreNV(GLuint64 vkSemaphore)
{
	__nvkglWaitVkSemaphoreNV(vkSemaphore);
}

void glSignalVkSemaphoreNV(GLuint64 vkSemaphore)
{
	__nvkglSignalVkSemaphoreNV(vkSemaphore);
}

void glSignalVkFenceNV(GLuint64 vkFence)
{
	__nvkglSignalVkFenceNV(vkFence);
}

void glDrawVkImageNV(GLuint64 vkImage, GLuint sampler, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat z, GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1)
{
	__nvkglDrawVkImageNV(vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
}

typedef void (VKAPI_PTR * PFN_vkQueuePresentNV) (VkQueue queue, VkImage image);
PFN_vkQueuePresentNV pfn_vkQueuePresentNV = NULL;
VKAPI_ATTR void VKAPI_CALL vkQueuePresentNV(VkQueue queue, VkImage image)
{
	pfn_vkQueuePresentNV(queue, image);
}

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof( (a)[0] ))


/*
void NvVkContext::shutdown()
{
	vkDestroyDevice(_device, NULL);

	//TODO: investigate why this crashes during app shutdown
#if 0
	dbgDestroyMsgCallback(_instance, msg_callback);
#endif
	vkDestroyInstance(_instance, NULL);
}
*/

NvAppWrapperContextVK::NvAppWrapperContextVK(NvAppContextGL* glContext, NvVKConfiguration config, const std::string& appTitle, NvPlatformInfo info, bool useGLUI) :
NvAppContextVK(config, info)
, mGLContext(glContext)
, mCurrentWidth(0)
, mCurrentHeight(0)
, mAppTitle(appTitle)
, mNVVKDrawImageVK(false)
, mUseGLUI(useGLUI)
, mGLDrawCallbacksInitialized(false)
, mGLDrawCallbacks(NULL)
{
}

NvAppWrapperContextVK::~NvAppWrapperContextVK()
{
	// we do NOT own the glContext;
}

/*
static void* vulkan_so = NULL;
static void* dlVkProc(const char* name) {
	if (!vulkan_so)
		vulkan_so = dlopen("libvulkan.so", RTLD_NOW);

	return 	vulkan_so ? dlsym(vulkan_so, name) : NULL;
}
*/

static VkInstance getVkProcInstance = NULL;

#ifdef ANDROID
#include <dlfcn.h>
static void* vulkan_so = NULL;
static PFN_vkGetInstanceProcAddr android_getVkProc = NULL;
static bool AndroidInitGetProcAddr() {
	vulkan_so = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!vulkan_so)
		return false;

	android_getVkProc = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_so, "vkGetInstanceProcAddr");
	return android_getVkProc ? true : false;
}

static void* AndroidGetVKProcAddr(const char* name) {
	return (void*)android_getVkProc(getVkProcInstance, name);
}
#endif

bool NvAppWrapperContextVK::initialize() {
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	mGLContext->bindContext();
	mGLContext->setSwapInterval(1);

	NvImage::setSupportsBGR(false);

	mNVVKDrawImageVK = mGLContext->isExtensionSupported("GL_NV_draw_vulkan_image");

    typedef PFN_vkVoidFunction(VKAPI_CALL * PFN_vkGetProcAddressNV) (const char *name);
    PFN_vkGetProcAddressNV driver_getProc = NULL;

	driver_getProc = NULL;

	if (!mNVVKDrawImageVK) {
		driver_getProc = (PFN_vkGetProcAddressNV)(mGLContext->getGLProcAddress("ex7991765ed"));
	}
	else {
#ifdef ANDROID
		if (!AndroidInitGetProcAddr())
			return false;
		driver_getProc = (PFN_vkGetProcAddressNV)AndroidGetVKProcAddr;
#else
		driver_getProc = (PFN_vkGetProcAddressNV)(mGLContext->getGLProcAddress("glGetVkProcAddrNV"));
#endif
	}

	if (mNVVKDrawImageVK) {
		__nvkglWaitVkSemaphoreNV = (PFN_glWaitVkSemaphoreNV)mGLContext->getGLProcAddress("glWaitVkSemaphoreNV");
		__nvkglSignalVkSemaphoreNV = (PFN_glSignalVkSemaphoreNV)mGLContext->getGLProcAddress("glSignalVkSemaphoreNV");
		__nvkglSignalVkFenceNV = (PFN_glSignalVkFenceNV)mGLContext->getGLProcAddress("glSignalVkFenceNV");
		__nvkglDrawVkImageNV = (PFN_glDrawVkImageNV)mGLContext->getGLProcAddress("glDrawVkImageNV");
	}
	else {
		// vulkan.dll doesn't export this, so we fetch it from the driver directly
		PFN_vkGetProcAddressNV glvk_getProc = (PFN_vkGetProcAddressNV)(mGLContext->getGLProcAddress("ex7991765ed"));

		if (!pfn_vkQueuePresentNV) {
			if (!glvk_getProc)
				return false;
			pfn_vkQueuePresentNV = (PFN_vkQueuePresentNV)glvk_getProc("vkQueuePresentNV");
		}
	}

	if (!initializeInstance(mAppTitle))
		return false;

	getVkProcInstance = _instance;

	if (!initializeDevice())
		return false;

	if (mNVVKDrawImageVK) {
		// OpenGL drawing
		VkSemaphoreCreateInfo semCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		vkCreateSemaphore(device(), &semCreateInfo, NULL, &m_semImageRead);
	}

	// Query for a valid render target format.
	static VkFormat const cbFormats[] = {
		VK_FORMAT_R8G8B8A8_UNORM,
	};

    // Query for a valid depth stencil format.
    static VkFormat const dsFormats[] = {
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
    };


	m_cbFormat = pickOptimalFormat(ARRAY_SIZE(cbFormats), cbFormats, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    m_dsFormat = pickOptimalFormat(ARRAY_SIZE(dsFormats), dsFormats, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	
	if (m_cbFormat == VK_FORMAT_UNDEFINED) {
		// Could not find a format we want to render with.
		CHECK_VK_RESULT();
	}

	if (m_dsFormat == VK_FORMAT_UNDEFINED) {
        // Could not find a format we want to render with.
        CHECK_VK_RESULT();
    }

    if (!initRenderTarget())
		return false;

	return true;
}

void NvAppWrapperContextVK::contextInitRendering() {
	mGLContext->contextInitRendering();
}


bool NvAppWrapperContextVK::bindContext() {
	return mGLContext->bindContext();
}

bool NvAppWrapperContextVK::unbindContext() {
	return mGLContext->unbindContext();
}

bool NvAppWrapperContextVK::setSwapInterval(int32_t interval) {
	return mGLContext->setSwapInterval(interval);
}

int32_t NvAppWrapperContextVK::width() {
	return mGLContext->width();
}

int32_t NvAppWrapperContextVK::height() {
	return mGLContext->height();
}

bool NvAppWrapperContextVK::useOffscreenRendering(int32_t w, int32_t h) {
	return mGLContext->useOffscreenRendering(w, h);
}

bool NvAppWrapperContextVK::isRenderingToMainScreen() {
	return mGLContext->isRenderingToMainScreen();
}

void NvAppWrapperContextVK::platformReshape(int32_t& w, int32_t& h) {
	VkResult result = VK_SUCCESS;

	mGLContext->platformReshape(w, h);

	if ((mCurrentWidth == w) && (mCurrentHeight == h))
		return;

	// this is for the UI rendered in GL
	glViewport(0, 0, (GLint)w, (GLint)h);

	if (!reshape(w, h))
		return;

	mCurrentWidth = w;
	mCurrentHeight = h;
}

void NvAppWrapperContextVK::beginFrame() {
	if (NvUIIsGL() && mGLDrawCallbacks && !mGLDrawCallbacksInitialized) {
		mGLDrawCallbacks->initGLResources(mGLContext);
		mGLDrawCallbacksInitialized = true;
	}

	if (mainRenderTarget())
		mainRenderTarget()->beginFrame();

	mGLContext->beginFrame();
}

void NvAppWrapperContextVK::beginScene() {
	mGLContext->beginScene();
}

void NvAppWrapperContextVK::drawVKToGL() {
	if (mainRenderTarget()) {
		NvVkRenderTarget& rt = *mainRenderTarget();
		VkSemaphore* sem = getSignalSync();

		rt.endFrame();

		if (mNVVKDrawImageVK) {
			glDisable(GL_DEPTH_TEST);
			glWaitVkSemaphoreNV((GLuint64)*sem);
			glDrawVkImageNV((GLuint64)(VkImage)(rt.image()), 0,
				0, 0, rt.width(), rt.height(), 0,
				0, 1, 1, 0);
			glEnable(GL_DEPTH_TEST);
			glSignalVkSemaphoreNV((GLuint64)m_semImageRead);

			VkSubmitInfo waitPresentDone = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			waitPresentDone.waitSemaphoreCount = 1;
			waitPresentDone.pWaitSemaphores = &m_semImageRead;
			vkQueueSubmit(queue(), 1, &waitPresentDone, 0);
		}
		else {
			vkQueuePresentNV(queue(), rt.image());
		}
	}
}
void NvAppWrapperContextVK::endScene() {
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	if (NvUIIsGL()) {
		if (mGLDrawCallbacks) {
			if (mGLDrawCallbacks->skipVKDraw()) {
				if (mainRenderTarget())
					mainRenderTarget()->endFrame();
			} else {
				NvAppContextVK::endFrame();
				drawVKToGL();
			}
			mGLDrawCallbacks->drawGL(mGLContext);
		}
		else {
			drawVKToGL();
		}
	}

	mGLContext->endScene();
}

void NvAppWrapperContextVK::endFrame() {
	if (!NvUIIsGL()) {
		NvAppContextVK::endFrame();
		drawVKToGL();
	}

	mGLContext->endFrame();
}

bool NvAppWrapperContextVK::swap(void) {
	return true;
}

void NvAppWrapperContextVK::initUI() {
	if (mUseGLUI)
		NvUIUseGL();
	else
		NvUIUseVK(this);
}

bool NvAppWrapperContextVK::readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h) {
	return mGLContext->readFramebufferRGBX32(dest, w, h);
}

NvVkRenderTarget* NvAppWrapperContextVK::mainRenderTarget() {
	return mOffscreenRenderTarget;
}

bool NvAppWrapperContextVK::initRenderTarget() {
	mOffscreenRenderTarget = NvVkMultibufferedRenderTarget::create(*this, m_cbFormat, m_dsFormat, false);

	return mOffscreenRenderTarget ? true : false;
}

