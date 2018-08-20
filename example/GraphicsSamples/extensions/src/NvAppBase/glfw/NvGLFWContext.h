//----------------------------------------------------------------------------------
// File:        NvAppBase\glfw/NvGLFWContext.h
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
#ifndef NV_GLFW_CONTEXT_VK_H
#define NV_GLFW_CONTEXT_VK_H
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include "NvAppBase/gl/NvAppContextGL.h"
#include <NV/NvLogs.h>

class NvGLFWContextGL: public NvAppContextGL {
public:
    NvGLFWContextGL(NvGLConfiguration& config, const NvPlatformCategory::Enum plat, const NvPlatformOS::Enum os) :
        NvAppContextGL(NvPlatformInfo(plat, os))
    {
        mWindow = NULL;
        // Hack - we can't query most of this back from GLFW, so we assume it all "took"
        mConfig = config;
        glfwWindowHint(GLFW_RED_BITS, config.redBits);
        glfwWindowHint(GLFW_GREEN_BITS, config.greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, config.blueBits);
        glfwWindowHint(GLFW_ALPHA_BITS, config.alphaBits);
        glfwWindowHint(GLFW_DEPTH_BITS, config.depthBits);
        glfwWindowHint(GLFW_STENCIL_BITS, config.stencilBits);
        glfwWindowHint(GLFW_SAMPLES, config.msaaSamples);
    }

    void setWindow(GLFWwindow* window) {
        mWindow = window;
    }

    bool bindContext() {
        glfwMakeContextCurrent(mWindow);
        return true;
    }

    bool unbindContext() {
        glfwMakeContextCurrent(NULL);
        return true;
    }

    bool swap() {
        glfwSwapBuffers(mWindow);
        return true;
    }

    bool setSwapInterval(int32_t interval) {
        glfwSwapInterval( interval );
        return false;
    }

    int32_t width() {
        int32_t w, h;
        glfwGetFramebufferSize(mWindow, &w, &h);
        return w;
    }

    int32_t height() {
        int32_t w, h;
        glfwGetFramebufferSize(mWindow, &w, &h);
        return h;
    }

    GLproc getGLProcAddress(const char* procname) {
        return glfwGetProcAddress(procname);
    }

    bool isExtensionSupported(const char* ext) {
        return glfwExtensionSupported(ext) ? true : false;
    }

    void setConfiguration(const NvGLConfiguration& config) { 
		mConfig = config;

		int32_t major = glfwGetWindowAttrib(mWindow, GLFW_CONTEXT_VERSION_MAJOR);
		int32_t minor = glfwGetWindowAttrib(mWindow, GLFW_CONTEXT_VERSION_MINOR);
		mConfig.apiVer = NvGLAPIVersion(NvGLAPI::GL, major, minor);

		glGetIntegerv(GL_RED_BITS, (GLint*)&mConfig.redBits);
		glGetIntegerv(GL_GREEN_BITS, (GLint*)&mConfig.greenBits);
		glGetIntegerv(GL_BLUE_BITS, (GLint*)&mConfig.blueBits);
		glGetIntegerv(GL_ALPHA_BITS, (GLint*)&mConfig.alphaBits);
		glGetIntegerv(GL_DEPTH_BITS, (GLint*)&mConfig.depthBits);
		glGetIntegerv(GL_STENCIL_BITS, (GLint*)&mConfig.stencilBits);

		bindContext();

		// initialize extensions framework
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			// Problem: glewInit failed, something is seriously wrong.
			// TODO: would be nice to use dialog here too....
			LOGE("Error: %s\n", glewGetErrorString(err));
			exit(-1);
		}
		LOGI("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

    virtual void* getCurrentPlatformContext() { 
#ifdef _WIN32
        return (void*)wglGetCurrentContext(); 
#else
        return (void*)glXGetCurrentContext(); 
#endif
    }

    virtual void* getCurrentPlatformDisplay() { 
#ifdef _WIN32
        return (void*)wglGetCurrentDC(); 
#else
        return (void*)glXGetCurrentDrawable(); 
#endif
    }

protected:
    GLFWwindow* mWindow;
};


#endif
