//----------------------------------------------------------------------------------
// File:        NvImage/BlockDXT.h
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

#ifndef NV_IMAGE_BLOCKDXT_H
#define NV_IMAGE_BLOCKDXT_H

#include "ColorBlock.h"

namespace nv
{
    /// DXT1 block.
    struct BlockDXT1
    {
        Color16 col0;
        Color16 col1;
        union {
            uint8_t row[4];
            uint32_t indices;
        };
    
        bool isFourColorMode() const;
    
        uint32_t evaluatePalette(Color32 color_array[4]) const;
        uint32_t evaluatePaletteFast(Color32 color_array[4]) const;
        void evaluatePalette3(Color32 color_array[4]) const;
        void evaluatePalette4(Color32 color_array[4]) const;
        
        void decodeBlock(ColorBlock * block) const;
        
        void setIndices(int32_t * idx);
    };
    
    /// Return true if the block uses four color mode, false otherwise.
    inline bool BlockDXT1::isFourColorMode() const
    {
        return col0.u > col1.u;
    }
    

    /// DXT3 alpha block with explicit alpha.
    struct AlphaBlockDXT3
    {
        union {
            struct {
                uint32_t alpha0 : 4;
                uint32_t alpha1 : 4;
                uint32_t alpha2 : 4;
                uint32_t alpha3 : 4;
                uint32_t alpha4 : 4;
                uint32_t alpha5 : 4;
                uint32_t alpha6 : 4;
                uint32_t alpha7 : 4;
                uint32_t alpha8 : 4;
                uint32_t alpha9 : 4;
                uint32_t alphaA : 4;
                uint32_t alphaB : 4;
                uint32_t alphaC : 4;
                uint32_t alphaD : 4;
                uint32_t alphaE : 4;
                uint32_t alphaF : 4;
            };
            uint16_t row[4];
        };
        
        void decodeBlock(ColorBlock * block) const;
        
        void flip4();
        void flip2();
    };
    
    
    /// DXT3 block.
    struct BlockDXT3
    {
        AlphaBlockDXT3 alpha;
        BlockDXT1 color;
        
        void decodeBlock(ColorBlock * block) const;
    };
    
    
    /// DXT5 alpha block.
    struct AlphaBlockDXT5
    {
        union {
            struct {
                uint64_t alpha0 : 8;    // 8
                uint64_t alpha1 : 8;    // 16
                uint64_t bits0 : 3;    // 3 - 19
                uint64_t bits1 : 3;     // 6 - 22
                uint64_t bits2 : 3;     // 9 - 25
                uint64_t bits3 : 3;    // 12 - 28
                uint64_t bits4 : 3;    // 15 - 31
                uint64_t bits5 : 3;    // 18 - 34
                uint64_t bits6 : 3;    // 21 - 37
                uint64_t bits7 : 3;    // 24 - 40
                uint64_t bits8 : 3;    // 27 - 43
                uint64_t bits9 : 3;     // 30 - 46
                uint64_t bitsA : 3;     // 33 - 49
                uint64_t bitsB : 3;    // 36 - 52
                uint64_t bitsC : 3;    // 39 - 55
                uint64_t bitsD : 3;    // 42 - 58
                uint64_t bitsE : 3;    // 45 - 61
                uint64_t bitsF : 3;    // 48 - 64
            };
            uint64_t u;
        };
        
        void evaluatePalette(uint8_t alpha[8]) const;
        void evaluatePalette8(uint8_t alpha[8]) const;
        void evaluatePalette6(uint8_t alpha[8]) const;
        void indices(uint8_t index_array[16]) const;

        uint32_t index(uint32_t index) const;
        void setIndex(uint32_t index, uint32_t value);
        
        void decodeBlock(ColorBlock * block) const;
    };

    
    /// DXT5 block.
    struct BlockDXT5
    {
        AlphaBlockDXT5 alpha;
        BlockDXT1 color;
        
        void decodeBlock(ColorBlock * block) const;
    };

} // nv namespace

#endif // NV_IMAGE_BLOCKDXT_H
