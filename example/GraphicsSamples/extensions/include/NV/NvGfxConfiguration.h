//----------------------------------------------------------------------------------
// File:        NV/NvGfxConfiguration.h
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

#ifndef NV_GFX_CONFIGURATION_H
#define NV_GFX_CONFIGURATION_H

#include <NvSimpleTypes.h>

/// \file
/// Graphics API representation

struct NvGfxConfiguration {
	/// Inline all-elements constructor.
	/// \param[in] _apiVer the API and version information
	/// \param[in] r the red color depth in bits
	/// \param[in] g the green color depth in bits
	/// \param[in] b the blue color depth in bits
	/// \param[in] a the alpha color depth in bits
	/// \param[in] d the depth buffer depth in bits
	/// \param[in] s the stencil buffer depth in bits
	/// \param[in] msaa the msaa sample count
	NvGfxConfiguration(uint32_t r = 8, uint32_t g = 8,
		uint32_t b = 8, uint32_t a = 8,
		uint32_t d = 24, uint32_t s = 0, uint32_t msaa = 0) :
		redBits(r), greenBits(g), blueBits(b), alphaBits(a),
		depthBits(d), stencilBits(s), msaaSamples(msaa) {}

	uint32_t redBits; ///< red color channel depth in bits
	uint32_t greenBits; ///< green color channel depth in bits
	uint32_t blueBits; ///< blue color channel depth in bits
	uint32_t alphaBits; ///< alpha color channel depth in bits
	uint32_t depthBits; ///< depth buffer depth in bits
	uint32_t stencilBits; ///< stencil buffer depth in bits
	uint32_t msaaSamples; ///< MSAA samples
};

#endif
