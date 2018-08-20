//----------------------------------------------------------------------------------
// File:        NvEGLUtil/NvEGLUtil.cpp
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
#ifdef ANDROID

#include "NvEGLUtil.h"
#include <android/log.h>
#include <unistd.h>
#include <NV/NvPlatformGL.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  \
                                             "NvEGLWinUtil", \
                                             __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,  \
                                             "NvEGLWinUtil", \
                                             __VA_ARGS__))

//#define NV_ES31_FUNC(ret, func, parms) \
//ret (KHRONOS_APIENTRY *NvRemap_ ## func) parms = 0;

bool s_functionsInitialized = false;

// Hack until I split out the enums and types
typedef void (GL_APIENTRY  *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

#undef NV_ES31_FUNC
#undef NV_ES31_FUNC_RET

#define NV_ES31_FUNC(ret, func, formals, actuals) \
    typedef ret (KHRONOS_APIENTRY* NvPtr_ ## func ## Type) formals; \
    static NvPtr_ ## func ## Type NvPtr_ ## func = NULL;

#define NV_ES31_FUNC_RET(ret, func, formals, actuals) \
    typedef ret (KHRONOS_APIENTRY* NvPtr_ ## func ## Type) formals; \
    static NvPtr_ ## func ## Type NvPtr_ ## func = NULL;

#include "NvGLESWrapper/NvES31Funcs.h"
#include "NvGLESWrapper/NvES31AEPFuncs.h"

#undef NV_ES31_FUNC
#undef NV_ES31_FUNC_RET

#define NV_ES31_FUNC(ret, func, formals, actuals) \
ret KHRONOS_APIENTRY NvRemap_ ## func formals \
    { NvPtr_ ## func actuals; }

#define NV_ES31_FUNC_RET(ret, func, formals, actuals) \
ret KHRONOS_APIENTRY NvRemap_ ## func formals \
    { return NvPtr_ ## func actuals; }

#include "NvGLESWrapper/NvES31Funcs.h"
#include "NvGLESWrapper/NvES31AEPFuncs.h"

#undef NV_ES31_FUNC
#undef NV_ES31_FUNC_RET

static void NvInitWrapperFunctions(bool isGL)
{
    if (isGL)
    {
#define NV_ES31_WRAP_FUNC(ret, func, noextfunc, formals, actuals) \
    NvPtr_ ## func = (NvPtr_ ## func ## Type) eglGetProcAddress(#noextfunc);

#define NV_ES31_WRAP_FUNC_RET(ret, func, noextfunc, formals, actuals) \
    NvPtr_ ## func = (NvPtr_ ## func ## Type) eglGetProcAddress(#noextfunc);

#include "NvGLESWrapper/NvES31GLRemap.h"
#include "NvGLESWrapper/NvES31AEPGLRemap.h"

#undef NV_ES31_WRAP_FUNC
#undef NV_ES31_WRAP_FUNC_RET
    }
    else
    {
#define NV_ES31_FUNC(ret, func, formals, actuals) \
    NvPtr_ ## func = (NvPtr_ ## func ## Type) eglGetProcAddress(#func);

#define NV_ES31_FUNC_RET(ret, func, formals, actuals) \
    NvPtr_ ## func = (NvPtr_ ## func ## Type) eglGetProcAddress(#func);

#include "NvGLESWrapper/NvES31Funcs.h"
#include "NvGLESWrapper/NvES31AEPFuncs.h"

#undef NV_ES31_FUNC
#undef NV_ES31_FUNC_RET
    }
    s_functionsInitialized = true;
}


// EGL_KHR_create_context
#define EGL_CONTEXT_MAJOR_VERSION_KHR           0x3098
#define EGL_CONTEXT_MINOR_VERSION_KHR           0x30FB
#define EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR        0x30FD
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR 0x00000002 


#if 1
#define EGL_STATUS_LOG(str) \
    LOGD("Success: %s (%s:%d)", str, __FUNCTION__, __LINE__)

#define EGL_ERROR_LOG(str) \
        LOGE("Failure: %s, error = 0x%08x (%s:%d)", str, eglGetError(), __FUNCTION__, __LINE__)
#else
#define EGL_STATUS_LOG(str)
#define EGL_ERROR_LOG(str)
#endif


bool NvEGLWinUtil::defaultEGLChooser(EGLDisplay disp, EGLint apiSupport, EGLConfig& bestConfig, 
    EGLint& api, EGLint& apiMajVer, EGLint& apiMinVer)
{
	if (!(apiSupport & EGL_OPENGL_ES_BIT))
        return false;

    EGLint count = 0;
    if (!eglGetConfigs(disp, NULL, 0, &count))
    {
        EGL_ERROR_LOG("defaultEGLChooser cannot query count of all configs");
        return false;
    }

    LOGD("Config count = %d", count);

    EGLConfig* configs = new EGLConfig[count];
    if (!eglGetConfigs(disp, configs, count, &count))
    {
        EGL_ERROR_LOG("defaultEGLChooser cannot query all configs");
        return false;
    }

    int32_t bestMatch = 1<<30;
    int32_t bestIndex = -1;

    int32_t i;
    for (i = 0; i < count; i++)
    {
        int32_t match = 0;
        EGLint surfaceType = 0;
        EGLint blueBits = 0;
        EGLint greenBits = 0;
        EGLint redBits = 0;
        EGLint alphaBits = 0;
        EGLint depthBits = 0;
        EGLint stencilBits = 0;
        EGLint renderableFlags = 0;

        eglGetConfigAttrib(disp, configs[i], EGL_SURFACE_TYPE, &surfaceType);
        eglGetConfigAttrib(disp, configs[i], EGL_BLUE_SIZE, &blueBits);
        eglGetConfigAttrib(disp, configs[i], EGL_GREEN_SIZE, &greenBits);
        eglGetConfigAttrib(disp, configs[i], EGL_RED_SIZE, &redBits);
        eglGetConfigAttrib(disp, configs[i], EGL_ALPHA_SIZE, &alphaBits);
        eglGetConfigAttrib(disp, configs[i], EGL_DEPTH_SIZE, &depthBits);
        eglGetConfigAttrib(disp, configs[i], EGL_STENCIL_SIZE, &stencilBits);
        eglGetConfigAttrib(disp, configs[i], EGL_RENDERABLE_TYPE, &renderableFlags);
        LOGD("Config[%d]: R%dG%dB%dA%d D%dS%d Type=%04x Render=%04x",
            i, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, surfaceType, renderableFlags);

        if ((surfaceType & EGL_WINDOW_BIT) == 0)
            continue;
        if ((renderableFlags & EGL_OPENGL_ES2_BIT) == 0)
            continue;

        int32_t penalty = depthBits - 16;
        match += penalty * penalty;
        penalty = redBits - 5;
        match += penalty * penalty;
        penalty = greenBits - 6;
        match += penalty * penalty;
        penalty = blueBits - 5;
        match += penalty * penalty;
        penalty = alphaBits;
        match += penalty * penalty;
        penalty = stencilBits;
        match += penalty * penalty;

        if ((match < bestMatch) || (bestIndex == -1))
        {
            bestMatch = match;
            bestIndex = i;
            LOGD("Config[%d] is the new best config", i, configs[i]);
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
    apiMajVer = 2;
    apiMinVer = 0;

    return true; 
}

NvEGLWinUtil* NvEGLWinUtil::create(ConfigChooser chooser)
{
    NvEGLWinUtil* thiz = new NvEGLWinUtil;

    thiz->m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (thiz->m_display != EGL_NO_DISPLAY)
    {
        EGL_STATUS_LOG("eglGetDisplay");
    }
    else
    {
        EGL_ERROR_LOG("eglGetDisplay");
        delete thiz;
        return NULL;
    }

    if (eglInitialize(thiz->m_display, 0, 0))
    {
        EGL_STATUS_LOG("eglInitialize");
    }
    else
    {
        EGL_ERROR_LOG("eglInitialize");
        delete thiz;
        return NULL;
    }

    // Scan the configs for GLES and EGL support
    EGLint apiSupport = 0;
    EGLint count = 0;
    if (!eglGetConfigs(thiz->m_display, NULL, 0, &count))
    {
        EGL_ERROR_LOG("defaultEGLChooser cannot query all configs");
        delete thiz;
        return NULL;
    }

    EGLConfig* configs = new EGLConfig[count];
    if (!eglGetConfigs(thiz->m_display, configs, count, &count))
    {
        EGL_ERROR_LOG("defaultEGLChooser cannot query all configs");
        delete thiz;
        return NULL;
    }

    int32_t i;
    for (i = 0; i < count; i++)
    {
        EGLint surfaceType = 0;
        EGLint renderableFlags = 0;

        eglGetConfigAttrib(thiz->m_display, configs[i], EGL_SURFACE_TYPE, &surfaceType);
        eglGetConfigAttrib(thiz->m_display, configs[i], EGL_RENDERABLE_TYPE, &renderableFlags);

        if ((surfaceType & EGL_WINDOW_BIT) == 0)
            continue;
        apiSupport |= renderableFlags;
    }

    delete[] configs;

    if (chooser(thiz->m_display, apiSupport, thiz->m_config, thiz->m_api, thiz->m_majVer, thiz->m_minVer))
    {
        EGL_STATUS_LOG("Config chooser");
    }
    else
    {
        EGL_ERROR_LOG("Config chooser");
        delete thiz;
        return NULL;
    }

    if (eglGetConfigAttrib(thiz->m_display, thiz->m_config, EGL_NATIVE_VISUAL_ID, 
        &(thiz->m_format)))
    {
        EGL_STATUS_LOG("eglGetConfigAttrib");
    }
    else
    {
        EGL_ERROR_LOG("eglGetConfigAttrib");
        delete thiz;
        return NULL;
    }

    thiz->m_context = EGL_NO_CONTEXT;

    if (!thiz->createContext())
    {
        EGL_ERROR_LOG("eglCreateContext");
        delete thiz;
        return NULL;
    }

    if (thiz->m_context == EGL_NO_CONTEXT) {
        EGL_ERROR_LOG("eglCreateContext");
        delete thiz;
        return NULL;
    }

    thiz->m_status = NV_INITIALIZED;

	ms_defaultWindow = thiz;

    return thiz;
}

bool NvEGLWinUtil::createContext() {
    if (m_api == EGL_OPENGL_API) {
        EGL_STATUS_LOG("Attempting EGL_OPENGL_API");
        EGLint contextAttrs[] = { EGL_CONTEXT_MAJOR_VERSION_KHR, m_majVer, 
                                  EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,
                                  EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
                                  EGL_NONE };        
        EGLBoolean result = eglBindAPI(EGL_OPENGL_API);
        if (!result) {
            EGL_STATUS_LOG("Failed to bind GL API!");
        }
        if (result && (m_context = eglCreateContext(m_display, m_config, NULL, contextAttrs))) {
            EGLint value = 0;
            if (eglQueryContext(m_display, m_context, EGL_CONTEXT_MAJOR_VERSION_KHR, &value))
                m_majVer = value;
            m_minVer = 0;
            if (eglQueryContext(m_display, m_context, EGL_CONTEXT_MINOR_VERSION_KHR, &value))
                m_minVer = value;
            EGL_STATUS_LOG("eglCreateContext");
        } else {
            // Failed - try ES3.1
            m_api = EGL_OPENGL_ES_API;
            m_majVer = 3;
            m_minVer = 1;
        }
    }

    // No context yet - we may have tried full GL, but we didn't get it.  Or we
    // requested ES from the get-go.  In any case, we're going to try ES next. 
    if (m_context == EGL_NO_CONTEXT) {
        EGLBoolean result = eglBindAPI(EGL_OPENGL_ES_API);

        if (m_minVer != 0) {
            EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, m_majVer, 
                                      EGL_CONTEXT_MINOR_VERSION_KHR, m_minVer,
                                      EGL_NONE };

            if (m_context = eglCreateContext(m_display, m_config, NULL, contextAttrs))
	        {
                // We succeeded, but we may not have gotten what we wanted.  We need to check
		        EGL_STATUS_LOG("eglCreateContext ES");
                EGLint ver = 0;
                if (eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &ver)) {
                    if (m_majVer != ver && ver < 2) {
                        // we didn't get what we wanted...
                        eglDestroyContext(m_display, m_context);
                        m_context = EGL_NO_CONTEXT;
                    } else {
                        m_majVer = ver;
                        m_minVer = 0;
                    }
                } else {
                    eglDestroyContext(m_display, m_context);
                    m_context = EGL_NO_CONTEXT;
                }
                if (eglQueryContext(m_display, m_context, EGL_CONTEXT_MINOR_VERSION_KHR, &ver)) {
    		        EGL_STATUS_LOG("eglCreateContext got minor version");
                    m_minVer = ver;
                } else {
                    EGL_ERROR_LOG("eglQueryContext version fail");
                }
	        } else {
                    EGL_ERROR_LOG("eglCreateContext version fail");
            }
        }

        if (m_context == EGL_NO_CONTEXT) {
            m_minVer = 0;
            EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, m_majVer, EGL_NONE };

            if (m_context = eglCreateContext(m_display, m_config, NULL, contextAttrs))
	        {
                // We succeeded, but we may not have gotten what we wanted.  We need to check
		        EGL_STATUS_LOG("eglCreateContext");
                EGLint ver = 0;
                if (eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &ver)) {
                    if (m_majVer != ver && ver < 2) {
                        // we didn't get what we wanted...
                        eglDestroyContext(m_display, m_context);
                        m_context = EGL_NO_CONTEXT;
                    } else {
                        m_majVer = ver;
                        m_minVer = 0;
                    }
                } else {
                    eglDestroyContext(m_display, m_context);
                    m_context = EGL_NO_CONTEXT;
                }
	        }
        }

	    if (m_context == EGL_NO_CONTEXT) {
            // failed.  If we were asking for ES3, try ES2...
            if (m_majVer > 2) {
                m_majVer = 2;
                m_minVer = 0;
                EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, m_majVer, EGL_NONE };

                if (m_context = eglCreateContext(m_display, m_config, NULL, contextAttrs))
	            {
                    EGLint ver = 0;
                    eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &ver);
		            LOGD("eglCreateContext ES2 fallback %d", ver);
	            }
	            else
	            {
		            EGL_ERROR_LOG("eglCreateContext");
                }
    	    }
        }

        {
            EGLint ver = 0;
            if (eglQueryContext(m_display, m_context, EGL_CONTEXT_CLIENT_VERSION, &ver)) {
                m_majVer = ver;
            }
            if (eglQueryContext(m_display, m_context, EGL_CONTEXT_MINOR_VERSION_KHR, &ver)) {
                m_minVer = ver;
            }
        }
    }

    return (m_context == EGL_NO_CONTEXT) ? false : true;
}


NvEGLWinUtil::~NvEGLWinUtil()
{
    LOGD("**** Releasing EGL Manager object and all EGL Resources");
    if (eglDestroyContext(m_display, m_context))
    {
        EGL_STATUS_LOG("eglDestroyContext");
    }
    else
    {
        EGL_ERROR_LOG("eglDestroyContext");
    }

    if (eglTerminate(m_display))
    {
        EGL_STATUS_LOG("eglTerminate");
    }
    else
    {
        EGL_ERROR_LOG("eglTerminate");
    }
}

bool NvEGLWinUtil::setWindow(ANativeWindow* window)
{
    if (window != m_window)
    {
        LOGD("**** Window has changed!");
        destroySurface();
    }

    m_window = window;

    m_width = m_window ? ANativeWindow_getWidth(m_window) : 0;
    m_height = m_window ? ANativeWindow_getHeight(m_window) : 0;

    return true;
}

bool NvEGLWinUtil::checkWindowResized()
{
    if (!m_window)
        return false;

    int32_t w = ANativeWindow_getWidth(m_window);
    int32_t h = ANativeWindow_getHeight(m_window);
    if (m_width != w || m_height != h || mNewWindow)
    {
        m_width = w;
        m_height = h;
        mNewWindow = false;
        return true;
    }

    return false;
}

bool NvEGLWinUtil::createSurface()
{
    if (m_status >= NV_HAS_SURFACE)
        return true;

    if (!m_window)
        return false;

    if (m_status < NV_INITIALIZED)
        return false;

    ANativeWindow_setBuffersGeometry(m_window, 0, 0, m_format);

	if ( m_HDRAppFlag && m_HDRSinkFlag ) {
		EGLint surfaceAttribsBT2020PQ[] =
		{
			EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_BT2020_PQ_EXT,
			EGL_NONE
		};
		EGLint surfaceAttribsBT2020Linear[] =
		{
			EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_BT2020_LINEAR_EXT,
			EGL_NONE
		};
		EGLint surfaceAttribsscRGB[] =
		{
			EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT,
			EGL_NONE
		};
		m_surface = eglCreateWindowSurface(m_display, m_config, m_window, surfaceAttribsscRGB);
		EGL_STATUS_LOG("creating HDR window surface");

	}
	else {
		m_surface = eglCreateWindowSurface(m_display, m_config, m_window, NULL);
		EGL_STATUS_LOG("creating SDR window surface");
	}
	

    if (m_surface != EGL_NO_SURFACE)
    {
        EGL_STATUS_LOG("eglCreateWindowSurface");
        int32_t w = ANativeWindow_getWidth(m_window);
        int32_t h = ANativeWindow_getHeight(m_window);
        EGLint sw, sh;
        eglQuerySurface(m_display, m_surface, EGL_WIDTH, &sw);
        eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &sh);
        LOGD("Win=%dx%d, Surf=%dx%d", w, h, sw, sh);
        mNewWindow = true;
    }
    else
    {
        EGL_ERROR_LOG("eglCreateWindowSurface");
        return false;
    }

	if (m_HDRAppFlag && m_HDRSinkFlag)
	{
		struct Chromacities
		{
			float red_x, red_y;
			float green_x, green_y;
			float blue_x, blue_y;
			float wp_x, wp_y;
		};

		const Chromacities chromacityList[] =
		{
			{ 0.64000f, 0.33000f, 0.30000f, 0.60000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f }, // rec709
			{ 0.68000f, 0.32000f, 0.26500f, 0.69000f, 0.15000f, 0.06000f, 0.31400f, 0.35100f }, // DCI-P3
			{ 0.70800f, 0.29200f, 0.17000f, 0.79700f, 0.13100f, 0.04600f, 0.31270f, 0.32900f }, // bt2020
			{ 0.68000f, 0.32000f, 0.26500f, 0.69000f, 0.15000f, 0.06000f, 0.32168f, 0.33767f }, // D60 DCI
			{ 0.64000f, 0.33000f, 0.21000f, 0.71000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f }, // Adobe98
		};

		// Based on mastering display
		const float maxLum = 1000.0f;
		const float minLum = 0.0f;

		const Chromacities* masteringDisplayChromaticity = &(chromacityList[0]);

		int metadata[10];
		metadata[0] = (int)(masteringDisplayChromaticity->red_x * EGL_METADATA_SCALING);
		metadata[1] = (int)(masteringDisplayChromaticity->red_y * EGL_METADATA_SCALING);
		metadata[2] = (int)(masteringDisplayChromaticity->green_x * EGL_METADATA_SCALING);
		metadata[3] = (int)(masteringDisplayChromaticity->green_y * EGL_METADATA_SCALING);
		metadata[4] = (int)(masteringDisplayChromaticity->blue_x * EGL_METADATA_SCALING);
		metadata[5] = (int)(masteringDisplayChromaticity->blue_y * EGL_METADATA_SCALING);
		metadata[6] = (int)(masteringDisplayChromaticity->wp_x * EGL_METADATA_SCALING);
		metadata[7] = (int)(masteringDisplayChromaticity->wp_y * EGL_METADATA_SCALING);
		metadata[8] = (int)(maxLum * EGL_METADATA_SCALING);
		LOGD("Setting Max Luminance :         %f", maxLum);
		metadata[9] = (int)(minLum * EGL_METADATA_SCALING);
		LOGD("Setting Min Luminance :         %f", minLum);

		EGLBoolean result = false;

		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_RX_EXT, metadata[0]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting red primary X");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_RY_EXT, metadata[1]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting red primary Y");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_GX_EXT, metadata[2]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting green primary X");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_GY_EXT, metadata[3]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting green primary Y");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_BX_EXT, metadata[4]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting blue primary X");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_DISPLAY_PRIMARY_BY_EXT, metadata[5]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting blue primary Y");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_WHITE_POINT_X_EXT, metadata[6]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting white point X");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_WHITE_POINT_Y_EXT, metadata[7]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting white point Y");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_MAX_LUMINANCE_EXT, metadata[8]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting max luminance");
		result = eglSurfaceAttrib(m_display, m_surface, EGL_SMPTE2086_MIN_LUMINANCE_EXT, metadata[9]);
		if (result != EGL_TRUE) EGL_ERROR_LOG("Error setting min luminance");
	}
	
    m_status = NV_HAS_SURFACE;

    return true;
}

