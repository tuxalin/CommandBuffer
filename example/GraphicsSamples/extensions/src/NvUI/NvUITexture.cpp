//----------------------------------------------------------------------------------
// File:        NvUI/NvUITexture.cpp
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
/*
 * library for implementing base classes for NV UI framework
 * used for building Widgets and Applications on common code.
 */

#include "NvUIInternal.h"

#include "NvAssetLoader/NvAssetLoader.h"
#include "NvImage/NvImage.h"
#include "NV/NvLogs.h"

#include <stdio.h>
#include <string>

#include "NvEmbeddedAsset.h"

//======================================================================
//======================================================================
NvUITexture *NvUITexture::ms_texTable[NvUITexture::NV_UITEX_HASHMAX] = {NULL};


//======================================================================
//======================================================================
static NvImage *LoadEmbeddedTexture(const char* filename)
{
    const uint8_t *data = 0;
    uint32_t len = 0;
    if (NvEmbeddedAssetLookup(filename, data, len))
    {
        if (data!=NULL && len!=0)
        {
            // load and return image
            NvImage* image = new NvImage;
            bool result = image->loadImageFromFileData(data, len, "dds");
            if (!result) {
                delete image;
                image = NULL;
            }
            return image; // may be NULL.
        }
    }
    return NULL;
}

//======================================================================
//======================================================================
NvUITexture::NvUITexture(const std::string& texname, bool noMips/*==true*/)
: m_llnext(NULL)
, m_refcount(1)
, m_cached(false)
, m_render(NULL)
{
    // always copy in the name, so regardless of loading, we have a valid object.
    m_filename = texname;
    if (m_filename.empty())
    {
        LOGI("NvUITexture: Passed an empty texture filename");
        return;
    }
    
// TODO !!!!!TBD we shouldn't be loading in constructor, bad.  need to revise.
    NvImage *image = NULL;
    if (NULL == (image = LoadEmbeddedTexture(texname.c_str())))
        image = NvImage::CreateFromDDSFile(texname.c_str());
    if (image)
    {
        m_width = image->getWidth();
        m_height = image->getHeight();
        m_hasAlpha = image->hasAlpha();

        m_render = NvUIRenderFactory::TextureRenderCreate(image);

        delete image;
    }

    if (!m_render) // !!!!!TBD
    {
        LOGE("NvUITexture: FAILED to load texture file: %s", m_filename.c_str());
    }
    else
    {
    }
}

//======================================================================
//======================================================================
NvUITexture::~NvUITexture()
{
    // refcount owned outside of us...
    if (0==m_refcount)
    {
        delete m_render;
        m_render = NULL;
    }
    else
    {
        LOGE("NvUITexture refcount is not 0!\n");
    }
}

//======================================================================
//======================================================================
void NvUITexture::DelRef()
{
    if (m_refcount == 0)
    {
        LOGE("NvUITexture refcount is already 0!\n");
        return;
    }
    m_refcount--;

    if (m_refcount==0)
    {
        if (m_cached)
        {
            LOGE("Refcount reached 0 for a texture flagged as cached!!!\n");
        }
        delete this;
    }
    else if (m_cached && m_refcount == 1)
    {
        // cached textures got 1 more refcount so calling
        // DerefTexture when the counter reaches 1 is correct
        DerefTexture();
    }
}



//======================================================================
// STATIC MEMBER
//======================================================================
uint32_t NvUITexture::CalculateNameHash(const std::string& texname)
{
    // calculate some hash based on the filename string.
    uint32_t namehash = 0;
    std::string::size_type namelen = texname.length();
    const char* chars = texname.c_str();
    for (std::string::size_type i = 0; i<namelen; i++)
        namehash += chars[i];
    namehash = namehash % NV_UITEX_HASHMAX;
    return namehash;
}


//======================================================================
// STATIC MEMBER
//======================================================================
NvUITexture *NvUITexture::CacheTexture(const std::string& texname, bool noMips/*==true*/)
{
    NvUITexture *tex = NULL;
    
   if (texname.empty())
        return NULL; // !!!!TBD is this the right error handling?

    // find existing texture in table
    // calculate some hash based on the filename string.
    uint32_t namehash = CalculateNameHash(texname);
    // now we have hash, look in table slot, walk ll
    NvUITexture *list = ms_texTable[namehash];
    while (list)
    {
        // !!!!!TBD this isn't case insensitive, and SHOULD BE!!!
        // also possible we need full paths in order to ensure uniqueness.
        if (list->m_filename == texname)
        {
            tex = list;
            break;
        }
        list = list->m_llnext;
    }
    
    // if not exist, create a new one
    if (tex==NULL)
    {
        tex = new NvUITexture(texname, noMips);
        if (!tex)
            return NULL;
        // TODO: tex->Create...
        if (tex->m_filename != texname)
        {
            tex->DelRef();
            return NULL;
        }
            
        tex->m_cached = true;
        // then use calc'd hash info from above to insert into table.
        list = ms_texTable[namehash]; // get current head.
        tex->m_llnext = list; // move in front.
        ms_texTable[namehash] = tex; // take over head position.
    }
        
    if (tex==NULL)
    {
        // !!!!TBD error handling
        return NULL;
    }

    tex->m_refcount++; // increment regardless of cached or allocated.
    return tex;
}

//======================================================================
// STATIC MEMBER
//======================================================================
bool NvUITexture::DerefTexture()
{
    if (m_refcount==0) // we shouldn't be here
        return false;

    m_refcount--;
    if (m_refcount>0) // okay, we're still alive
        return true;

    // otherwise, we need to pull from list and REALLY delete. 
    
    // find existing texture in table
    if (m_filename.empty()) // then something created by passing in a gl ID
    { // !!!!TBD is this the right thing to do?  need to look at use-case.
        delete this;
        return true;
    }
    else // created by loading from filename, should be in list.
    {
        // calculate some hash based on the filename string.
        uint32_t namehash = CalculateNameHash(m_filename);
        // now we have hash, look in table slot, walk ll
        NvUITexture *list = ms_texTable[namehash], *follow = NULL;
        while (list)
        {
            // !!!!!TBD this isn't case insensitive, and SHOULD BE!!!
            // also possible we need full paths in order to ensure uniqueness.
            if (m_filename == list->m_filename)
            {
                // pull out of list.
                if (follow==NULL) // head of slot
                    ms_texTable[namehash] = list->m_llnext;
                else
                    follow->m_llnext = list->m_llnext;
                // now we can safely, REALLY delete    
                delete list;
                return true; // all done.
            }
            follow = list;
            list = list->m_llnext;
        }
    }

    return false;
}
