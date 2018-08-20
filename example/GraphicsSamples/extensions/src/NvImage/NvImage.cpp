//----------------------------------------------------------------------------------
// File:        NvImage/NvImage.cpp
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

#include <string.h>
#include <algorithm>
#include <NvAssert.h>
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvImage/NvImage.h"
#include "BlockDXT.h"

using std::vector;
using std::max;

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

uint32_t NvImage::formatLoaderCount = 1;

NvImage::FormatInfo NvImage::formatTable[MAX_LOADERS] = {
    { "dds", NvImage::readDDS, 0}
};

bool NvImage::vertFlip = true;
bool NvImage::m_expandDXT = true;
bool NvImage::m_supportsBGR = true;

//
//
////////////////////////////////////////////////////////////
void NvImage::VerticalFlip( bool flip) {
    vertFlip = flip;
}

//
//
////////////////////////////////////////////////////////////
NvImage::NvImage() : _width(0), _height(0), _depth(0), _levelCount(0), _layers(0), _format(NVIMAGE_RGBA),
    _internalFormat(NVIMAGE_RGBA8), _type(NVIMAGE_UNSIGNED_BYTE), _elementSize(0), _cubeMap(false), 
	_blockSize_x(4), _blockSize_y(4) {
	_dataBlock = NULL;
	_dataArrayCount = 0;
	_data = NULL;
	_dataBlockSize = 0;
}

//
//
////////////////////////////////////////////////////////////
NvImage::~NvImage() {
    freeData();
}

//
//
////////////////////////////////////////////////////////////
void NvImage::freeData() {
    delete[] _data;
    delete[] _dataBlock;
}

//
//
////////////////////////////////////////////////////////////
int32_t NvImage::getImageSize( int32_t level) const {
    bool compressed = isCompressed();
    int32_t w = _width >> level;
    int32_t h = _height >> level;
    int32_t d = _depth >> level;
    w = (w) ? w : 1;
    h = (h) ? h : 1;
    d = (d) ? d : 1;
    int32_t bw = (compressed) ? ( w - 1 ) / _blockSize_x + 1 : w;
    int32_t bh = (compressed) ? ( h - 1 ) / _blockSize_y + 1 : h;
    int32_t elementSize = _elementSize;

    return bw*bh*d*elementSize;
}


//
//
////////////////////////////////////////////////////////////
const void* NvImage::getLevel( int32_t level) const {
    return getLevel(level, NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X);
}

