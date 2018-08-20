//----------------------------------------------------------------------------------
// File:        NvAppBase\android/NvAndroidWinUtil.h
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
#ifndef NV_AND_WIN_UTIL_H
#define NV_AND_WIN_UTIL_H

#include <NvSimpleTypes.h>
#include <android/native_window.h>

class NvAndroidWinUtil {
public:
	// Shuts down any active EGL resources and deletes
	virtual ~NvAndroidWinUtil();

	// valid only when getWindow()!=NULL
	virtual int32_t getWidth() = 0;
	virtual int32_t getHeight() = 0;

	// used to set or update the native window given to us by Android
	// Must be called with NULL when the window is destroyed
	virtual bool setWindow(ANativeWindow* window) = 0;

	// Returns true if the app has an allocated surface (bound or not)
	// returns false otherwise
	virtual bool hasSurface() = 0;

	// Returns true if the window size has changed since the last call to this
	// function, and indicates that the app needs to re-set its viewport, etc
	virtual bool checkWindowResized() = 0;

	// Returns true if the rendering surface and context are bound
	// false otherwise
	virtual bool isBound() = 0;

	// Binds the rendering context and on-screen surface; 
	// fails if there is no rendering surface available
	virtual bool bind() = 0;

	// Unbinds the current rendering context and surface
	virtual bool unbind() = 0;

	// Preps a new thread for use with EGL/GL[ES]
	virtual bool prepareThread() = 0;

	// Swaps the current on-screen surface.  Fails if the surface and context
	// are not already bounds
	virtual bool swap() = 0;

	virtual bool requestResetContext() = 0;

	// Returns true if there is a bound surface and context.
	// If allocateIfNeeded is true, then the function will 
	// attempt to allocate the rendering surface and bind the
	// surface and context if possible.  Otherwise, the app
	// takes no actions and only returns whether or not the
	// system was already ready for rendering
	virtual bool isReadyToRender(bool allocateIfNeeded = false) = 0;

	virtual bool hasContext() = 0;

	static NvAndroidWinUtil* getDefaultWindow() { return ms_defaultWindow; }

	static bool getHDRSinkFlag() { return m_HDRSinkFlag; }
	static void setHDRSinkFlag(bool flag) { m_HDRSinkFlag = flag; }
	static bool getHDRAppFlag() { return m_HDRAppFlag; }
	static void setHDRAppFlag(bool flag) { m_HDRAppFlag = flag; }

protected:
	static NvAndroidWinUtil* ms_defaultWindow;
	static bool m_HDRSinkFlag;
	static bool m_HDRAppFlag;
};
#endif
