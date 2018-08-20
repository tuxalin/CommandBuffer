//----------------------------------------------------------------------------------
// File:        NV/NvString.h
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

#ifndef NV_STRING_H
#define NV_STRING_H

#include <NvSimpleTypes.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>

/// \file
/// Cross-platform string handling for 'printf' needs.

#ifdef _WIN32
#define safe_vsnprintf(buf, sz, mx, fmt, ap) vsnprintf_s(buf, sz, mx, fmt, ap)
#else
#define safe_vsnprintf(buf, sz, mx, fmt, ap) vsnprintf(buf, sz, fmt, ap)
#endif

#ifdef _WIN32
// redefine posix standards to things windows will like...
#define strtok_r    strtok_s
#endif

inline void NvStringPrint(char *outbuf, uint32_t outsize, const char* fmt, ...) {
    va_list ap;  
    va_start(ap, fmt);
    safe_vsnprintf(outbuf, outsize-1, outsize-2, fmt, ap);
    outbuf[outsize-1] = 0; // safety null.
    va_end(ap);
}

inline void NvStringPrint(std::string s, const char* fmt, ...) {
    const int outsize = 2048;
    char outbuf[outsize];

    va_list ap;  
    va_start(ap, fmt);
    s.clear();
    safe_vsnprintf(outbuf, outsize-1, outsize-2, fmt, ap);
    outbuf[outsize-1] = 0; // safety null.
    s.append(outbuf);
    va_end(ap);
}

#endif
