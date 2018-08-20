//----------------------------------------------------------------------------------
// File:        NvEGLUtil/NvEGLUtil.h
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

#ifndef _NV_EGL_UTIL_H
#define _NV_EGL_UTIL_H

#include <NvSimpleTypes.h>
#include <NvAppBase/gl/NvAppContextGL.h>
#include <../../src/NvAppBase/android/NvAndroidWinUtil.h>
#include <NV/NvPlatformGL.h>

#include <stdlib.h>
#include <time.h>
#include <android/native_window.h>
#include <EGL/eglplatform.h>
#include <EGL/egl.h>

#ifndef EGL_NV_system_time
#define EGL_NV_system_time 1
typedef khronos_uint64_t EGLuint64NV;
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeFrequencyNV(void);
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeNV(void);
#endif
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC)(void);
typedef EGLuint64NV (EGLAPIENTRYP PFNEGLGETSYSTEMTIMENVPROC)(void);
#endif

///////////////////////////////////////////////////////////////////////////////

// EGL attributes and values for FP16 format (EGL_NV_pixel_format_float)
#ifndef EGL_EXT_pixel_format_float
#define EGL_EXT_pixel_format_float 1
#define EGL_COLOR_COMPONENT_TYPE_EXT             0x3339
#define EGL_COLOR_COMPONENT_TYPE_FIXED_EXT       0x333A
#define EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT       0x333B
#endif

///////////////////////////////////////////////////////////////////////////////

// EGL Colorspace token 
#ifndef EGL_GL_COLORSPACE
#define EGL_GL_COLORSPACE                       0x309D
#endif

// EGL tokens for BT.2020 colorspace encoding (EGL_NV_gl_colorspace_bt2020)
#ifndef EGL_EXT_gl_colorspace_bt2020
#define EGL_EXT_gl_colorspace_bt2020 1
#define EGL_GL_COLORSPACE_BT2020_LINEAR_EXT      0x333F
#define EGL_GL_COLORSPACE_BT2020_PQ_EXT          0x3340
#endif

/* EGL tokens for scRGB colorspace encoding (EGL_NV_gl_colorspace_scrgb)*/
#ifndef EGL_NV_gl_colorspace_scrgb
#define EGL_NV_gl_colorspace_scrgb 1
#define EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT       0x3350
#endif

///////////////////////////////////////////////////////////////////////////////

// EGL tokens for HDR metadata encoding (EGL_NV_surface_SMPTE2086_metadata)
#ifndef EGL_EXT_surface_SMPTE2086_metadata
#define EGL_EXT_surface_SMPTE2086_metadata 1
#define EGL_SMPTE2086_DISPLAY_PRIMARY_RX_EXT       0x3341
#define EGL_SMPTE2086_DISPLAY_PRIMARY_RY_EXT       0x3342
#define EGL_SMPTE2086_DISPLAY_PRIMARY_GX_EXT       0x3343
#define EGL_SMPTE2086_DISPLAY_PRIMARY_GY_EXT       0x3344
#define EGL_SMPTE2086_DISPLAY_PRIMARY_BX_EXT       0x3345
#define EGL_SMPTE2086_DISPLAY_PRIMARY_BY_EXT       0x3346
#define EGL_SMPTE2086_WHITE_POINT_X_EXT            0x3347
#define EGL_SMPTE2086_WHITE_POINT_Y_EXT            0x3348
#define EGL_SMPTE2086_MAX_LUMINANCE_EXT            0x3349
#define EGL_SMPTE2086_MIN_LUMINANCE_EXT            0x334A
#endif

///////////////////////////////////////////////////////////////////////////////

/* Constant scale factor by which SMPTE 2086 metadata values are scaled when
* queried as integer values.
*/
#define EGL_METADATA_SCALING    50000

///////////////////////////////////////////////////////////////////////////////

class NvEGLWinUtil : public NvAndroidWinUtil {
public:
    // typedef for a function pointer that can be used by the app to find their own
    // EGL configuration.  This function, if supplied to construction, must return the
    // app's desired EGL config for the on-screen context
    // API support is a vector of the EGL_<API>_BIT flags supported by all configs ORed
    // Returns false if no useful configs are found
    typedef bool (*ConfigChooser)(EGLDisplay disp, EGLint apiSupport, EGLConfig& bestConfig, 
        EGLint& api, EGLint& apiMajVer, EGLint& apiMinVer);

    // The default config chooser function, which returns a 565 RGB config with a 16 bit depth buffer
    // and an ES2 context
    static bool defaultEGLChooser(EGLDisplay disp, EGLint apiSupport, EGLConfig& bestConfig, 
        EGLint& api, EGLint& apiMajVer, EGLint& apiMinVer);

