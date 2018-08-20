//----------------------------------------------------------------------------------
// File:        NvImage/BlockDXT.cpp
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

#include "ColorBlock.h"
#include "BlockDXT.h"

using namespace nv;


/*----------------------------------------------------------------------------
    BlockDXT1
----------------------------------------------------------------------------*/

uint32_t BlockDXT1::evaluatePalette(Color32 color_array[4]) const
{
    // Does bit expansion before interpolation.
    color_array[0].b = (col0.b << 3) | (col0.b >> 2);
    color_array[0].g = (col0.g << 2) | (col0.g >> 4);
    color_array[0].r = (col0.r << 3) | (col0.r >> 2);
    color_array[0].a = 0xFF;
    
    // @@ Same as above, but faster?
//    Color32 c;
//    c.u = ((col0.u << 3) & 0xf8) | ((col0.u << 5) & 0xfc00) | ((col0.u << 8) & 0xf80000);
//    c.u |= (c.u >> 5) & 0x070007;
//    c.u |= (c.u >> 6) & 0x000300;
//    color_array[0].u = c.u;
    
    color_array[1].r = (col1.r << 3) | (col1.r >> 2);
    color_array[1].g = (col1.g << 2) | (col1.g >> 4);
    color_array[1].b = (col1.b << 3) | (col1.b >> 2);
    color_array[1].a = 0xFF;
    
    // @@ Same as above, but faster?
//    c.u = ((col1.u << 3) & 0xf8) | ((col1.u << 5) & 0xfc00) | ((col1.u << 8) & 0xf80000);
//    c.u |= (c.u >> 5) & 0x070007;
//    c.u |= (c.u >> 6) & 0x000300;
//    color_array[1].u = c.u;
    
    if( col0.u > col1.u ) {
        // Four-color block: derive the other two colors.
        color_array[2].r = (2 * color_array[0].r + color_array[1].r) / 3;
        color_array[2].g = (2 * color_array[0].g + color_array[1].g) / 3;
        color_array[2].b = (2 * color_array[0].b + color_array[1].b) / 3;
        color_array[2].a = 0xFF;
        
        color_array[3].r = (2 * color_array[1].r + color_array[0].r) / 3;
        color_array[3].g = (2 * color_array[1].g + color_array[0].g) / 3;
        color_array[3].b = (2 * color_array[1].b + color_array[0].b) / 3;
        color_array[3].a = 0xFF;
        
        return 4;
    }
    else {
        // Three-color block: derive the other color.
        color_array[2].r = (color_array[0].r + color_array[1].r) / 2;
        color_array[2].g = (color_array[0].g + color_array[1].g) / 2;
        color_array[2].b = (color_array[0].b + color_array[1].b) / 2;
        color_array[2].a = 0xFF;
        
        // Set all components to 0 to match DXT specs.
        color_array[3].r = 0x00; // color_array[2].r;
        color_array[3].g = 0x00; // color_array[2].g;
        color_array[3].b = 0x00; // color_array[2].b;
        color_array[3].a = 0x00;
        
        return 3;
    }
}

// Evaluate palette assuming 3 color block.
void BlockDXT1::evaluatePalette3(Color32 color_array[4]) const
{
    color_array[0].b = (col0.b << 3) | (col0.b >> 2);
    color_array[0].g = (col0.g << 2) | (col0.g >> 4);
    color_array[0].r = (col0.r << 3) | (col0.r >> 2);
    color_array[0].a = 0xFF;
    
    color_array[1].r = (col1.r << 3) | (col1.r >> 2);
    color_array[1].g = (col1.g << 2) | (col1.g >> 4);
    color_array[1].b = (col1.b << 3) | (col1.b >> 2);
    color_array[1].a = 0xFF;
    
    // Three-color block: derive the other color.
    color_array[2].r = (color_array[0].r + color_array[1].r) / 2;
    color_array[2].g = (color_array[0].g + color_array[1].g) / 2;
    color_array[2].b = (color_array[0].b + color_array[1].b) / 2;
    color_array[2].a = 0xFF;
        
    // Set all components to 0 to match DXT specs.
    color_array[3].r = 0x00; // color_array[2].r;
    color_array[3].g = 0x00; // color_array[2].g;
    color_array[3].b = 0x00; // color_array[2].b;
    color_array[3].a = 0x00;
}

// Evaluate palette assuming 4 color block.
void BlockDXT1::evaluatePalette4(Color32 color_array[4]) const
{
    color_array[0].b = (col0.b << 3) | (col0.b >> 2);
    color_array[0].g = (col0.g << 2) | (col0.g >> 4);
    color_array[0].r = (col0.r << 3) | (col0.r >> 2);
    color_array[0].a = 0xFF;
    
    color_array[1].r = (col1.r << 3) | (col1.r >> 2);
    color_array[1].g = (col1.g << 2) | (col1.g >> 4);
    color_array[1].b = (col1.b << 3) | (col1.b >> 2);
    color_array[1].a = 0xFF;
    
    // Four-color block: derive the other two colors.
    color_array[2].r = (2 * color_array[0].r + color_array[1].r) / 3;
    color_array[2].g = (2 * color_array[0].g + color_array[1].g) / 3;
    color_array[2].b = (2 * color_array[0].b + color_array[1].b) / 3;
    color_array[2].a = 0xFF;
        
    color_array[3].r = (2 * color_array[1].r + color_array[0].r) / 3;
    color_array[3].g = (2 * color_array[1].g + color_array[0].g) / 3;
    color_array[3].b = (2 * color_array[1].b + color_array[0].b) / 3;
    color_array[3].a = 0xFF;
}

