//----------------------------------------------------------------------------------
// File:        NvImage/NvFilePtr.cpp
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

#define _CRT_SECURE_NO_WARNINGS

#include "NvFilePtr.h"
#include <algorithm>

#ifdef LINUX
#include <string.h>
#endif

#ifdef _WIN32
//fix non-standard naming
#define strcasecmp _stricmp
#endif

struct NvFilePtr::Rep
{
    char* data;
    char* ptr;
    size_t filesize;
    uint32_t m;
};

NvFilePtr* NvFilePtr::Create(uint8_t* block, size_t size, NvFileMode::Enum m)
{
    NvFilePtr *ptr = new NvFilePtr;
    ptr->_rep = new Rep;

    ptr->_rep->data = (char*)block;
    ptr->_rep->ptr = ptr->_rep->data;
    ptr->_rep->filesize = size;
    ptr->_rep->m = m;

    return ptr;
}

size_t NvFilePtr::Read(size_t numBytes, void* data)
{
    size_t readBytes = 0;

    size_t realRead = std::min( _rep->filesize - (_rep->ptr - _rep->data), numBytes);

    memcpy( data, _rep->ptr, realRead);
    _rep->ptr += realRead;

    readBytes = realRead;

    return readBytes;
}

size_t NvFilePtr::Write(size_t numBytes, const void* data)
{
    size_t wroteBytes = 0;

    size_t realWrite = std::min( _rep->filesize - (_rep->ptr - _rep->data), numBytes);

    memcpy( _rep->ptr, data, realWrite);
    _rep->ptr += realWrite;

    wroteBytes = realWrite;

    return wroteBytes;
}

void NvFilePtr::Flush( )
{
    // nop
}

size_t NvFilePtr::GetSize()
{
    return _rep->filesize;
}
        
NvFilePtr::~NvFilePtr()
{
    // caller owns the block - leave it

    delete _rep;
}