bool NvEGLWinUtil::destroySurface()
{
    if (m_status < NV_HAS_SURFACE)
        return true;

    if (m_status >= NV_IS_BOUND)
    {
        if(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
        {
            EGL_STATUS_LOG("eglMakeCurrent");
        }
        else
        {
            EGL_ERROR_LOG("eglMakeCurrent");
            return false;
        }
    }
    
    if (eglDestroySurface(m_display, m_surface))
    {
        EGL_STATUS_LOG("eglDestroySurface");
    }
    else
    {
        EGL_ERROR_LOG("eglDestroySurface");
        return false;
    }

    m_surface = EGL_NO_SURFACE;
    
    m_status = NV_INITIALIZED;

    return true;
}

bool NvEGLWinUtil::isReadyToRender(bool allocateIfNeeded)
{
    if (m_status >= NV_IS_BOUND)
        return true;

    if (!allocateIfNeeded)
        return false;

    // If we have not bound the context and surface, do we even _have_ a surface?
    if (m_status < NV_HAS_SURFACE)
    {
        if (m_status < NV_INITIALIZED)
            return false;

        if (!createSurface())
            return false;
    }

    // We have a surface and (possibly) context, so bind them
    if (!bind())
        return false;

    return true;
}

bool NvEGLWinUtil::bind()
{
    if (m_status >= NV_IS_BOUND)
        return true;

    if (m_status < NV_HAS_SURFACE)
        return false;

    EGLBoolean result = eglBindAPI(m_api);

    if (m_context == EGL_NO_CONTEXT) {
        EGL_STATUS_LOG("Creating context inside of bind()");
        if (!createContext())
            return false;
    }

    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context))
    {
        EGL_STATUS_LOG("eglMakeCurrent");
    }
    else
    {
        EGL_ERROR_LOG("eglMakeCurrent");
        return false;
    }

    if (!s_functionsInitialized)
        NvInitWrapperFunctions(m_api == EGL_OPENGL_API);

    m_status = NV_IS_BOUND;

    return true;
}