void BlockDXT1::decodeBlock(ColorBlock * block) const
{
    // Decode color block.
    Color32 color_array[4];
    evaluatePalette(color_array);
    
    // Write color block.
    for( uint32_t j = 0; j < 4; j++ ) {
        for( uint32_t i = 0; i < 4; i++ ) {
            uint32_t idx = (row[j] >> (2 * i)) & 3;
            block->color(i, j) = color_array[idx];
        }
    }    
}

void BlockDXT1::setIndices(int * idx)
{
    indices = 0;
    for(uint32_t i = 0; i < 16; i++) {
        indices |= (idx[i] & 3) << (2 * i);
    }
}


/*----------------------------------------------------------------------------
    BlockDXT3
----------------------------------------------------------------------------*/

void BlockDXT3::decodeBlock(ColorBlock * block) const
{
    // Decode color.
    color.decodeBlock(block);
    
    // Decode alpha.
    alpha.decodeBlock(block);
}

void AlphaBlockDXT3::decodeBlock(ColorBlock * block) const
{
    block->color(0x0).a = (alpha0 << 4) | alpha0;
    block->color(0x1).a = (alpha1 << 4) | alpha1;
    block->color(0x2).a = (alpha2 << 4) | alpha2;
    block->color(0x3).a = (alpha3 << 4) | alpha3;
    block->color(0x4).a = (alpha4 << 4) | alpha4;
    block->color(0x5).a = (alpha5 << 4) | alpha5;
    block->color(0x6).a = (alpha6 << 4) | alpha6;
    block->color(0x7).a = (alpha7 << 4) | alpha7;
    block->color(0x8).a = (alpha8 << 4) | alpha8;
    block->color(0x9).a = (alpha9 << 4) | alpha9;
    block->color(0xA).a = (alphaA << 4) | alphaA;
    block->color(0xB).a = (alphaB << 4) | alphaB;
    block->color(0xC).a = (alphaC << 4) | alphaC;
    block->color(0xD).a = (alphaD << 4) | alphaD;
    block->color(0xE).a = (alphaE << 4) | alphaE;
    block->color(0xF).a = (alphaF << 4) | alphaF;
}

/*----------------------------------------------------------------------------
    BlockDXT5
----------------------------------------------------------------------------*/

void AlphaBlockDXT5::evaluatePalette(uint8_t alpha[8]) const
{
    if (alpha0 > alpha1) {
        evaluatePalette8(alpha);
    }
    else {
        evaluatePalette6(alpha);
    }
}

void AlphaBlockDXT5::evaluatePalette8(uint8_t alpha[8]) const
{
    // 8-alpha block:  derive the other six alphas.
    // Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
    alpha[0] = alpha0;
    alpha[1] = alpha1;
    alpha[2] = (6 * alpha[0] + 1 * alpha[1]) / 7;    // bit code 010
    alpha[3] = (5 * alpha[0] + 2 * alpha[1]) / 7;    // bit code 011
    alpha[4] = (4 * alpha[0] + 3 * alpha[1]) / 7;    // bit code 100
    alpha[5] = (3 * alpha[0] + 4 * alpha[1]) / 7;    // bit code 101
    alpha[6] = (2 * alpha[0] + 5 * alpha[1]) / 7;    // bit code 110
    alpha[7] = (1 * alpha[0] + 6 * alpha[1]) / 7;    // bit code 111
}

void AlphaBlockDXT5::evaluatePalette6(uint8_t alpha[8]) const
{
    // 6-alpha block.
    // Bit code 000 = alpha0, 001 = alpha1, others are interpolated.
    alpha[0] = alpha0;
    alpha[1] = alpha1;
    alpha[2] = (4 * alpha[0] + 1 * alpha[1]) / 5;    // Bit code 010
    alpha[3] = (3 * alpha[0] + 2 * alpha[1]) / 5;    // Bit code 011
    alpha[4] = (2 * alpha[0] + 3 * alpha[1]) / 5;    // Bit code 100
    alpha[5] = (1 * alpha[0] + 4 * alpha[1]) / 5;    // Bit code 101
    alpha[6] = 0x00;                            // Bit code 110
    alpha[7] = 0xFF;                            // Bit code 111
}

void AlphaBlockDXT5::indices(uint8_t index_array[16]) const
{
    index_array[0x0] = bits0;
    index_array[0x1] = bits1;
    index_array[0x2] = bits2;
    index_array[0x3] = bits3;
    index_array[0x4] = bits4;
    index_array[0x5] = bits5;
    index_array[0x6] = bits6;
    index_array[0x7] = bits7;
    index_array[0x8] = bits8;
    index_array[0x9] = bits9;
    index_array[0xA] = bitsA;
    index_array[0xB] = bitsB;
    index_array[0xC] = bitsC;
    index_array[0xD] = bitsD;
    index_array[0xE] = bitsE;
    index_array[0xF] = bitsF;
}

uint32_t AlphaBlockDXT5::index(uint32_t index) const
{
    int offset = (3 * index + 16);
    return uint32_t((this->u >> offset) & 0x7);
}

void AlphaBlockDXT5::setIndex(uint32_t index, uint32_t value)
{
    int offset = (3 * index + 16);
    uint64_t mask = uint64_t(0x7) << offset;
    this->u = (this->u & ~mask) | (uint64_t(value) << offset);
}

void AlphaBlockDXT5::decodeBlock(ColorBlock * block) const
{
    uint8_t alpha_array[8];
    evaluatePalette(alpha_array);
    
    uint8_t index_array[16];
    indices(index_array);
    
    for(uint32_t i = 0; i < 16; i++) {
        block->color(i).a = alpha_array[index_array[i]];
    }
}

void BlockDXT5::decodeBlock(ColorBlock * block) const
{
    // Decode color.
    color.decodeBlock(block);
    
    // Decode alpha.
    alpha.decodeBlock(block);

}

