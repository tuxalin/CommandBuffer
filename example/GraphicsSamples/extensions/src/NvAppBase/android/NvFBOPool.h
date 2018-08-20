//----------------------------------------------------------------------------------
// File:        NvAppBase\android/NvFBOPool.h
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
#ifndef NV_FBO_POOL_H
#define NV_FBO_POOL_H

#include <deque>
#include <mutex>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <NV/NvPlatformGL.h>
#include "NvGLUtils/NvSimpleFBO.h"

class NvFBOPool {
public:
    NvFBOPool(NvSimpleFBO::Desc desc, EGLDisplay dpy, int32_t num = 4);
    ~NvFBOPool();
    bool initialized();

    NvSimpleFBO* deque();
    void enque(NvSimpleFBO* fbo);

    class NvFBOHolder {
    public:
        NvFBOHolder(NvSimpleFBO::Desc desc);
        NvFBOHolder(NvSimpleFBO *fbo, EGLSyncKHR sync);
        ~NvFBOHolder();
        NvSimpleFBO* detachFBO();
        EGLSyncKHR detachFence();
    protected:
        NvSimpleFBO *mFBO;
        EGLSyncKHR msync;
    };

protected:
    std::deque<NvFBOHolder*> mFBOqueue;
    std::mutex mMutex;
    EGLDisplay mDisplay;
    PFNEGLCREATESYNCKHRPROC createSync;
    PFNEGLDESTROYSYNCKHRPROC destroySync;
    PFNEGLCLIENTWAITSYNCKHRPROC clientWaitSync;
};

#endif
