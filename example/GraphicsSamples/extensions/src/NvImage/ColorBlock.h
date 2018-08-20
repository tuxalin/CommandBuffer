//----------------------------------------------------------------------------------
// File:        NvImage/ColorBlock.h
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

#ifndef NV_IMAGE_COLORBLOCK_H
#define NV_IMAGE_COLORBLOCK_H

#include <NvSimpleTypes.h>

namespace nv
{
    /// 32 bit color stored as RGBA.
    class Color32
    {
    public:
        Color32() { }
        Color32(const Color32 & c) : u(c.u) { }
        Color32(uint8_t R, uint8_t G, uint8_t B) { setRGBA(R, G, B, 0xFF); }
        Color32(uint8_t R, uint8_t G, uint8_t B, uint8_t A) { setRGBA( R, G, B, A); }
        explicit Color32(uint32_t U) : u(U) { }

        void setRGBA(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
        {
            r = R;
            g = G;
            b = B;
            a = A;
        }

        void setBGRA(uint8_t B, uint8_t G, uint8_t R, uint8_t A = 0xFF)
        {
            r = R;
            g = G;
            b = B;
            a = A;
        }
    
        operator uint32_t () const {
            return u;
        }
    
        union {
            struct {
                uint8_t r: 8;
                uint8_t g: 8;
                uint8_t b: 8;
                uint8_t a: 8;
            };
            uint32_t u;
        };
    };

    /// 16 bit 565 BGR color.
    class Color16
    {
    public:
        Color16() { }
        Color16(const Color16 & c) : u(c.u) { }
        explicit Color16(uint16_t U) : u(U) { }
    
        union {
            struct {
                uint16_t b : 5;
                uint16_t g : 6;
                uint16_t r : 5;
            };
            uint16_t u;
        };
    };

    /// Uncompressed 4x4 color block.
    struct ColorBlock
    {
        ColorBlock();
        ColorBlock(const uint32_t * linearImage);
        ColorBlock(const ColorBlock & block);
        
        // Accessors
        const Color32 * colors() const;

        Color32 color(uint32_t i) const;
        Color32 & color(uint32_t i);
        
        Color32 color(uint32_t x, uint32_t y) const;
        Color32 & color(uint32_t x, uint32_t y);
        
    private:
        
        Color32 m_color[4*4];
        
    };
    

    /// Get pointer to block colors.
    inline const Color32 * ColorBlock::colors() const
    {
        return m_color;
    }
    
    /// Get block color.
    inline Color32 ColorBlock::color(uint32_t i) const
    {
        return m_color[i];
    }
    
    /// Get block color.
    inline Color32 & ColorBlock::color(uint32_t i)
    {
        return m_color[i];
    }
    
    /// Get block color.
    inline Color32 ColorBlock::color(uint32_t x, uint32_t y) const
    {
        return m_color[y * 4 + x];
    }
    
    /// Get block color.
    inline Color32 & ColorBlock::color(uint32_t x, uint32_t y)
    {
        return m_color[y * 4 + x];
    }
    
} // nv namespace

#endif // NV_IMAGE_COLORBLOCK_H
