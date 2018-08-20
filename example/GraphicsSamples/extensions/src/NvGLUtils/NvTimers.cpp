//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvTimers.cpp
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
#include <NvGLUtils/NvTimers.h>

NvGPUTimer::NV_PFNGLGENQUERIESPROC          NvGPUTimer::m_glGenQueries = NULL;
NvGPUTimer::NV_PFNGLQUERYCOUNTERPROC        NvGPUTimer::m_glQueryCounter = NULL;
NvGPUTimer::NV_PFNGLGETQUERYOBJECTUIVPROC   NvGPUTimer::m_glGetQueryObjectuiv = NULL;
NvGPUTimer::NV_PFNGLGETQUERYOBJECTUI64VPROC NvGPUTimer::m_glGetQueryObjectui64v = NULL;

static void nullGenQueries(GLsizei, GLuint *) { }
static void nullQueryCounter(GLuint, GLenum) { }
static void nullGetQueryObjectuiv(GLuint, GLenum, GLuint *) { }
static void nullGetQueryObjectui64v(GLuint, GLenum, uint64_t *) { }

void NvGPUTimer::globalInit(NvGLExtensionsAPI& api) {
    m_glGenQueries = NULL;
    m_glQueryCounter = NULL;
    m_glGetQueryObjectuiv = NULL;
    m_glGetQueryObjectui64v = NULL;

#ifdef GL_ES_VERSION_3_0
    m_glGenQueries = (NV_PFNGLGENQUERIESPROC)glGenQueries;
    m_glGetQueryObjectuiv = (NV_PFNGLGETQUERYOBJECTUIVPROC)glGetQueryObjectuiv;
#endif

#ifdef GL_VERSION_1_5
    m_glGenQueries = (NV_PFNGLGENQUERIESPROC)glGenQueries;
    m_glGetQueryObjectuiv = (NV_PFNGLGETQUERYOBJECTUIVPROC)glGetQueryObjectuiv;
#endif

#ifdef GL_VERSION_3_3
    m_glQueryCounter = (NV_PFNGLQUERYCOUNTERPROC)glQueryCounter;
    m_glGetQueryObjectui64v = (NV_PFNGLGETQUERYOBJECTUI64VPROC)glGetQueryObjectui64v;
#endif

    if (api.isExtensionSupported("GL_ARB_occlusion_query")) {
        if (!m_glGenQueries)
            m_glGenQueries = (NV_PFNGLGENQUERIESPROC)api.getGLProcAddress("glGenQueriesARB");
        if (!m_glGetQueryObjectuiv)
            m_glGetQueryObjectuiv = (NV_PFNGLGETQUERYOBJECTUIVPROC)api.getGLProcAddress("glGetQueryObjectuivARB");
    }

    if (api.isExtensionSupported("GL_EXT_occlusion_query_boolean")) {
        if (!m_glGenQueries)
            m_glGenQueries = (NV_PFNGLGENQUERIESPROC)api.getGLProcAddress("glGenQueriesEXT");
        if (!m_glGetQueryObjectuiv)
            m_glGetQueryObjectuiv = (NV_PFNGLGETQUERYOBJECTUIVPROC)api.getGLProcAddress("glGetQueryObjectuivEXT");
    }

    if (api.isExtensionSupported("GL_NV_timer_query")) {
        if (!m_glQueryCounter)
            m_glQueryCounter = (NV_PFNGLQUERYCOUNTERPROC)api.getGLProcAddress("glQueryCounterNV");
        if (!m_glGetQueryObjectui64v)
            m_glGetQueryObjectui64v = (NV_PFNGLGETQUERYOBJECTUI64VPROC)api.getGLProcAddress("glGetQueryObjectui64vNV");
    }

    if (api.isExtensionSupported("GL_ARB_timer_query")) {
        if (!m_glQueryCounter)
            m_glQueryCounter = (NV_PFNGLQUERYCOUNTERPROC)api.getGLProcAddress("glQueryCounter");
        if (!m_glGetQueryObjectui64v)
            m_glGetQueryObjectui64v = (NV_PFNGLGETQUERYOBJECTUI64VPROC)api.getGLProcAddress("glGetQueryObjectui64v");
    }

    if (!m_glGenQueries)
        m_glGenQueries = (NV_PFNGLGENQUERIESPROC)nullGenQueries;

    if (!m_glQueryCounter)
        m_glQueryCounter = (NV_PFNGLQUERYCOUNTERPROC)nullQueryCounter;

    if (!m_glGetQueryObjectuiv)
        m_glGetQueryObjectuiv = (NV_PFNGLGETQUERYOBJECTUIVPROC)nullGetQueryObjectuiv;

    if (!m_glGetQueryObjectui64v)
        m_glGetQueryObjectui64v = (NV_PFNGLGETQUERYOBJECTUI64VPROC)nullGetQueryObjectui64v;
}

