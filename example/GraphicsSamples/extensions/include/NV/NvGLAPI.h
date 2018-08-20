//----------------------------------------------------------------------------------
// File:        NV/NvGLAPI.h
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

#ifndef NV_GL_API_H
#define NV_GL_API_H

#include <NvSimpleTypes.h>

/// \file
/// Graphics API version representation

/// High-level Khronos API.
struct NvGLAPI {
    enum Enum {
        GLES, ///< OpenGL ES
        GL ///< OpenGL
    };
};

/// Representation of a graphics API and version.
/// Includes basic comparisons between objects
struct NvGLAPIVersion {
	NvGLAPIVersion(NvGLAPI::Enum _api, int32_t _majVersion, int32_t _minVersion = 0) :
        api(_api)
      , majVersion(_majVersion)
      , minVersion(_minVersion) {}
        
    ///@{
    /// Compare two APIs and versions.
    /// OpenGL ES is considered to be "less than" OpenGL
    /// If the APIs match, then the major and minor versions are compared
	bool operator<(const NvGLAPIVersion &rhs) const {
        if (api < rhs.api) return true;
        if (api > rhs.api) return false;

        if (majVersion < rhs.majVersion) return true;
        if (majVersion > rhs.majVersion) return false;

        if (minVersion < rhs.minVersion) return true;
        return false;
    }
        
	bool operator<=(const NvGLAPIVersion &rhs) const {
        if (api < rhs.api) return true;
        if (api > rhs.api) return false;

        if (majVersion < rhs.majVersion) return true;
        if (majVersion > rhs.majVersion) return false;

        if (minVersion <= rhs.minVersion) return true;
        return false;
    }
        
	bool operator>(const NvGLAPIVersion &rhs) const {
        if (api > rhs.api) return true;
        if (api < rhs.api) return false;

        if (majVersion > rhs.majVersion) return true;
        if (majVersion < rhs.majVersion) return false;

        if (minVersion > rhs.minVersion) return true;
        return false;
    }
        
	bool operator>=(const NvGLAPIVersion &rhs) const {
        if (api > rhs.api) return true;
        if (api < rhs.api) return false;

        if (majVersion > rhs.majVersion) return true;
        if (majVersion < rhs.majVersion) return false;

        if (minVersion >= rhs.minVersion) return true;
        return false;
    }
        
	bool operator==(const NvGLAPIVersion &rhs) const {
        return (api == rhs.api)  && (majVersion == rhs.majVersion) &&
            (minVersion == rhs.minVersion);
    }

	bool operator!=(const NvGLAPIVersion &rhs) const {
        return (api != rhs.api)  || (majVersion != rhs.majVersion) ||
            (minVersion != rhs.minVersion);
    }
    ///@}

	NvGLAPI::Enum api; ///< The high-level API
    int32_t majVersion; ///< The major version (X.0)
    int32_t minVersion; ///< The minor version (0.Y)
};

/// Predefined object representing OpenGL ES 2.0.
struct NvGLAPIVersionES2 : public NvGLAPIVersion {
	NvGLAPIVersionES2() : NvGLAPIVersion(NvGLAPI::GLES, 2) {}
};

/// Predefined object representing OpenGL ES 3.0.
struct NvGLAPIVersionES3 : public NvGLAPIVersion {
	NvGLAPIVersionES3() : NvGLAPIVersion(NvGLAPI::GLES, 3) {}
};

/// Predefined object representing OpenGL ES 3.1.
struct NvGLAPIVersionES3_1 : public NvGLAPIVersion {
	NvGLAPIVersionES3_1() : NvGLAPIVersion(NvGLAPI::GLES, 3, 1) {}
};

/// Predefined object representing OpenGL 4.0.
struct NvGLAPIVersionGL4 : public NvGLAPIVersion {
	NvGLAPIVersionGL4() : NvGLAPIVersion(NvGLAPI::GL, 4) {}
};

/// Predefined object representing OpenGL 4.3.
struct NvGLAPIVersionGL4_3 : public NvGLAPIVersion {
	NvGLAPIVersionGL4_3() : NvGLAPIVersion(NvGLAPI::GL, 4, 3) {}
};

/// Predefined object representing OpenGL 4.4.
struct NvGLAPIVersionGL4_4 : public NvGLAPIVersion {
	NvGLAPIVersionGL4_4() : NvGLAPIVersion(NvGLAPI::GL, 4, 4) {}
};

#endif