bool NvEGLWinUtil::unbind()
{
    if (m_status < NV_IS_BOUND)
        return true;

    EGLBoolean result = eglBindAPI(m_api);

    if (eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
    {
        EGL_STATUS_LOG("eglMakeCurrent");
    }
    else
    {
        EGL_ERROR_LOG("eglMakeCurrent");
        return false;
    }

    m_status = NV_HAS_SURFACE;

    return true;
}

bool NvEGLWinUtil::prepareThread() {
    if (eglInitialize(m_display, 0, 0))
    {
        EGL_STATUS_LOG("eglInitialize");
    }
    else
    {
        EGL_ERROR_LOG("eglInitialize");
        return false;
    }

    if (!eglBindAPI(m_api))
    {
        return false;
    }

    return true;
}

bool NvEGLWinUtil::requestResetContext() {
    if (m_context == EGL_NO_CONTEXT)
        return true;

    unbind();

    if (eglDestroyContext(m_display, m_context))
    {
        EGL_STATUS_LOG("eglDestroyContext");
    }
    else
    {
        EGL_ERROR_LOG("eglDestroyContext");
    }

    m_context = EGL_NO_CONTEXT;

    return true;
}

bool NvEGLWinUtil::swap()
{
    if (m_status < NV_IS_BOUND)
        return false;

    // Because we expect Swap to be called every frame, we only not failure
    if (!eglSwapBuffers(m_display, m_surface))
    {
        EGL_ERROR_LOG("eglSwapBuffers");
        return false;
    }

    return true;
}


bool NvEGLWinUtil::queryNVTimeSupport()
{
    if (m_nvTimeExtensionQueried)
        return true;

    if (m_status < NV_IS_BOUND)
        return false;

    m_nvTimeExtensionQueried = true;

    m_eglGetSystemTimeFrequencyNVProc = (PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC) eglGetProcAddress("eglGetSystemTimeFrequencyNV");
    m_eglGetSystemTimeNVProc = (PFNEGLGETSYSTEMTIMENVPROC) eglGetProcAddress("eglGetSystemTimeNV");

    // now, we'll proceed through a series of sanity checking.
    // if they all succeed, we'll return.
    // if any fail, we fall out of conditional tests to end of function, null pointers, and return.
    if (m_eglGetSystemTimeFrequencyNVProc &&
        m_eglGetSystemTimeNVProc)
    {
        m_eglSystemTimeFrequency = m_eglGetSystemTimeFrequencyNVProc();
        if (m_eglSystemTimeFrequency>0) // assume okay.  quick-check it works.
        {
            EGLuint64NV time1, time2;
            time1 = m_eglGetSystemTimeNVProc();
            usleep(2000); // 2ms should be MORE than sufficient, right?
            time2 = m_eglGetSystemTimeNVProc();
            if (time1 != time2) // quick sanity only...
            {
                // we've sanity checked:
                // - fn pointers non-null
                // - freq non-zero
                // - two calls to time sep'd by sleep non-equal
                // safe to return now.
                return true;
            }
        }
    }

    // fall back if we've not returned already.
    m_eglGetSystemTimeFrequencyNVProc = (PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC) NULL;
    m_eglGetSystemTimeNVProc = (PFNEGLGETSYSTEMTIMENVPROC) NULL;

    return true;
}


uint32_t NvEGLWinUtil::getSystemTime()
{
    // if the extension is available _and_ EGL is bound, we use the extension
    // If either of these cases is false, we fall back to system time
    // Both of these sources just compute deltas from the last call and accumulate
    // that to the main clock, which is returned.
    // When we change modes, we reset our base and return a fake 1ms step that one time
    queryNVTimeSupport();

    if (m_eglGetSystemTimeFrequencyNVProc && m_eglGetSystemTimeNVProc && 
        (m_status >= NV_IS_BOUND))
    {
        EGLuint64NV egltime = m_eglGetSystemTimeNVProc();

        EGLuint64NV egltimequot = egltime / m_eglSystemTimeFrequency;
        EGLuint64NV egltimerem = egltime - (m_eglSystemTimeFrequency * egltimequot);
        egltimequot *= 1000;
        egltimerem *= 1000;
        egltimerem /= m_eglSystemTimeFrequency;
        egltimequot += egltimerem;

        if (m_timeMode == NV_TIME_MODE_EGL)
        {
            m_accumulatedTime += egltimequot - m_lastQueriedTime;
        }
        else
        {
            LOGD("Switching to NV Extension timer mode");

            m_accumulatedTime++; // don't return zero delta in this case
        }
        
        m_timeMode = NV_TIME_MODE_EGL;
        m_lastQueriedTime = egltimequot;
    }
    else
    {
        // Extension not supported - use system time
        struct timeval endTime;
        gettimeofday(&endTime, 0);
        uint32_t currTime = (endTime.tv_sec - m_startTime.tv_sec) * 1000;
        currTime += (endTime.tv_usec - m_startTime.tv_usec) / 1000;


        if (m_timeMode == NV_TIME_MODE_SYSTEM)
        {
            m_accumulatedTime += currTime - m_lastQueriedTime;
        }
        else
        {
            LOGD("Switching to System timer mode");
            m_accumulatedTime++; // don't return zero delta in this case
        }
        
        m_timeMode = NV_TIME_MODE_SYSTEM;
        m_lastQueriedTime = currTime;
    }

    return m_accumulatedTime;
}

NvGLExtensionsAPI::GLproc NvEGLWinUtil::getProcAddress(const char* procname) {
	return eglGetProcAddress(procname);
}

void NvEGLWinUtil::getConfiguration(NvGLConfiguration& config) {
	eglGetConfigAttrib(m_display, m_config, EGL_RED_SIZE, (EGLint *)&config.redBits);
	eglGetConfigAttrib(m_display, m_config, EGL_GREEN_SIZE, (EGLint *)&config.greenBits);
	eglGetConfigAttrib(m_display, m_config, EGL_BLUE_SIZE, (EGLint *)&config.blueBits);
	eglGetConfigAttrib(m_display, m_config, EGL_ALPHA_SIZE, (EGLint *)&config.alphaBits);
	eglGetConfigAttrib(m_display, m_config, EGL_DEPTH_SIZE, (EGLint *)&config.depthBits);
	eglGetConfigAttrib(m_display, m_config, EGL_STENCIL_SIZE, (EGLint *)&config.stencilBits);

	config.apiVer.api = (m_api == EGL_OPENGL_API)
		? NvGLAPI::GL : NvGLAPI::GLES;
	config.apiVer.majVersion = m_majVer;
	config.apiVer.minVersion = m_minVer;
}

#endif