//
//
////////////////////////////////////////////////////////////
const void* NvImage::getLevel( int32_t level, uint32_t face) const {
    NV_ASSERT( level < _levelCount);
    NV_ASSERT( face >= NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X && face <= NVIMAGE_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    NV_ASSERT( face == NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X || _cubeMap);

    face = face - NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X;

	NV_ASSERT((face*_levelCount + level) < _dataArrayCount);

    // make sure we don't hand back a garbage pointer
    if (level >=_levelCount || (int32_t)face >= _layers)
        return NULL;

    return _data[ face*_levelCount + level];
}

uint32_t NvImage::getLevelOffset(int32_t level) const
{
	return (uint8_t*)getLevel(level) - _dataBlock;
}

uint32_t NvImage::getLevelOffset(int32_t level, uint32_t face) const
{
	return (uint8_t*)getLevel(level, face) - _dataBlock;
}

uint32_t NvImage::getLayerLevelOffset(int32_t level, int32_t slice) const
{
	return (uint8_t*)getLayerLevel(level, slice) - _dataBlock;
}

//
//
////////////////////////////////////////////////////////////
void* NvImage::getLevel( int32_t level) {
    return getLevel(level, NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X);
}

//
//
////////////////////////////////////////////////////////////
void* NvImage::getLevel( int32_t level, uint32_t face) {
    NV_ASSERT( level < _levelCount);
    NV_ASSERT( face >= NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X && face <= NVIMAGE_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    NV_ASSERT( face == NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X || _cubeMap);

    face = face - NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X;

	NV_ASSERT((face*_levelCount + level) < _dataArrayCount);

    // make sure we don't hand back a garbage pointer
    if (level >=_levelCount || (int32_t)face >= _layers)
        return NULL;

    return _data[ face*_levelCount + level];
}

//
//
////////////////////////////////////////////////////////////
const void* NvImage::getLayerLevel( int32_t level, int32_t layer) const {
    NV_ASSERT( level < _levelCount);
    NV_ASSERT( layer < _layers);


	NV_ASSERT((layer*_levelCount + level) < (int32_t)_dataArrayCount);

    // make sure we don't hand back a garbage pointer
    if (level >=_levelCount || layer >= _layers)
        return NULL;

    return _data[ layer*_levelCount + level];
}

//
//
////////////////////////////////////////////////////////////
void* NvImage::getLayerLevel( int32_t level, int32_t layer) {
    NV_ASSERT( level < _levelCount);
    NV_ASSERT( layer < _layers);


	NV_ASSERT((layer*_levelCount + level) < _dataArrayCount);

    // make sure we don't hand back a garbage pointer
    if (level >=_levelCount || layer >= _layers)
        return NULL;

    return _data[ layer*_levelCount + level];
}

//
//
////////////////////////////////////////////////////////////
bool NvImage::loadImageFromFileData(const uint8_t* fileData, size_t size, const char* fileExt) {

    //try to match by format first
    for ( uint32_t ii = 0; ii < formatLoaderCount; ii++) {
        if ( ! strcasecmp( formatTable[ii].extension, fileExt)) {
            //extension matches, load it
            return formatTable[ii].reader( fileData, size, *this);
        }
    }

    return false;
}

bool NvImage::addFileLoader(const FormatInfo& loader) {
	if (formatLoaderCount >= MAX_LOADERS) {
		return false;
	}
	for (uint32_t ii = 0; ii < formatLoaderCount; ii++) {
		if (!strcasecmp(formatTable[ii].extension, loader.extension)) {
			return false;
		}
	}

	formatTable[formatLoaderCount] = loader;
	formatLoaderCount++;

	return true;
}

//
//
////////////////////////////////////////////////////////////
void NvImage::flipSurface(uint8_t *surf, int32_t width, int32_t height, int32_t depth)
{
    uint32_t lineSize;

    depth = (depth) ? depth : 1;

    if (!isCompressed()) {
        lineSize = _elementSize * width;
        uint32_t sliceSize = lineSize * height;

        uint8_t *tempBuf = new uint8_t[lineSize];

        for ( int32_t ii = 0; ii < depth; ii++) {
            uint8_t *top = surf + ii*sliceSize;
            uint8_t *bottom = top + (sliceSize - lineSize);
    
            for ( int32_t jj = 0; jj < (height >> 1); jj++) {
                memcpy( tempBuf, top, lineSize);
                memcpy( top, bottom, lineSize);
                memcpy( bottom, tempBuf, lineSize);

                top += lineSize;
                bottom -= lineSize;
            }
        }

        delete []tempBuf;
    }
    else
    {
        void (*flipblocks)(uint8_t*, uint32_t);
        width = (width + 3) / 4;
        height = (height + 3) / 4;
        uint32_t blockSize = 0;

        switch (_format)
        {
            case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1: 
                blockSize = 8;
                flipblocks = &NvImage::flip_blocks_dxtc1; 
                break;
            case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3: 
                blockSize = 16;
                flipblocks = &NvImage::flip_blocks_dxtc3; 
                break;
            case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5: 
                blockSize = 16;
                flipblocks = &NvImage::flip_blocks_dxtc5; 
                break;
            case NVIMAGE_COMPRESSED_LUMINANCE_LATC1:
            case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_LATC1:
            case NVIMAGE_COMPRESSED_RED_RGTC1:
            case NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1:
                blockSize = 8;
                flipblocks = &NvImage::flip_blocks_bc4;
                break;
            case NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2:
            case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2:
            case NVIMAGE_COMPRESSED_RG_RGTC2:
            case NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2:
                blockSize = 16;
                flipblocks = &NvImage::flip_blocks_bc5;
                break;
            default:
                return;
        }

        lineSize = width * blockSize;
        uint8_t *tempBuf = new uint8_t[lineSize];

        uint8_t *top = surf;
        uint8_t *bottom = surf + (height-1) * lineSize;

        for (uint32_t j = 0; j < std::max( (uint32_t)height >> 1, (uint32_t)1); j++)
        {
            if (top == bottom)
            {
                flipblocks(top, width);
                break;
            }

            flipblocks(top, width);
            flipblocks(bottom, width);

            memcpy( tempBuf, top, lineSize);
            memcpy( top, bottom, lineSize);
            memcpy( bottom, tempBuf, lineSize);

            top += lineSize;
            bottom -= lineSize;
        }
        delete []tempBuf;
    }
}    

//
//
////////////////////////////////////////////////////////////
uint32_t NvImage::componentSwapSurface(uint8_t *surf, int32_t width, int32_t height, int32_t depth)
{
    depth = (depth) ? depth : 1;

    if (_type != NVIMAGE_UNSIGNED_BYTE)
        return _format;
    if (isCompressed())
        return _format;

    if (_format == NVIMAGE_BGR) {
        for ( int32_t ii = 0; ii < depth; ii++) {    
            for ( int32_t jj = 0; jj < height; jj++) {
                for ( int32_t kk = 0; kk < width; kk++) {
                    uint8_t tmp = surf[0];
                    surf[0] = surf[2];
                    surf[2] = tmp;
                    surf += 3;
                }
            }
        }
        return NVIMAGE_RGB;
    } else if (_format == NVIMAGE_BGRA) {
        for ( int32_t ii = 0; ii < depth; ii++) {    
            for ( int32_t jj = 0; jj < height; jj++) {
                for ( int32_t kk = 0; kk < width; kk++) {
                    uint8_t tmp = surf[0];
                    surf[0] = surf[2];
                    surf[2] = tmp;
                    surf += 4;
                }
            }
        }
        return NVIMAGE_RGBA;
    }
    return _format;
}    

static inline int32_t min(int32_t a, int32_t b) {
    return (a <= b) ? a : b;
}

//
//
////////////////////////////////////////////////////////////
void NvImage::expandDXT(uint8_t *surf, int32_t width, int32_t height, int32_t depth, int32_t srcSize)
{
    depth = (depth) ? depth : 1;

	uint32_t* dest = (uint32_t*)surf;
    uint32_t* plane = dest;

	uint8_t* srcBlock = new uint8_t[srcSize];
	uint8_t* src = srcBlock;
	memcpy(src, surf, srcSize);

    int32_t bh = (height + 3) / 4;
    int32_t bw = (width + 3) / 4;

    if (_format == NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1) {
        for (int32_t k = 0; k < depth; k++) {

            for (int32_t j = 0; j < bh; j++) {
                int32_t yBlockSize = min(4, height - 4 * j);

                for (int32_t i = 0; i < bw; i++) {
                    int32_t xBlockSize = min(4, width - 4 * i);
                    nv::BlockDXT1* block = (nv::BlockDXT1*)src;
                    nv::ColorBlock color;

                    block->decodeBlock(&color);

                    // Write color block.
                    for (int32_t y = 0; y < yBlockSize; y++) {
                        for (int32_t x = 0; x < xBlockSize; x++) {
                            plane[4*i+x + (4*j+y)*width] = (uint32_t)color.color(x, y);
                        }
                    }

                    src += sizeof(nv::BlockDXT1); // 64bits
                }
            }

            plane += width * height;
        }
    } else if (_format == NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3) {
        for (int32_t k = 0; k < depth; k++) {

            for (int32_t j = 0; j < bh; j++) {
                int32_t yBlockSize = min(4, height - 4 * j);

                for (int32_t i = 0; i < bw; i++) {
                    int32_t xBlockSize = min(4, width - 4 * i);
                    nv::BlockDXT3* block = (nv::BlockDXT3*)src;
                    nv::ColorBlock color;

                    block->decodeBlock(&color);

                    // Write color block.
                    for (int32_t y = 0; y < yBlockSize; y++) {
                        for (int32_t x = 0; x < xBlockSize; x++) {
                            plane[4*i+x + (4*j+y)*width] = (uint32_t)color.color(x, y);
                        }
                    }

                    src += sizeof(nv::BlockDXT3); // 64bits
                }
            }

            plane += width * height;
        }
    } else {
        for (int32_t k = 0; k < depth; k++) {

            for (int32_t j = 0; j < bh; j++) {
                int32_t yBlockSize = min(4U, height - 4 * j);

                for (int32_t i = 0; i < bw; i++) {
                    int32_t xBlockSize = min(4, width - 4 * i);
                    nv::BlockDXT5* block = (nv::BlockDXT5*)src;
                    nv::ColorBlock color;

                    block->decodeBlock(&color);

                    // Write color block.
                    for (int32_t y = 0; y < yBlockSize; y++) {
                        for (int32_t x = 0; x < xBlockSize; x++) {
                            plane[4*i+x + (4*j+y)*width] = (uint32_t)color.color(x, y);
                        }
                    }

                    src += sizeof(nv::BlockDXT5); // 64bits
                }
            }

            plane += width * height;
        }
    }

	delete[] srcBlock;
}

//
//
////////////////////////////////////////////////////////////
bool NvImage::convertCrossToCubemap() {
    //can't already be a cubemap
    if (isCubeMap())
        return false;

    //mipmaps are not supported
    if (_levelCount != 1)
        return false;

    //compressed textures are not supported
    if (isCompressed())
        return false;

    //this function only supports vertical cross format for now (3 wide by 4 high)
    if (  (_width / 3 != _height / 4) || (_width % 3 != 0) || (_height % 4 != 0) || (_depth != 0))
        return false;

    //get the source data
    uint8_t *data = _data[0];

    int32_t fWidth = _width / 3;
    int32_t fHeight = _height / 4;

    //remove the old pointer from the vector
	delete[] _data;
	_data = new uint8_t*[6];
	_dataArrayCount = 6;

	_dataBlockSize = fWidth * fHeight * _elementSize * 6;
	_dataBlock = new uint8_t[_dataBlockSize];
    
    //extract the faces
	uint8_t *ptr = _dataBlock;

    // positive X
	_data[0] = ptr;
    for (int32_t j=0; j<fHeight; j++) {
        memcpy( ptr, &data[((_height - (fHeight + j + 1))*_width + 2 * fWidth) * _elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }

    // negative X
	_data[1] = ptr;
	for (int32_t j = 0; j<fHeight; j++) {
        memcpy( ptr, &data[(_height - (fHeight + j + 1))*_width*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }

    // positive Y
	_data[2] = ptr;
	for (int32_t j = 0; j<fHeight; j++) {
        memcpy( ptr, &data[((4 * fHeight - j - 1)*_width + fWidth)*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }

    // negative Y
	_data[3] = ptr;
	for (int32_t j = 0; j<fHeight; j++) {
        memcpy( ptr, &data[((2*fHeight - j - 1)*_width + fWidth)*_elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }

    // positive Z
	_data[4] = ptr;
	for (int32_t j = 0; j<fHeight; j++) {
        memcpy( ptr, &data[((_height - (fHeight + j + 1))*_width + fWidth) * _elementSize], fWidth*_elementSize);
        ptr += fWidth*_elementSize;
    }

    // negative Z
	_data[5] = ptr;
	for (int32_t j = 0; j<fHeight; j++) {
        for (int32_t i=0; i<fWidth; i++) {
            memcpy( ptr, &data[(j*_width + 2 * fWidth - (i + 1))*_elementSize], _elementSize);
            ptr += _elementSize;
        }
    }

    //set the new # of faces, width and height
    _layers = 6;
    _width = fWidth;
    _height = fHeight;
    _cubeMap = true;

    //delete the old pointer
    delete []data;

    return true;
}

//
//
////////////////////////////////////////////////////////////
bool NvImage::reformatImage(int32_t width, int32_t height, uint32_t format, uint32_t type) {
    //check parameters before destroying the old image
    int32_t elementSize;
    uint32_t internalFormat;

    switch (format) {
    case NVIMAGE_ALPHA:
        switch (type) {
        case NVIMAGE_UNSIGNED_BYTE:
            internalFormat = NVIMAGE_ALPHA8;
            elementSize = 1;
            break;
        case NVIMAGE_UNSIGNED_SHORT:
            internalFormat = NVIMAGE_ALPHA16;
            elementSize = 2;
            break;
        case NVIMAGE_FLOAT:
            internalFormat = NVIMAGE_ALPHA32F;
            elementSize = 4;
            break;
        case NVIMAGE_HALF_FLOAT:
            internalFormat = NVIMAGE_ALPHA16F;
            elementSize = 2;
            break;
        default:
            return false; //format/type combo not supported
        }
        break;
    case NVIMAGE_LUMINANCE:
        switch (type) {
        case NVIMAGE_UNSIGNED_BYTE:
            internalFormat = NVIMAGE_LUMINANCE8;
            elementSize = 1;
            break;
        case NVIMAGE_UNSIGNED_SHORT:
            internalFormat = NVIMAGE_LUMINANCE16;
            elementSize = 2;
            break;
        case NVIMAGE_FLOAT:
            internalFormat = NVIMAGE_LUMINANCE32F;
            elementSize = 4;
            break;
        case NVIMAGE_HALF_FLOAT:
            internalFormat = NVIMAGE_LUMINANCE16F;
            elementSize = 2;
            break;
        default:
            return false; //format/type combo not supported
        }
        break;
    case NVIMAGE_LUMINANCE_ALPHA:
        switch (type) {
        case NVIMAGE_UNSIGNED_BYTE:
            internalFormat = NVIMAGE_LUMINANCE8_ALPHA8;
            elementSize = 2;
            break;
        case NVIMAGE_UNSIGNED_SHORT:
            internalFormat = NVIMAGE_LUMINANCE16_ALPHA16;
            elementSize = 4;
            break;
        case NVIMAGE_FLOAT:
            internalFormat = NVIMAGE_LUMINANCE_ALPHA32F;
            elementSize = 8;
            break;
        case NVIMAGE_HALF_FLOAT:
            internalFormat = NVIMAGE_LUMINANCE_ALPHA16F;
            elementSize = 4;
            break;
        default:
            return false; //format/type combo not supported
        }
        break;
    case NVIMAGE_RGB:
        switch (type) {
        case NVIMAGE_UNSIGNED_BYTE:
            internalFormat = NVIMAGE_RGB8;
            elementSize = 3;
            break;
        case NVIMAGE_UNSIGNED_SHORT:
            internalFormat = NVIMAGE_RGB16;
            elementSize = 6;
            break;
        case NVIMAGE_FLOAT:
            internalFormat = NVIMAGE_RGB32F;
            elementSize = 12;
            break;
        case NVIMAGE_HALF_FLOAT:
            internalFormat = NVIMAGE_RGB16F;
            elementSize = 6;
            break;
        default:
            return false; //format/type combo not supported
        }
        break;
    case NVIMAGE_RGBA:
        switch (type) {
        case NVIMAGE_UNSIGNED_BYTE:
            internalFormat = NVIMAGE_RGBA8;
            elementSize = 4;
            break;
        case NVIMAGE_UNSIGNED_SHORT:
            internalFormat = NVIMAGE_RGBA16;
            elementSize = 8;
            break;
        case NVIMAGE_FLOAT:
            internalFormat = NVIMAGE_RGBA32F;
            elementSize = 16;
            break;
        case NVIMAGE_HALF_FLOAT:
            internalFormat = NVIMAGE_RGBA16F;
            elementSize = 8;
            break;
        default:
            return false; //format/type combo not supported
        }
        break;
    default:
        //bad format
        return false;
        break;
    }

    //clear old data
    freeData();

    _dataBlockSize = width*height*elementSize;
    _dataBlock = new uint8_t[_dataBlockSize];

    _data = new uint8_t*[1];
    _data[0] = _dataBlock;
    _dataArrayCount = 1;


    _width = width;
    _height = height;
    _elementSize = elementSize;
    _internalFormat = internalFormat;
    _levelCount = 1;
    _layers = 1;
    _depth = 0;
    _format = format;
    _type = type;
    _cubeMap = false;

    return true;
}

//
//
////////////////////////////////////////////////////////////
bool NvImage::setImage( int32_t width, int32_t height, uint32_t format, uint32_t type, const void* data){
    if (!reformatImage(width, height, format, type))
        return false;

    memcpy(_dataBlock, data, _dataBlockSize);

    return true;
}

bool NvImage::isCompressed() const {
    switch(_format) {
        case NVIMAGE_COMPRESSED_RGB_S3TC_DXT1:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5:
        case NVIMAGE_COMPRESSED_LUMINANCE_LATC1:
        case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_LATC1:
        case NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2:
        case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2:
        case NVIMAGE_COMPRESSED_RG_RGTC2:
        case NVIMAGE_COMPRESSED_RED_RGTC1:
        case NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2:
        case NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_4x4:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_5x4:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_5x5:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_6x5:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_6x6:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_8x5:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_8x6:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_8x8:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_10x5:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_10x6:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_10x8:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_10x10:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_12x10:
		case NVIMAGE_COMPRESSED_RGBA_ASTC_12x12:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10:
		case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12:
		return true;
    }
    return false;
}

bool NvImage::hasAlpha() const {
    switch(_format) {
        case NVIMAGE_COMPRESSED_RGB_S3TC_DXT1:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3:
        case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5:
        case NVIMAGE_COMPRESSED_LUMINANCE_LATC1:
        case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_LATC1:
        case NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2:
        case NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2:
        case NVIMAGE_ALPHA:
        case NVIMAGE_LUMINANCE_ALPHA:
        case NVIMAGE_RGBA:
        case NVIMAGE_RGBA_INTEGER:
        case NVIMAGE_BGRA:
            return true;
    }
    return false;
}

NvImage* NvImage::CreateFromDDSFile(const char* filename) {
    int32_t len;
    char* ddsData = NvAssetLoaderRead(filename, len);

    if (!ddsData)
        return NULL;

    NvImage* image = new NvImage;
    bool result = image->loadImageFromFileData((const uint8_t*)ddsData, len, "dds");

    NvAssetLoaderFree(ddsData);
    if (!result) {
        delete image;
        image = NULL;
    }

    return image;
}