    // returns a wrapper with a valid config on success.
    // no surface is created yet
    // A context is created but is not bound
	static NvEGLWinUtil* create(ConfigChooser chooser = &defaultEGLChooser);
    
    // Shuts down any active EGL resources and deletes
	~NvEGLWinUtil();

	// valid only when getWindow()!=NULL
	int32_t getWidth() { return m_width; }
	int32_t getHeight() { return m_height; }

	// used to set or update the native window given to us by Android
	// Must be called with NULL when the window is destroyed
	bool setWindow(ANativeWindow* window);

	// Returns true if the app has an allocated surface (bound or not)
	// returns false otherwise
	bool hasSurface() { return m_status >= NV_HAS_SURFACE; }

	// Returns true if the window size has changed since the last call to this
	// function, and indicates that the app needs to re-set its viewport, etc
	bool checkWindowResized();

	// Returns true if the rendering surface and context are bound
	// false otherwise
	bool isBound() { return m_status == NV_IS_BOUND; }

	// Binds the rendering context and on-screen surface; 
	// fails if there is no rendering surface available
	bool bind();

	// Unbinds the current rendering context and surface
	bool unbind();

	// Preps a new thread for use with EGL/GL[ES]
	bool prepareThread();

	// Swaps the current on-screen surface.  Fails if the surface and context
	// are not already bounds
	bool swap();

	bool requestResetContext();

	// Returns true if there is a bound surface and context.
	// If allocateIfNeeded is true, then the function will 
	// attempt to allocate the rendering surface and bind the
	// surface and context if possible.  Otherwise, the app
	// takes no actions and only returns whether or not the
	// system was already ready for rendering
	bool isReadyToRender(bool allocateIfNeeded = false);

	bool hasContext() { return m_context != EGL_NO_CONTEXT; }

	// Accessors for the low-level EGL/Android objects
	virtual void*  getDisplay() { return (void*)m_display; }
	virtual void*  getContext() { return (void*)m_context; }

	NvGLExtensionsAPI::GLproc getProcAddress(const char* procname);

	void getConfiguration(NvGLConfiguration& config);

protected:
    enum {
        // Numbering matters here, as we use >/< for state hierarchy
        NV_UNINITIALIZED = 0,
        NV_INITIALIZED = 1,
        NV_HAS_SURFACE = 2,
        NV_IS_BOUND = 3
    };

    enum {
        // the clock timing mode
        NV_TIME_MODE_INIT,
        NV_TIME_MODE_SYSTEM,
        NV_TIME_MODE_EGL
    };

	NvEGLWinUtil()
    { 
        m_display = EGL_NO_DISPLAY;
        m_config = NULL;
        m_context = EGL_NO_CONTEXT;
        m_surface = EGL_NO_SURFACE;
        m_window = NULL;
        m_status = NV_UNINITIALIZED;
        m_width = 0;
        m_height = 0;
        mNewWindow = true;
        m_lastQueriedTime = 0;
        m_accumulatedTime = 0;
        gettimeofday(&m_startTime, 0);
        m_eglGetSystemTimeFrequencyNVProc = NULL;
        m_eglGetSystemTimeNVProc = NULL;
        m_eglSystemTimeFrequency = 0;
        m_nvTimeExtensionQueried = false;
        m_timeMode = NV_TIME_MODE_INIT;
    }

    bool queryNVTimeSupport();

    bool createContext();

	// This clock will use the NV time extension if available, allowing the
	// time to be slowed, accelerated or stopped.  This function may return
	// the same time from multiple calls, specifically if PerfHUD ES is active
	// Units are miliseconds
	uint32_t getSystemTime();

	// Create the rendering surface.  Will fail if no valid window has been
	// passed using setWindow
	bool createSurface();

	// Unbinds and destroys the rendering surface
	bool destroySurface();

	EGLDisplay m_display;
    EGLConfig m_config;
    EGLContext m_context;
    EGLSurface m_surface;
    EGLint m_format;
    EGLint m_api;
    EGLint m_majVer;
    EGLint m_minVer;
    int32_t m_width;
    int32_t m_height;

    ANativeWindow* m_window;
    bool mNewWindow;

    uint32_t m_status;

    uint32_t m_lastQueriedTime;
    uint32_t m_accumulatedTime;
    PFNEGLGETSYSTEMTIMEFREQUENCYNVPROC m_eglGetSystemTimeFrequencyNVProc;
    PFNEGLGETSYSTEMTIMENVPROC m_eglGetSystemTimeNVProc;
    EGLuint64NV m_eglSystemTimeFrequency;
    struct timeval m_startTime;
    bool m_nvTimeExtensionQueried;
    int32_t m_timeMode;
};



#endif

