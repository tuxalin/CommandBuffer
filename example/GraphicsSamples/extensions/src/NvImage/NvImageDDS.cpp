//----------------------------------------------------------------------------------
// File:        NvImage/NvImageDDS.cpp
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

#include <stdio.h>
#include <string.h>

#include "NvImage/NvImage.h"
#include "NvFilePtr.h"
#include "NV/NvLogs.h"

using std::vector;

//
//  Structure defines and constants from nvdds
//
//////////////////////////////////////////////////////////////////////

// surface description flags
const uint32_t DDSF_CAPS           = 0x00000001l;
const uint32_t DDSF_HEIGHT         = 0x00000002l;
const uint32_t DDSF_WIDTH          = 0x00000004l;
const uint32_t DDSF_PITCH          = 0x00000008l;
const uint32_t DDSF_PIXELFORMAT    = 0x00001000l;
const uint32_t DDSF_MIPMAPCOUNT    = 0x00020000l;
const uint32_t DDSF_LINEARSIZE     = 0x00080000l;
const uint32_t DDSF_DEPTH          = 0x00800000l;

// pixel format flags
const uint32_t DDSF_ALPHAPIXELS    = 0x00000001l;
const uint32_t DDSF_FOURCC         = 0x00000004l;
const uint32_t DDSF_RGB            = 0x00000040l;
const uint32_t DDSF_RGBA           = 0x00000041l;

// dwCaps1 flags
const uint32_t DDSF_COMPLEX         = 0x00000008l;
const uint32_t DDSF_TEXTURE         = 0x00001000l;
const uint32_t DDSF_MIPMAP          = 0x00400000l;

// dwCaps2 flags
const uint32_t DDSF_CUBEMAP         = 0x00000200l;
const uint32_t DDSF_CUBEMAP_POSITIVEX  = 0x00000400l;
const uint32_t DDSF_CUBEMAP_NEGATIVEX  = 0x00000800l;
const uint32_t DDSF_CUBEMAP_POSITIVEY  = 0x00001000l;
const uint32_t DDSF_CUBEMAP_NEGATIVEY  = 0x00002000l;
const uint32_t DDSF_CUBEMAP_POSITIVEZ  = 0x00004000l;
const uint32_t DDSF_CUBEMAP_NEGATIVEZ  = 0x00008000l;
const uint32_t DDSF_CUBEMAP_ALL_FACES  = 0x0000FC00l;
const uint32_t DDSF_VOLUME          = 0x00200000l;

// compressed texture types
const uint32_t FOURCC_UNKNOWN       = 0;

#ifndef MAKEFOURCC
#define MAKEFOURCC(c0,c1,c2,c3) \
    ((uint32_t)(uint8_t)(c0)| \
    ((uint32_t)(uint8_t)(c1) << 8)| \
    ((uint32_t)(uint8_t)(c2) << 16)| \
    ((uint32_t)(uint8_t)(c3) << 24))
#endif

const uint32_t FOURCC_R8G8B8        = 20;
const uint32_t FOURCC_A8R8G8B8      = 21;
const uint32_t FOURCC_X8R8G8B8      = 22;
const uint32_t FOURCC_R5G6B5        = 23;
const uint32_t FOURCC_X1R5G5B5      = 24;
const uint32_t FOURCC_A1R5G5B5      = 25;
const uint32_t FOURCC_A4R4G4B4      = 26;
const uint32_t FOURCC_R3G3B2        = 27;
const uint32_t FOURCC_A8            = 28;
const uint32_t FOURCC_A8R3G3B2      = 29;
const uint32_t FOURCC_X4R4G4B4      = 30;
const uint32_t FOURCC_A2B10G10R10   = 31;
const uint32_t FOURCC_A8B8G8R8      = 32;
const uint32_t FOURCC_X8B8G8R8      = 33;
const uint32_t FOURCC_G16R16        = 34;
const uint32_t FOURCC_A2R10G10B10   = 35;
const uint32_t FOURCC_A16B16G16R16  = 36;

const uint32_t FOURCC_L8            = 50;
const uint32_t FOURCC_A8L8          = 51;
const uint32_t FOURCC_A4L4          = 52;
const uint32_t FOURCC_DXT1          = 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
const uint32_t FOURCC_DXT2          = 0x32545844l; //(MAKEFOURCC('D','X','T','1'))
const uint32_t FOURCC_DXT3          = 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
const uint32_t FOURCC_DXT4          = 0x34545844l; //(MAKEFOURCC('D','X','T','3'))
const uint32_t FOURCC_DXT5          = 0x35545844l; //(MAKEFOURCC('D','X','T','5'))
const uint32_t FOURCC_ATI1          = MAKEFOURCC('A','T','I','1');
const uint32_t FOURCC_ATI2          = MAKEFOURCC('A','T','I','2');
const uint32_t FOURCC_BC4U          = MAKEFOURCC('B','C','4','U');
const uint32_t FOURCC_BC4S          = MAKEFOURCC('B','C','4','S');
const uint32_t FOURCC_BC5S          = MAKEFOURCC('B','C','5','S');

const uint32_t FOURCC_D16_LOCKABLE  = 70;
const uint32_t FOURCC_D32           = 71;
const uint32_t FOURCC_D24X8         = 77;
const uint32_t FOURCC_D16           = 80;

const uint32_t FOURCC_D32F_LOCKABLE = 82;

const uint32_t FOURCC_L16           = 81;

const uint32_t FOURCC_DX10          = MAKEFOURCC('D','X','1','0');

// signed normalized formats
const uint32_t FOURCC_Q16W16V16U16  = 110;

// Floating point surface formats

// s10e5 formats (16-bits per channel)
const uint32_t FOURCC_R16F          = 111;
const uint32_t FOURCC_G16R16F       = 112;
const uint32_t FOURCC_A16B16G16R16F = 113;

// IEEE s23e8 formats (32-bits per channel)
const uint32_t FOURCC_R32F          = 114;
const uint32_t FOURCC_G32R32F       = 115;
const uint32_t FOURCC_A32B32G32R32F = 116;

//DXGI enums
const uint32_t DDS10_FORMAT_UNKNOWN = 0;
const uint32_t DDS10_FORMAT_R32G32B32A32_TYPELESS = 1;
const uint32_t DDS10_FORMAT_R32G32B32A32_FLOAT = 2;
const uint32_t DDS10_FORMAT_R32G32B32A32_UINT = 3;
const uint32_t DDS10_FORMAT_R32G32B32A32_SINT = 4;
const uint32_t DDS10_FORMAT_R32G32B32_TYPELESS = 5;
const uint32_t DDS10_FORMAT_R32G32B32_FLOAT = 6;
const uint32_t DDS10_FORMAT_R32G32B32_UINT = 7;
const uint32_t DDS10_FORMAT_R32G32B32_SINT = 8;
const uint32_t DDS10_FORMAT_R16G16B16A16_TYPELESS = 9;
const uint32_t DDS10_FORMAT_R16G16B16A16_FLOAT = 10;
const uint32_t DDS10_FORMAT_R16G16B16A16_UNORM = 11;
const uint32_t DDS10_FORMAT_R16G16B16A16_UINT = 12;
const uint32_t DDS10_FORMAT_R16G16B16A16_SNORM = 13;
const uint32_t DDS10_FORMAT_R16G16B16A16_SINT = 14;
const uint32_t DDS10_FORMAT_R32G32_TYPELESS = 15;
const uint32_t DDS10_FORMAT_R32G32_FLOAT = 16;
const uint32_t DDS10_FORMAT_R32G32_UINT = 17;
const uint32_t DDS10_FORMAT_R32G32_SINT = 18;
const uint32_t DDS10_FORMAT_R32G8X24_TYPELESS = 19;
const uint32_t DDS10_FORMAT_D32_FLOAT_S8X24_UINT = 20;
const uint32_t DDS10_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21;
const uint32_t DDS10_FORMAT_X32_TYPELESS_G8X24_UINT = 22;
const uint32_t DDS10_FORMAT_R10G10B10A2_TYPELESS = 23;
const uint32_t DDS10_FORMAT_R10G10B10A2_UNORM = 24;
const uint32_t DDS10_FORMAT_R10G10B10A2_UINT = 25;
const uint32_t DDS10_FORMAT_R11G11B10_FLOAT = 26;
const uint32_t DDS10_FORMAT_R8G8B8A8_TYPELESS = 27;
const uint32_t DDS10_FORMAT_R8G8B8A8_UNORM = 28;
const uint32_t DDS10_FORMAT_R8G8B8A8_UNORM_SRGB = 29;
const uint32_t DDS10_FORMAT_R8G8B8A8_UINT = 30;
const uint32_t DDS10_FORMAT_R8G8B8A8_SNORM = 31;
const uint32_t DDS10_FORMAT_R8G8B8A8_SINT = 32;
const uint32_t DDS10_FORMAT_R16G16_TYPELESS = 33;
const uint32_t DDS10_FORMAT_R16G16_FLOAT = 34;
const uint32_t DDS10_FORMAT_R16G16_UNORM = 35;
const uint32_t DDS10_FORMAT_R16G16_UINT = 36;
const uint32_t DDS10_FORMAT_R16G16_SNORM = 37;
const uint32_t DDS10_FORMAT_R16G16_SINT = 38;
const uint32_t DDS10_FORMAT_R32_TYPELESS = 39;
const uint32_t DDS10_FORMAT_D32_FLOAT = 40;
const uint32_t DDS10_FORMAT_R32_FLOAT = 41;
const uint32_t DDS10_FORMAT_R32_UINT = 42;
const uint32_t DDS10_FORMAT_R32_SINT = 43;
const uint32_t DDS10_FORMAT_R24G8_TYPELESS = 44;
const uint32_t DDS10_FORMAT_D24_UNORM_S8_UINT = 45;
const uint32_t DDS10_FORMAT_R24_UNORM_X8_TYPELESS = 46;
const uint32_t DDS10_FORMAT_X24_TYPELESS_G8_UINT = 47;
const uint32_t DDS10_FORMAT_R8G8_TYPELESS = 48;
const uint32_t DDS10_FORMAT_R8G8_UNORM = 49;
const uint32_t DDS10_FORMAT_R8G8_UINT = 50;
const uint32_t DDS10_FORMAT_R8G8_SNORM = 51;
const uint32_t DDS10_FORMAT_R8G8_SINT = 52;
const uint32_t DDS10_FORMAT_R16_TYPELESS = 53;
const uint32_t DDS10_FORMAT_R16_FLOAT = 54;
const uint32_t DDS10_FORMAT_D16_UNORM = 55;
const uint32_t DDS10_FORMAT_R16_UNORM = 56;
const uint32_t DDS10_FORMAT_R16_UINT = 57;
const uint32_t DDS10_FORMAT_R16_SNORM = 58;
const uint32_t DDS10_FORMAT_R16_SINT = 59;
const uint32_t DDS10_FORMAT_R8_TYPELESS = 60;
const uint32_t DDS10_FORMAT_R8_UNORM = 61;
const uint32_t DDS10_FORMAT_R8_UINT = 62;
const uint32_t DDS10_FORMAT_R8_SNORM = 63;
const uint32_t DDS10_FORMAT_R8_SINT = 64;
const uint32_t DDS10_FORMAT_A8_UNORM = 65;
const uint32_t DDS10_FORMAT_R1_UNORM = 66;
const uint32_t DDS10_FORMAT_R9G9B9E5_SHAREDEXP = 67;
const uint32_t DDS10_FORMAT_R8G8_B8G8_UNORM = 68;
const uint32_t DDS10_FORMAT_G8R8_G8B8_UNORM = 69;
const uint32_t DDS10_FORMAT_BC1_TYPELESS = 70;
const uint32_t DDS10_FORMAT_BC1_UNORM = 71;
const uint32_t DDS10_FORMAT_BC1_UNORM_SRGB = 72;
const uint32_t DDS10_FORMAT_BC2_TYPELESS = 73;
const uint32_t DDS10_FORMAT_BC2_UNORM = 74;
const uint32_t DDS10_FORMAT_BC2_UNORM_SRGB = 75;
const uint32_t DDS10_FORMAT_BC3_TYPELESS = 76;
const uint32_t DDS10_FORMAT_BC3_UNORM = 77;
const uint32_t DDS10_FORMAT_BC3_UNORM_SRGB = 78;
const uint32_t DDS10_FORMAT_BC4_TYPELESS = 79;
const uint32_t DDS10_FORMAT_BC4_UNORM = 80;
const uint32_t DDS10_FORMAT_BC4_SNORM = 81;
const uint32_t DDS10_FORMAT_BC5_TYPELESS = 82;
const uint32_t DDS10_FORMAT_BC5_UNORM = 83;
const uint32_t DDS10_FORMAT_BC5_SNORM = 84;
const uint32_t DDS10_FORMAT_B5G6R5_UNORM = 85;
const uint32_t DDS10_FORMAT_B5G5R5A1_UNORM = 86;
const uint32_t DDS10_FORMAT_B8G8R8A8_UNORM = 87;
const uint32_t DDS10_FORMAT_B8G8R8X8_UNORM = 88;
const uint32_t DDS10_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89;
const uint32_t DDS10_FORMAT_B8G8R8A8_TYPELESS = 90;
const uint32_t DDS10_FORMAT_B8G8R8A8_UNORM_SRGB = 91;
const uint32_t DDS10_FORMAT_B8G8R8X8_TYPELESS = 92;
const uint32_t DDS10_FORMAT_B8G8R8X8_UNORM_SRGB = 93;
const uint32_t DDS10_FORMAT_BC6H_TYPELESS = 94;
const uint32_t DDS10_FORMAT_BC6H_UF16 = 95;
const uint32_t DDS10_FORMAT_BC6H_SF16 = 96;
const uint32_t DDS10_FORMAT_BC7_TYPELESS = 97;
const uint32_t DDS10_FORMAT_BC7_UNORM = 98;
const uint32_t DDS10_FORMAT_BC7_UNORM_SRGB = 99;
const uint32_t DDS10_FORMAT_FORCE_UINT = 0xffffffffUL;
// ASTC extension
const uint32_t DDS10_FORMAT_ASTC_4X4_UNORM              = 134;
const uint32_t DDS10_FORMAT_ASTC_4X4_UNORM_SRGB         = 135;
const uint32_t DDS10_FORMAT_ASTC_5X4_TYPELESS           = 137;
const uint32_t DDS10_FORMAT_ASTC_5X4_UNORM              = 138;
const uint32_t DDS10_FORMAT_ASTC_5X4_UNORM_SRGB         = 139;
const uint32_t DDS10_FORMAT_ASTC_5X5_TYPELESS           = 141;
const uint32_t DDS10_FORMAT_ASTC_5X5_UNORM              = 142;
const uint32_t DDS10_FORMAT_ASTC_5X5_UNORM_SRGB         = 143;
const uint32_t DDS10_FORMAT_ASTC_6X5_TYPELESS           = 145;
const uint32_t DDS10_FORMAT_ASTC_6X5_UNORM              = 146;
const uint32_t DDS10_FORMAT_ASTC_6X5_UNORM_SRGB         = 147;
const uint32_t DDS10_FORMAT_ASTC_6X6_TYPELESS           = 149;
const uint32_t DDS10_FORMAT_ASTC_6X6_UNORM              = 150;
const uint32_t DDS10_FORMAT_ASTC_6X6_UNORM_SRGB         = 151;
const uint32_t DDS10_FORMAT_ASTC_8X5_TYPELESS           = 153;
const uint32_t DDS10_FORMAT_ASTC_8X5_UNORM              = 154;
const uint32_t DDS10_FORMAT_ASTC_8X5_UNORM_SRGB         = 155;
const uint32_t DDS10_FORMAT_ASTC_8X6_TYPELESS           = 157;
const uint32_t DDS10_FORMAT_ASTC_8X6_UNORM              = 158;
const uint32_t DDS10_FORMAT_ASTC_8X6_UNORM_SRGB         = 159;
const uint32_t DDS10_FORMAT_ASTC_8X8_TYPELESS           = 161;
const uint32_t DDS10_FORMAT_ASTC_8X8_UNORM              = 162;
const uint32_t DDS10_FORMAT_ASTC_8X8_UNORM_SRGB         = 163;
const uint32_t DDS10_FORMAT_ASTC_10X5_TYPELESS          = 165;
const uint32_t DDS10_FORMAT_ASTC_10X5_UNORM             = 166;
const uint32_t DDS10_FORMAT_ASTC_10X5_UNORM_SRGB        = 167;
const uint32_t DDS10_FORMAT_ASTC_10X6_TYPELESS          = 169;
const uint32_t DDS10_FORMAT_ASTC_10X6_UNORM             = 170;
const uint32_t DDS10_FORMAT_ASTC_10X6_UNORM_SRGB        = 171;
const uint32_t DDS10_FORMAT_ASTC_10X8_TYPELESS          = 173;
const uint32_t DDS10_FORMAT_ASTC_10X8_UNORM             = 174;
const uint32_t DDS10_FORMAT_ASTC_10X8_UNORM_SRGB        = 175;
const uint32_t DDS10_FORMAT_ASTC_10X10_TYPELESS         = 177;
const uint32_t DDS10_FORMAT_ASTC_10X10_UNORM            = 178;
const uint32_t DDS10_FORMAT_ASTC_10X10_UNORM_SRGB       = 179;
const uint32_t DDS10_FORMAT_ASTC_12X10_TYPELESS         = 181;
const uint32_t DDS10_FORMAT_ASTC_12X10_UNORM            = 182;
const uint32_t DDS10_FORMAT_ASTC_12X10_UNORM_SRGB       = 183;
const uint32_t DDS10_FORMAT_ASTC_12X12_TYPELESS         = 185;
const uint32_t DDS10_FORMAT_ASTC_12X12_UNORM            = 186;
const uint32_t DDS10_FORMAT_ASTC_12X12_UNORM_SRGB       = 187;



//DDS 10 resource dimension enums
const uint32_t DDS10_RESOURCE_DIMENSION_UNKNOWN = 0;
const uint32_t DDS10_RESOURCE_DIMENSION_BUFFER = 1;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE1D = 2;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE2D = 3;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE3D = 4;


struct DXTColBlock
{
    uint16_t col0;
    uint16_t col1;

    uint8_t row[4];
};

struct DXT3AlphaBlock
{
    uint16_t row[4];
};

struct DXT5AlphaBlock
{
    uint8_t alpha0;
    uint8_t alpha1;
    
    uint8_t row[6];
};

struct DDS_PIXELFORMAT
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct DDS_HEADER
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t dwCaps1;
    uint32_t dwCaps2;
    uint32_t dwReserved2[3];
};

struct DDS_HEADER_10
{
    uint32_t dxgiFormat;  // check type
    uint32_t resourceDimension; //check type
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t reserved;
};

//
//
////////////////////////////////////////////////////////////
bool TranslateDX10Format( const void *ptr, NvImage &i, int32_t &bytesPerElement, bool &btcCompressed) {
    const DDS_HEADER_10 &header = *(const DDS_HEADER_10*)ptr;

#ifdef _DEBUG
    LOGI( "translating DX10 Format\n");
    LOGI( "  header.dxgiFormat = %x\n", header.dxgiFormat);
    LOGI( "  header.resourceDimension = %x\n", header.resourceDimension);
    LOGI( "  header.arraySize = %x\n", header.arraySize);
    LOGI( "  header.miscFlag = %x\n", header.miscFlag);
#endif

    switch (header.resourceDimension) {
        case DDS10_RESOURCE_DIMENSION_TEXTURE1D:
        case DDS10_RESOURCE_DIMENSION_TEXTURE2D:
        case DDS10_RESOURCE_DIMENSION_TEXTURE3D:
            //do I really need to do anything here ?
            break;
        case DDS10_RESOURCE_DIMENSION_UNKNOWN:
        case DDS10_RESOURCE_DIMENSION_BUFFER:
        default:
            // these are presently unsupported formats
            LOGE( "Bad resource dimension\n");
            return false;
    };


#define SET_TYPE_INFO( intf, f, t, size) \
    i._internalFormat = intf; \
    i._format = f; \
    i._type = t; \
    bytesPerElement = size; \
    btcCompressed = false;

#define SET_COMPRESSED_TYPE_INFO( intf, f, t, size) \
    i._internalFormat = intf; \
    i._format = f; \
    i._type = t; \
    bytesPerElement = size; \
    btcCompressed = true;

	i._blockSize_x=4;
	i._blockSize_y=4;
	
    switch (header.dxgiFormat) {
        case DDS10_FORMAT_R32G32B32A32_FLOAT:
            SET_TYPE_INFO( NVIMAGE_RGBA32F, NVIMAGE_RGBA, NVIMAGE_FLOAT, 16);
            break;

        case DDS10_FORMAT_R32G32B32A32_UINT:
            SET_TYPE_INFO( NVIMAGE_RGBA32UI, NVIMAGE_RGBA_INTEGER, NVIMAGE_UNSIGNED_INT, 16);
            break;

        case DDS10_FORMAT_R32G32B32A32_SINT:
            SET_TYPE_INFO( NVIMAGE_RGBA32I, NVIMAGE_RGBA_INTEGER, NVIMAGE_INT, 16);
            break;

        case DDS10_FORMAT_R32G32B32_FLOAT:
            SET_TYPE_INFO( NVIMAGE_RGBA32F, NVIMAGE_RGB, NVIMAGE_FLOAT, 12);
            break;

        case DDS10_FORMAT_R32G32B32_UINT:
            SET_TYPE_INFO( NVIMAGE_RGB32UI, NVIMAGE_RGB_INTEGER, NVIMAGE_UNSIGNED_INT, 12);
            break;

        case DDS10_FORMAT_R32G32B32_SINT:
            SET_TYPE_INFO( NVIMAGE_RGB32I, NVIMAGE_RGB_INTEGER, NVIMAGE_INT, 12);
            break;

        case DDS10_FORMAT_R16G16B16A16_FLOAT:
            SET_TYPE_INFO( NVIMAGE_RGBA16F, NVIMAGE_RGBA, NVIMAGE_HALF_FLOAT, 8);
            break;

        case DDS10_FORMAT_R16G16B16A16_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGBA16, NVIMAGE_RGBA, NVIMAGE_UNSIGNED_SHORT, 8);
            break;

        case DDS10_FORMAT_R16G16B16A16_UINT:
            SET_TYPE_INFO( NVIMAGE_RGBA16UI, NVIMAGE_RGBA_INTEGER, NVIMAGE_UNSIGNED_SHORT, 8);
            break;

        case DDS10_FORMAT_R16G16B16A16_SNORM:
            SET_TYPE_INFO( NVIMAGE_RGBA16_SNORM, NVIMAGE_RGBA, NVIMAGE_SHORT, 8);
            break;

        case DDS10_FORMAT_R16G16B16A16_SINT:
            SET_TYPE_INFO( NVIMAGE_RGBA16I, NVIMAGE_RGBA_INTEGER, NVIMAGE_SHORT, 8);
            break;
        
        case DDS10_FORMAT_R32G32_FLOAT:
            SET_TYPE_INFO( NVIMAGE_RG32F, NVIMAGE_RG, NVIMAGE_FLOAT, 8);
            break;

        case DDS10_FORMAT_R32G32_UINT:
            SET_TYPE_INFO( NVIMAGE_RG32UI, NVIMAGE_RG_INTEGER, NVIMAGE_UNSIGNED_INT, 8);
            break;

        case DDS10_FORMAT_R32G32_SINT:
            SET_TYPE_INFO( NVIMAGE_RG32I, NVIMAGE_RG_INTEGER, NVIMAGE_INT, 8);
            break;

        case DDS10_FORMAT_R32G8X24_TYPELESS:
        case DDS10_FORMAT_D32_FLOAT_S8X24_UINT:
        case DDS10_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DDS10_FORMAT_X32_TYPELESS_G8X24_UINT:
            //these formats have no real direct mapping to OpenGL
            // fail creation
            return false;
        
        case DDS10_FORMAT_R10G10B10A2_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGB10_A2, NVIMAGE_RGBA, NVIMAGE_UNSIGNED_INT_2_10_10_10_REV, 4); // is the rev version needed?
            break;

        case DDS10_FORMAT_R10G10B10A2_UINT:
            //doesn't exist in OpenGL
            return false;

        case DDS10_FORMAT_R11G11B10_FLOAT:
            SET_TYPE_INFO( NVIMAGE_R11F_G11F_B10F, NVIMAGE_RGB, NVIMAGE_UNSIGNED_INT_10F_11F_11F_REV, 4);
            break;

        case DDS10_FORMAT_R8G8B8A8_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGBA8, NVIMAGE_RGBA, NVIMAGE_UNSIGNED_BYTE, 4);
            break;

        case DDS10_FORMAT_R8G8B8A8_UNORM_SRGB:
            SET_TYPE_INFO( NVIMAGE_SRGB8_ALPHA8, NVIMAGE_RGBA, NVIMAGE_UNSIGNED_BYTE, 4);
            break;

        case DDS10_FORMAT_R8G8B8A8_UINT:
            SET_TYPE_INFO( NVIMAGE_RGBA8UI, NVIMAGE_RGBA_INTEGER, NVIMAGE_UNSIGNED_BYTE, 4);
            break;

        case DDS10_FORMAT_R8G8B8A8_SNORM:
            SET_TYPE_INFO( NVIMAGE_RGBA8_SNORM, NVIMAGE_RGBA, NVIMAGE_BYTE, 4);
            break;

        case DDS10_FORMAT_R8G8B8A8_SINT:
            SET_TYPE_INFO( NVIMAGE_RGBA8UI, NVIMAGE_RGBA_INTEGER, NVIMAGE_BYTE, 4);
            break;

        case DDS10_FORMAT_R16G16_FLOAT:
            SET_TYPE_INFO( NVIMAGE_RG16F, NVIMAGE_RG, NVIMAGE_HALF_FLOAT, 4);
            break;

        case DDS10_FORMAT_R16G16_UNORM:
            SET_TYPE_INFO( NVIMAGE_RG16, NVIMAGE_RG, NVIMAGE_UNSIGNED_SHORT, 4);
            break;

        case DDS10_FORMAT_R16G16_UINT:
            SET_TYPE_INFO( NVIMAGE_RG16UI, NVIMAGE_RG_INTEGER, NVIMAGE_UNSIGNED_SHORT, 4);
            break;

        case DDS10_FORMAT_R16G16_SNORM:
            SET_TYPE_INFO( NVIMAGE_RG16_SNORM, NVIMAGE_RG, NVIMAGE_SHORT, 4);
            break;

        case DDS10_FORMAT_R16G16_SINT:
            SET_TYPE_INFO( NVIMAGE_RG16I, NVIMAGE_RG_INTEGER, NVIMAGE_SHORT, 4);
            break;

        case DDS10_FORMAT_D32_FLOAT:
            SET_TYPE_INFO( NVIMAGE_DEPTH_COMPONENT32F, NVIMAGE_DEPTH, NVIMAGE_FLOAT, 4);
            break;

        case DDS10_FORMAT_R32_FLOAT:
            SET_TYPE_INFO( NVIMAGE_R32F, NVIMAGE_RED, NVIMAGE_FLOAT, 4);
            break;

        case DDS10_FORMAT_R32_UINT:
            SET_TYPE_INFO( NVIMAGE_R32UI, NVIMAGE_RED_INTEGER, NVIMAGE_UNSIGNED_INT, 4);
            break;

        case DDS10_FORMAT_R32_SINT:
            SET_TYPE_INFO( NVIMAGE_R32I, NVIMAGE_RED_INTEGER, NVIMAGE_INT, 4);
            break;

        //these seem a little problematic to deal with
        case DDS10_FORMAT_R24G8_TYPELESS:
        case DDS10_FORMAT_D24_UNORM_S8_UINT:
        case DDS10_FORMAT_R24_UNORM_X8_TYPELESS:
        case DDS10_FORMAT_X24_TYPELESS_G8_UINT:
            //OpenGL doesn't really offer a packed depth stencil textures
            return false;

        case DDS10_FORMAT_R8G8_UNORM:
            SET_TYPE_INFO( NVIMAGE_RG8, NVIMAGE_RG, NVIMAGE_UNSIGNED_BYTE, 2);
            break;

        case DDS10_FORMAT_R8G8_UINT:
            SET_TYPE_INFO( NVIMAGE_RG8UI, NVIMAGE_RG_INTEGER, NVIMAGE_UNSIGNED_BYTE, 2);
            break;

        case DDS10_FORMAT_R8G8_SNORM:
            SET_TYPE_INFO( NVIMAGE_RG8_SNORM, NVIMAGE_RG, NVIMAGE_BYTE, 2);
            break;

        case DDS10_FORMAT_R8G8_SINT:
            SET_TYPE_INFO( NVIMAGE_RG8I, NVIMAGE_RG_INTEGER, NVIMAGE_BYTE, 2);
            break;

        case DDS10_FORMAT_R16_FLOAT:
            SET_TYPE_INFO( NVIMAGE_R16F, NVIMAGE_RED, NVIMAGE_HALF_FLOAT, 2);
            break;

        case DDS10_FORMAT_D16_UNORM:
            SET_TYPE_INFO( NVIMAGE_DEPTH_COMPONENT16, NVIMAGE_DEPTH, NVIMAGE_UNSIGNED_SHORT, 2);
            break;

        case DDS10_FORMAT_R16_UNORM:
            SET_TYPE_INFO( NVIMAGE_R16, NVIMAGE_RED, NVIMAGE_UNSIGNED_SHORT, 2);
            break;

        case DDS10_FORMAT_R16_UINT:
            SET_TYPE_INFO( NVIMAGE_R16UI, NVIMAGE_RED_INTEGER, NVIMAGE_UNSIGNED_SHORT, 2);
            break;

        case DDS10_FORMAT_R16_SNORM:
            SET_TYPE_INFO( NVIMAGE_R16_SNORM, NVIMAGE_RED, NVIMAGE_SHORT, 2);
            return false;

        case DDS10_FORMAT_R16_SINT:
            SET_TYPE_INFO( NVIMAGE_R16I, NVIMAGE_RED_INTEGER, NVIMAGE_SHORT, 2);
            break;

        case DDS10_FORMAT_R8_UNORM:
            SET_TYPE_INFO( NVIMAGE_R8, NVIMAGE_RED, NVIMAGE_UNSIGNED_BYTE, 1);
            break;

        case DDS10_FORMAT_R8_UINT:
            SET_TYPE_INFO( NVIMAGE_R8UI, NVIMAGE_RED_INTEGER, NVIMAGE_UNSIGNED_BYTE, 1);
            break;

        case DDS10_FORMAT_R8_SNORM:
            SET_TYPE_INFO( NVIMAGE_R8_SNORM, NVIMAGE_RED, NVIMAGE_BYTE, 1);
            break;

        case DDS10_FORMAT_R8_SINT:
            SET_TYPE_INFO( NVIMAGE_R8I, NVIMAGE_RED_INTEGER, NVIMAGE_BYTE, 1);
            break;

        case DDS10_FORMAT_A8_UNORM:
            SET_TYPE_INFO( NVIMAGE_ALPHA8, NVIMAGE_ALPHA, NVIMAGE_UNSIGNED_BYTE, 1);
            break;

        case DDS10_FORMAT_R9G9B9E5_SHAREDEXP:
            SET_TYPE_INFO( NVIMAGE_RGB9_E5, NVIMAGE_RGB, NVIMAGE_UNSIGNED_INT_5_9_9_9_REV, 4);
            break;

        case DDS10_FORMAT_R8G8_B8G8_UNORM:
        case DDS10_FORMAT_G8R8_G8B8_UNORM:
            // unsure how to interpret these formats
            return false;

        case DDS10_FORMAT_BC1_UNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_UNSIGNED_BYTE, 8);
            break;

        case DDS10_FORMAT_BC1_UNORM_SRGB:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_UNSIGNED_BYTE, 8);
            break;

        case DDS10_FORMAT_BC2_UNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_BC2_UNORM_SRGB:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_BC3_UNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_BC3_UNORM_SRGB:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_BC4_UNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RED_RGTC1, NVIMAGE_COMPRESSED_RED_RGTC1, NVIMAGE_UNSIGNED_BYTE, 8);
            break;

        case DDS10_FORMAT_BC4_SNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1, NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1, NVIMAGE_UNSIGNED_BYTE, 8);
            break;

        case DDS10_FORMAT_BC5_UNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_RG_RGTC2, NVIMAGE_COMPRESSED_RG_RGTC2, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_BC5_SNORM:
            SET_COMPRESSED_TYPE_INFO( NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2, NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2, NVIMAGE_UNSIGNED_BYTE, 16);
            break;

        case DDS10_FORMAT_B5G6R5_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGB5, NVIMAGE_BGR, NVIMAGE_UNSIGNED_SHORT_5_6_5, 2);
            break;

        case DDS10_FORMAT_B5G5R5A1_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGB5_A1, NVIMAGE_BGRA, NVIMAGE_UNSIGNED_SHORT_5_5_5_1, 2);
            break;

        case DDS10_FORMAT_B8G8R8A8_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGBA8, NVIMAGE_BGRA, NVIMAGE_UNSIGNED_BYTE, 2);
            break;

        case DDS10_FORMAT_B8G8R8X8_UNORM:
            SET_TYPE_INFO( NVIMAGE_RGB8, NVIMAGE_BGRA, NVIMAGE_UNSIGNED_BYTE, 4);
            break;

        case DDS10_FORMAT_B8G8R8A8_UNORM_SRGB:
            SET_TYPE_INFO( NVIMAGE_SRGB8_ALPHA8, NVIMAGE_BGRA, NVIMAGE_UNSIGNED_BYTE, 4);
            break;

        case DDS10_FORMAT_B8G8R8X8_UNORM_SRGB:
            SET_TYPE_INFO( NVIMAGE_SRGB8, NVIMAGE_BGRA, NVIMAGE_UNSIGNED_BYTE, 4);
            break;
		case DDS10_FORMAT_ASTC_4X4_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_4x4,NVIMAGE_COMPRESSED_RGBA_ASTC_4x4,NVIMAGE_UNSIGNED_BYTE,16);
			break;
		case DDS10_FORMAT_ASTC_4X4_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,NVIMAGE_UNSIGNED_BYTE,16);
			break;
		case DDS10_FORMAT_ASTC_5X4_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_5x4,NVIMAGE_COMPRESSED_RGBA_ASTC_5x4,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=5;
			break;
		case DDS10_FORMAT_ASTC_5X4_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=5;
			break;
		case DDS10_FORMAT_ASTC_5X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_5x5,NVIMAGE_COMPRESSED_RGBA_ASTC_5x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=5;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_5X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=5;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_6X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_6x5,NVIMAGE_COMPRESSED_RGBA_ASTC_6x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=6;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_6X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=6;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_6X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_6x6,NVIMAGE_COMPRESSED_RGBA_ASTC_6x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=6;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_6X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=6;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_8X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_8x5,NVIMAGE_COMPRESSED_RGBA_ASTC_8x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_8X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_8X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_8x6,NVIMAGE_COMPRESSED_RGBA_ASTC_8x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_8X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_8X8_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_8x8,NVIMAGE_COMPRESSED_RGBA_ASTC_8x8,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=8;
			break;
		case DDS10_FORMAT_ASTC_8X8_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=8;
			i._blockSize_y=8;
			break;
		case DDS10_FORMAT_ASTC_10X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_10x5,NVIMAGE_COMPRESSED_RGBA_ASTC_10x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_10X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=5;
			break;
		case DDS10_FORMAT_ASTC_10X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_10x6,NVIMAGE_COMPRESSED_RGBA_ASTC_10x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_10X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=6;
			break;
		case DDS10_FORMAT_ASTC_10X8_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_10x8,NVIMAGE_COMPRESSED_RGBA_ASTC_10x8,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=8;
			break;
		case DDS10_FORMAT_ASTC_10X8_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=8;
			break;
		case DDS10_FORMAT_ASTC_10X10_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_10x10,NVIMAGE_COMPRESSED_RGBA_ASTC_10x10,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=10;
			break;
		case DDS10_FORMAT_ASTC_10X10_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=10;
			i._blockSize_y=10;
			break;
		case DDS10_FORMAT_ASTC_12X10_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_12x10,NVIMAGE_COMPRESSED_RGBA_ASTC_12x10,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=12;
			i._blockSize_y=10;
			break;
		case DDS10_FORMAT_ASTC_12X10_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=12;
			i._blockSize_y=10;
			break;
		case DDS10_FORMAT_ASTC_12X12_UNORM:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_RGBA_ASTC_12x12,NVIMAGE_COMPRESSED_RGBA_ASTC_12x12,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=12;
			i._blockSize_y=12;
			break;
		case DDS10_FORMAT_ASTC_12X12_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,NVIMAGE_UNSIGNED_BYTE,16);
			i._blockSize_x=12;
			i._blockSize_y=12;
			break;

        case DDS10_FORMAT_R32G32B32A32_TYPELESS:
        case DDS10_FORMAT_R32G32B32_TYPELESS:
        case DDS10_FORMAT_R16G16B16A16_TYPELESS:
        case DDS10_FORMAT_R32G32_TYPELESS:
        case DDS10_FORMAT_R10G10B10A2_TYPELESS:
        case DDS10_FORMAT_R8G8B8A8_TYPELESS:
        case DDS10_FORMAT_R16G16_TYPELESS:
        case DDS10_FORMAT_R32_TYPELESS:
        case DDS10_FORMAT_R8G8_TYPELESS:
        case DDS10_FORMAT_R16_TYPELESS:
        case DDS10_FORMAT_R8_TYPELESS:
        case DDS10_FORMAT_BC1_TYPELESS:
        case DDS10_FORMAT_BC3_TYPELESS:
        case DDS10_FORMAT_BC4_TYPELESS:
        case DDS10_FORMAT_BC2_TYPELESS:
        case DDS10_FORMAT_BC5_TYPELESS:
        case DDS10_FORMAT_B8G8R8A8_TYPELESS:
        case DDS10_FORMAT_B8G8R8X8_TYPELESS:
		case DDS10_FORMAT_ASTC_5X4_TYPELESS:
		case DDS10_FORMAT_ASTC_5X5_TYPELESS:
		case DDS10_FORMAT_ASTC_6X5_TYPELESS:
		case DDS10_FORMAT_ASTC_6X6_TYPELESS:
		case DDS10_FORMAT_ASTC_8X5_TYPELESS:
		case DDS10_FORMAT_ASTC_8X6_TYPELESS:
		case DDS10_FORMAT_ASTC_8X8_TYPELESS:
		case DDS10_FORMAT_ASTC_10X5_TYPELESS:
		case DDS10_FORMAT_ASTC_10X6_TYPELESS:
		case DDS10_FORMAT_ASTC_10X8_TYPELESS:
		case DDS10_FORMAT_ASTC_10X10_TYPELESS:
		case DDS10_FORMAT_ASTC_12X10_TYPELESS:
		case DDS10_FORMAT_ASTC_12X12_TYPELESS:
            //unclear what to do with typeless formats, leave them as unsupported for now
            // in the future it might make sense to use a default type, if these are common
            return false;

        case DDS10_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DDS10_FORMAT_R1_UNORM:
        case DDS10_FORMAT_BC6H_TYPELESS:
        case DDS10_FORMAT_BC6H_UF16:
        case DDS10_FORMAT_BC6H_SF16:
        case DDS10_FORMAT_BC7_TYPELESS:
        case DDS10_FORMAT_BC7_UNORM:
        case DDS10_FORMAT_BC7_UNORM_SRGB:
            //these formats are unsupported presently
            return false;

        case DDS10_FORMAT_FORCE_UINT:
        case DDS10_FORMAT_UNKNOWN:
        default:
            //these are errors
            return false;
    };

    i._layers = header.arraySize;
    i._cubeMap = (header.miscFlag & 0x4) != 0;


    return true;
}

//
//
////////////////////////////////////////////////////////////
bool NvImage::readDDS(const uint8_t* data, size_t length, NvImage& i) {

    // open file
    //FILE *fp = fopen(file, "rb");
    NvFilePtr* fp = NvFilePtr::Create( (uint8_t*)data, length, NvFileMode::READ);

    if (fp == NULL)
        return false;

    // read in file marker, make sure its a DDS file
    char filecode[4];
    
    //fread(filecode, 1, 4, fp);
    fp->Read( 4, filecode);

    if (strncmp(filecode, "DDS ", 4) != 0)
    {
        //fclose(fp);
        delete fp;
        return false;
    }

    // read in DDS header
    DDS_HEADER ddsh;
    DDS_HEADER_10 ddsh10;
    
    //fread(&ddsh, sizeof(DDS_HEADER), 1, fp);
    fp->Read( sizeof(DDS_HEADER), &ddsh);

    // check if image is a volume texture
    if ((ddsh.dwCaps2 & DDSF_VOLUME) && (ddsh.dwDepth > 0))
        i._depth = ddsh.dwDepth;
    else
        i._depth = 0;

    if ((ddsh.ddspf.dwFlags & DDSF_FOURCC) && (ddsh.ddspf.dwFourCC == FOURCC_DX10)) {
        //This DDS file uses the DX10 header extension
        //fread(&ddsh10, sizeof(DDS_HEADER_10), 1, fp);
        fp->Read( sizeof(DDS_HEADER_10), &ddsh10);
    }

    // There are flags that are supposed to mark these fields as valid, but some dds files don't set them properly
    i._width = ddsh.dwWidth;
    i._height = ddsh.dwHeight;
    
    if (ddsh.dwFlags & DDSF_MIPMAPCOUNT) {
        i._levelCount = ddsh.dwMipMapCount;
    }
    else
        i._levelCount = 1;

    //check cube-map faces, the DX10 parser will override this
    if ( ddsh.dwCaps2 & DDSF_CUBEMAP && !(ddsh.ddspf.dwFlags & DDSF_FOURCC && ddsh.ddspf.dwFourCC == FOURCC_DX10)) {
        //this is a cubemap, count the faces
        i._layers = 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) ? 1 : 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) ? 1 : 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) ? 1 : 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) ? 1 : 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) ? 1 : 0;
        i._layers += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) ? 1 : 0;

        //check for a complete cubemap
        if ( (i._layers != 6) || (i._width != i._height) ) {
            //fclose(fp);
            delete fp;
            return false;
        }

        i._cubeMap = true;
    }
    else {
        //not a cubemap
        i._layers = 1;
        i._cubeMap = false;
    }

    bool btcCompressed = false;
    int32_t bytesPerElement = 0;

    // figure out what the image format is
    if (ddsh.ddspf.dwFlags & DDSF_FOURCC) 
    {
        switch(ddsh.ddspf.dwFourCC)
        {
            case FOURCC_DXT1:
                i._format = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1;
                i._internalFormat = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1;
                i._type = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1;
                bytesPerElement = 8;
                btcCompressed = true;
                break;

            case FOURCC_DXT2:
            case FOURCC_DXT3:
                i._format = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3;
                i._internalFormat = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3;
                i._type = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3;
                bytesPerElement = 16;
                btcCompressed = true;
                break;

            case FOURCC_DXT4:
            case FOURCC_DXT5:
                i._format = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5;
                i._internalFormat = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5;
                i._type = NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5;
                bytesPerElement = 16;
                btcCompressed = true;
                break;

            case FOURCC_ATI1:
                i._format = NVIMAGE_COMPRESSED_RED_RGTC1;
                i._internalFormat = NVIMAGE_COMPRESSED_RED_RGTC1;
                i._type = NVIMAGE_COMPRESSED_RED_RGTC1;
                bytesPerElement = 8;
                btcCompressed = true;
                break;

            case FOURCC_BC4U:
                i._format = NVIMAGE_COMPRESSED_RED_RGTC1;
                i._internalFormat = NVIMAGE_COMPRESSED_RED_RGTC1;
                i._type = NVIMAGE_COMPRESSED_RED_RGTC1;
                bytesPerElement = 8;
                btcCompressed = true;
                break;

            case FOURCC_BC4S:
                i._format = NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1;
                i._internalFormat = NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1;
                i._type = NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1;
                bytesPerElement = 8;
                btcCompressed = true;
                break;

            case FOURCC_ATI2:
                i._format = NVIMAGE_COMPRESSED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                i._internalFormat = NVIMAGE_COMPRESSED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                i._type = NVIMAGE_COMPRESSED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                bytesPerElement = 16;
                btcCompressed = true;
                break;

            case FOURCC_BC5S:
                i._format = NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                i._internalFormat = NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                i._type = NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2; //NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2;
                bytesPerElement = 16;
                btcCompressed = true;
                break;

            case FOURCC_R8G8B8:
                i._format = NVIMAGE_BGR;
                i._internalFormat = NVIMAGE_RGB8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 3;
                break;

            case FOURCC_A8R8G8B8:
                i._format = NVIMAGE_BGRA;
                i._internalFormat = NVIMAGE_RGBA8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 4;
                break;

            case FOURCC_X8R8G8B8:
                i._format = NVIMAGE_BGRA;
                i._internalFormat = NVIMAGE_RGB8;
                i._type = NVIMAGE_UNSIGNED_INT_8_8_8_8;
                bytesPerElement = 4;
                break;

            case FOURCC_R5G6B5:
                i._format = NVIMAGE_BGR;
                i._internalFormat = NVIMAGE_RGB5;
                i._type = NVIMAGE_UNSIGNED_SHORT_5_6_5;
                bytesPerElement = 2;
                break;

            case FOURCC_A8:
                i._format = NVIMAGE_ALPHA;
                i._internalFormat = NVIMAGE_ALPHA8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 1;
                break;

            case FOURCC_A2B10G10R10:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGB10_A2;
                i._type = NVIMAGE_UNSIGNED_INT_10_10_10_2;
                bytesPerElement = 4;
                break;

            case FOURCC_A8B8G8R8:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGBA8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 4;
                break;

            case FOURCC_X8B8G8R8:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGB8;
                i._type = NVIMAGE_UNSIGNED_INT_8_8_8_8;
                bytesPerElement = 4;
                break;

            case FOURCC_A2R10G10B10:
                i._format = NVIMAGE_BGRA;
                i._internalFormat = NVIMAGE_RGB10_A2;
                i._type = NVIMAGE_UNSIGNED_INT_10_10_10_2;
                bytesPerElement = 4;
                break;

            case FOURCC_G16R16:
                i._format = NVIMAGE_RG;
                i._internalFormat = NVIMAGE_RG16;
                i._type = NVIMAGE_UNSIGNED_SHORT;
                bytesPerElement = 4;
                break;

            case FOURCC_A16B16G16R16:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGBA16;
                i._type = NVIMAGE_UNSIGNED_SHORT;
                bytesPerElement = 8;
                break;

            case FOURCC_L8:
                i._format = NVIMAGE_LUMINANCE;
                i._internalFormat = NVIMAGE_LUMINANCE8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 1;
                break;

            case FOURCC_A8L8:
                i._format = NVIMAGE_LUMINANCE_ALPHA;
                i._internalFormat = NVIMAGE_LUMINANCE8_ALPHA8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 2;
                break;

            case FOURCC_L16:
                i._format = NVIMAGE_LUMINANCE;
                i._internalFormat = NVIMAGE_LUMINANCE16;
                i._type = NVIMAGE_UNSIGNED_SHORT;
                bytesPerElement = 2;
                break;

            case FOURCC_Q16W16V16U16:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGBA16_SNORM;
                i._type = NVIMAGE_SHORT;
                bytesPerElement = 8;
                break;

            case FOURCC_R16F:
                i._format = NVIMAGE_RED; 
                i._internalFormat = NVIMAGE_R16F; 
                i._type = NVIMAGE_HALF_FLOAT;
                bytesPerElement = 2;
                break;

            case FOURCC_G16R16F:
                i._format = NVIMAGE_RG; 
                i._internalFormat = NVIMAGE_RG16F; 
                i._type = NVIMAGE_HALF_FLOAT;
                bytesPerElement = 4;
                break;

            case FOURCC_A16B16G16R16F:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGBA16F;
                i._type = NVIMAGE_HALF_FLOAT;
                bytesPerElement = 8;
                break;

            case FOURCC_R32F:
                i._format = NVIMAGE_RED; 
                i._internalFormat = NVIMAGE_R32F; 
                i._type = NVIMAGE_FLOAT;
                bytesPerElement = 4;
                break;

            case FOURCC_G32R32F:
                i._format = NVIMAGE_RG; 
                i._internalFormat = NVIMAGE_RG32F; 
                i._type = NVIMAGE_FLOAT;
                bytesPerElement = 8;
                break;

            case FOURCC_A32B32G32R32F:
                i._format = NVIMAGE_RGBA;
                i._internalFormat = NVIMAGE_RGBA32F;
                i._type = NVIMAGE_FLOAT;
                bytesPerElement = 16;
                break;

            case FOURCC_DX10:
                if (!TranslateDX10Format( &ddsh10, i, bytesPerElement, btcCompressed)) {
                    //fclose(fp);
                    delete fp;
                    return false; //translation from DX10 failed
                }
                break;

            case FOURCC_UNKNOWN:
            case FOURCC_X1R5G5B5:
            case FOURCC_A1R5G5B5:
            case FOURCC_A4R4G4B4:
            case FOURCC_R3G3B2:
            case FOURCC_A8R3G3B2:
            case FOURCC_X4R4G4B4:
            case FOURCC_A4L4:
            case FOURCC_D16_LOCKABLE:
            case FOURCC_D32:
            case FOURCC_D24X8:
            case FOURCC_D16:
            case FOURCC_D32F_LOCKABLE:
                //these are unsupported for now
            default:
                //fclose(fp);
                delete fp;
                return false;
        }
    }
    else if (ddsh.ddspf.dwFlags == DDSF_RGBA && ddsh.ddspf.dwRGBBitCount == 32)
    {
        if ( ddsh.ddspf.dwRBitMask == 0xff && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff0000 && ddsh.ddspf.dwABitMask == 0xff000000 ) {
            //RGBA8 order
            i._format = NVIMAGE_RGBA;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_BYTE;
        }
        else if ( ddsh.ddspf.dwRBitMask == 0xff0000 && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff && ddsh.ddspf.dwABitMask == 0xff000000 ) {
            //BGRA8 order
            i._format = NVIMAGE_BGRA;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_BYTE;
        } 
        else if ( ddsh.ddspf.dwRBitMask == 0x3ff00000 && ddsh.ddspf.dwGBitMask == 0xffc00 && ddsh.ddspf.dwBBitMask == 0x3ff && ddsh.ddspf.dwABitMask == 0xc0000000 ) {
            //BGR10_A2 order
            i._format = NVIMAGE_RGBA;
            i._internalFormat = NVIMAGE_RGB10_A2;
            i._type = NVIMAGE_UNSIGNED_INT_2_10_10_10_REV; //NVIMAGE_UNSIGNED_INT_10_10_10_2;
        }
        else if ( ddsh.ddspf.dwRBitMask == 0x3ff && ddsh.ddspf.dwGBitMask == 0xffc00 && ddsh.ddspf.dwBBitMask == 0x3ff00000 && ddsh.ddspf.dwABitMask == 0xc0000000 ) {
            //RGB10_A2 order
            i._format = NVIMAGE_RGBA;
            i._internalFormat = NVIMAGE_RGB10_A2;
            i._type = NVIMAGE_UNSIGNED_INT_10_10_10_2;
        }
        else {
            //we'll just guess BGRA8, because that is the common legacy format for improperly labeled files
            i._format = NVIMAGE_BGRA;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_BYTE;
        }
        bytesPerElement = 4;
    }
    else if (ddsh.ddspf.dwFlags == DDSF_RGB  && ddsh.ddspf.dwRGBBitCount == 32)
    {
        if ( ddsh.ddspf.dwRBitMask == 0xffff && ddsh.ddspf.dwGBitMask == 0xffff0000 && ddsh.ddspf.dwBBitMask == 0x00 && ddsh.ddspf.dwABitMask == 0x00 ) {
            i._format = NVIMAGE_RG;
            i._internalFormat = NVIMAGE_RG16;
            i._type = NVIMAGE_UNSIGNED_SHORT;
        }
        else if ( ddsh.ddspf.dwRBitMask == 0xff && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff0000 && ddsh.ddspf.dwABitMask == 0x00 ) {
            i._format = NVIMAGE_RGB;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_INT_8_8_8_8;
        }
        else if ( ddsh.ddspf.dwRBitMask == 0xff0000 && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff && ddsh.ddspf.dwABitMask == 0x00 ) {
            i._format = NVIMAGE_BGR;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_INT_8_8_8_8;
        }
        else {
            // probably a poorly labeled file with BGRX semantics
            i._format = NVIMAGE_BGR;
            i._internalFormat = NVIMAGE_RGBA8;
            i._type = NVIMAGE_UNSIGNED_INT_8_8_8_8;
        }
        bytesPerElement = 4;
    }
    else if (ddsh.ddspf.dwFlags == DDSF_RGB  && ddsh.ddspf.dwRGBBitCount == 24)
    {
        i._format = NVIMAGE_BGR;
        i._internalFormat = NVIMAGE_RGB8;
        i._type = NVIMAGE_UNSIGNED_BYTE;
        bytesPerElement = 3;
    }
    // these cases revived from NVHHDDS...
    else if ((ddsh.ddspf.dwRGBBitCount == 16) &&
        (ddsh.ddspf.dwRBitMask == 0x0000F800) &&
        (ddsh.ddspf.dwGBitMask == 0x000007E0) &&
        (ddsh.ddspf.dwBBitMask == 0x0000001F) &&
        (ddsh.ddspf.dwABitMask == 0x00000000))
    {
        // We support D3D's R5G6B5, which is actually RGB in linear
        // memory.  It is equivalent to GL's NVIMAGE_UNSIGNED_SHORT_5_6_5
        i._format = NVIMAGE_BGR;
        i._internalFormat = NVIMAGE_RGB5;
        i._type = NVIMAGE_UNSIGNED_SHORT_5_6_5;
        bytesPerElement = 2;
    } 
    else if ((ddsh.ddspf.dwRGBBitCount == 8) &&
        (ddsh.ddspf.dwRBitMask == 0x00000000) &&
        (ddsh.ddspf.dwGBitMask == 0x00000000) &&
        (ddsh.ddspf.dwBBitMask == 0x00000000) &&
        (ddsh.ddspf.dwABitMask == 0x000000FF))
    {
        // We support D3D's A8
                i._format = NVIMAGE_ALPHA;
                i._internalFormat = NVIMAGE_ALPHA8;
                i._type = NVIMAGE_UNSIGNED_BYTE;
                bytesPerElement = 1;
    }
    else if ((ddsh.ddspf.dwRGBBitCount == 8) &&
            (ddsh.ddspf.dwRBitMask == 0x000000FF) &&
            (ddsh.ddspf.dwGBitMask == 0x00000000) &&
            (ddsh.ddspf.dwBBitMask == 0x00000000) &&
            (ddsh.ddspf.dwABitMask == 0x00000000))
    {
        // We support D3D's L8 (flagged as 8 bits of red only)
        i._format = NVIMAGE_LUMINANCE; 
        i._internalFormat = NVIMAGE_LUMINANCE8; 
        i._type = NVIMAGE_UNSIGNED_BYTE;
        bytesPerElement = 1;
    } 
    else if ((ddsh.ddspf.dwRGBBitCount == 16) &&
            (((ddsh.ddspf.dwRBitMask == 0x000000FF) &&
            (ddsh.ddspf.dwGBitMask == 0x00000000) &&
            (ddsh.ddspf.dwBBitMask == 0x00000000) &&
            (ddsh.ddspf.dwABitMask == 0x0000FF00)) ||
            ((ddsh.ddspf.dwRBitMask == 0x000000FF) && // GIMP header for L8A8
            (ddsh.ddspf.dwGBitMask == 0x000000FF) &&  // Ugh
            (ddsh.ddspf.dwBBitMask == 0x000000FF) &&
            (ddsh.ddspf.dwABitMask == 0x0000FF00)))
        )
    {
        // We support D3D's A8L8 (flagged as 8 bits of red and 8 bits of alpha)
        i._format = NVIMAGE_LUMINANCE_ALPHA;
        i._internalFormat = NVIMAGE_LUMINANCE8_ALPHA8;
        i._type = NVIMAGE_UNSIGNED_BYTE;
        bytesPerElement = 2;
    } 
    // else fall back to L8 generic handling if capable.
    else if (ddsh.ddspf.dwRGBBitCount == 8)
    {
        i._format = NVIMAGE_LUMINANCE; 
        i._internalFormat = NVIMAGE_LUMINANCE8; 
        i._type = NVIMAGE_UNSIGNED_BYTE;
        bytesPerElement = 1;
    }
    // else, we can't decode this file... :-(
    else
    {
        // TODO better error?
        //LOGI("! Error decoding DDS file.");
        //fclose(fp);
        delete fp;
        return false;
    }

	/// Flip not allowed for ASTC
	if (NvImage::vertFlip && 
		( (i._internalFormat>=0x93B0 && i._internalFormat<=0x93BD)
		|| (i._internalFormat>=0x93D0 && i._internalFormat<=0x93DD)))
	{
		delete fp;
		return false;
	}

    i._elementSize = bytesPerElement;

	delete[] i._dataBlock;
	delete[] i._data;

    bool mustExpandDXT = m_expandDXT &&
        ((i._format == NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1) ||
        (i._format == NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3) ||
        (i._format == NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5));

	i._dataArrayCount = i._layers * i._levelCount;
	i._data = new uint8_t*[i._dataArrayCount];

	int32_t totalSize = 0;
	{
		for (int32_t face = 0; face < i._layers; face++) {
			int32_t w = i._width, h = i._height, d = (i._depth) ? i._depth : 1;
			for (int32_t level = 0; level < i._levelCount; level++) {
				int32_t bw = (btcCompressed) ? (w - 1) / i._blockSize_x + 1 : w;
				int32_t bh = (btcCompressed) ? (h - 1) / i._blockSize_y + 1 : h;
				int32_t readSize = bw*bh*d*bytesPerElement;
				int32_t allocSize = mustExpandDXT ? (w * h * 4) : readSize;

				totalSize += allocSize;

				//reduce mip sizes
				if (!btcCompressed) {
					w = (w > 1) ? w >> 1 : 1;
					h = (h > 1) ? h >> 1 : 1;
					d = (d > 1) ? d >> 1 : 1;
				}
			}
		}
	}

	i._dataBlock = new uint8_t[totalSize];
	i._dataBlockSize = totalSize;
	uint8_t* ptr = i._dataBlock;
	int currentLevel = 0;

    // Use the original format when performing component swaps, but store any new
    // format in the image once the loop over all faces/levels is complete
    uint32_t originalFormat = i._format;
    uint32_t finalFormat = i._format;

    for (int32_t face = 0; face < i._layers; face++) {
        int32_t w = i._width, h = i._height, d = (i._depth) ? i._depth : 1;
        for (int32_t level = 0; level < i._levelCount; level++) {
			int32_t bw = (btcCompressed) ? (w-1)/i._blockSize_x+1 : w;
			int32_t bh = (btcCompressed) ? (h-1)/i._blockSize_y+1 : h;
            int32_t readSize = bw*bh*d*bytesPerElement;
			int32_t allocSize = mustExpandDXT ? (w * h * 4) : readSize;

			uint8_t *pixels = ptr;
			i._data[currentLevel++] = ptr;
			ptr += allocSize;

            //fread( data, size, 1, fp);
			fp->Read(readSize, pixels);
			
            if ( NvImage::vertFlip && !i._cubeMap)
                i.flipSurface( pixels, w, h, d);

            if (!m_supportsBGR)
                finalFormat = i.componentSwapSurface(pixels, w, h, d);

            // do we need to expand DXT?
            if (mustExpandDXT)
				i.expandDXT(pixels, w, h, d, readSize);

            //reduce mip sizes
            w = ( w > 1) ? w >> 1 : 1;
            h = ( h > 1) ? h >> 1 : 1;
            d = ( d > 1) ? d >> 1 : 1;
        }
    }

    if (mustExpandDXT) {
        i._format = NVIMAGE_RGBA;
        i._type = NVIMAGE_UNSIGNED_BYTE;
		i._internalFormat = NVIMAGE_RGBA8;
    }
    else
    {
        i._format = finalFormat;
    }

    //fclose(fp);
    delete fp;
    return true;
}

//
// flip a DXT1 color block
////////////////////////////////////////////////////////////
void NvImage::flip_blocks_dxtc1(uint8_t *ptr, uint32_t numBlocks)
{
    DXTColBlock *curblock = (DXTColBlock*)ptr;
    uint8_t temp;

    for (uint32_t i = 0; i < numBlocks; i++) {
        temp = curblock->row[0];
        curblock->row[0] = curblock->row[3];
        curblock->row[3] = temp;
        temp = curblock->row[1];
        curblock->row[1] = curblock->row[2];
        curblock->row[2] = temp;

        curblock++;
    }
}

//
// flip a DXT3 color block
////////////////////////////////////////////////////////////
void NvImage::flip_blocks_dxtc3(uint8_t *ptr, uint32_t numBlocks)
{
    DXTColBlock *curblock = (DXTColBlock*)ptr;
    DXT3AlphaBlock *alphablock;
    uint16_t tempS;
    uint8_t tempB;

    for (uint32_t i = 0; i < numBlocks; i++)
    {
        alphablock = (DXT3AlphaBlock*)curblock;

        tempS = alphablock->row[0];
        alphablock->row[0] = alphablock->row[3];
        alphablock->row[3] = tempS;
        tempS = alphablock->row[1];
        alphablock->row[1] = alphablock->row[2];
        alphablock->row[2] = tempS;

        curblock++;

        tempB = curblock->row[0];
        curblock->row[0] = curblock->row[3];
        curblock->row[3] = tempB;
        tempB = curblock->row[1];
        curblock->row[1] = curblock->row[2];
        curblock->row[2] = tempB;

        curblock++;
    }
}

//
// flip a DXT5 alpha block
////////////////////////////////////////////////////////////
void flip_dxt5_alpha(DXT5AlphaBlock *block)
{
    uint8_t gBits[4][4];
    
    const uint32_t mask = 0x00000007;          // bits = 00 00 01 11
    uint32_t bits = 0;
    memcpy(&bits, &block->row[0], sizeof(uint8_t) * 3);

    gBits[0][0] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[0][1] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[0][2] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[0][3] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[1][0] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[1][1] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[1][2] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[1][3] = (uint8_t)(bits & mask);

    bits = 0;
    memcpy(&bits, &block->row[3], sizeof(uint8_t) * 3);

    gBits[2][0] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[2][1] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[2][2] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[2][3] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[3][0] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[3][1] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[3][2] = (uint8_t)(bits & mask);
    bits >>= 3;
    gBits[3][3] = (uint8_t)(bits & mask);

    // clear existing alpha bits
    memset(block->row, 0, sizeof(uint8_t) * 6);

    uint32_t *pBits = ((uint32_t*) &(block->row[0]));

    *pBits = *pBits | (gBits[3][0] << 0);
    *pBits = *pBits | (gBits[3][1] << 3);
    *pBits = *pBits | (gBits[3][2] << 6);
    *pBits = *pBits | (gBits[3][3] << 9);

    *pBits = *pBits | (gBits[2][0] << 12);
    *pBits = *pBits | (gBits[2][1] << 15);
    *pBits = *pBits | (gBits[2][2] << 18);
    *pBits = *pBits | (gBits[2][3] << 21);

    pBits = ((uint32_t*) &(block->row[3]));

    *pBits = *pBits | (gBits[1][0] << 0);
    *pBits = *pBits | (gBits[1][1] << 3);
    *pBits = *pBits | (gBits[1][2] << 6);
    *pBits = *pBits | (gBits[1][3] << 9);

    *pBits = *pBits | (gBits[0][0] << 12);
    *pBits = *pBits | (gBits[0][1] << 15);
    *pBits = *pBits | (gBits[0][2] << 18);
    *pBits = *pBits | (gBits[0][3] << 21);
}

//
// flip a DXT5 color block
////////////////////////////////////////////////////////////
void NvImage::flip_blocks_dxtc5(uint8_t *ptr, uint32_t numBlocks)
{
    DXTColBlock *curblock = (DXTColBlock*)ptr;
    DXT5AlphaBlock *alphablock;
    uint8_t temp;
    
    for (uint32_t i = 0; i < numBlocks; i++)
    {
        alphablock = (DXT5AlphaBlock*)curblock;
        
        flip_dxt5_alpha(alphablock);

        curblock++;

        temp = curblock->row[0];
        curblock->row[0] = curblock->row[3];
        curblock->row[3] = temp;
        temp = curblock->row[1];
        curblock->row[1] = curblock->row[2];
        curblock->row[2] = temp;

        curblock++;
    }
}

//
// flip a BC4 block
////////////////////////////////////////////////////////////
void NvImage::flip_blocks_bc4(uint8_t *ptr, uint32_t numBlocks)
{
    DXT5AlphaBlock *alphablock = (DXT5AlphaBlock*)ptr;
    
    for (uint32_t i = 0; i < numBlocks; i++)
    {
        flip_dxt5_alpha(alphablock);

        alphablock++;
    }
}

//
// flip a BC5 block
////////////////////////////////////////////////////////////
void NvImage::flip_blocks_bc5(uint8_t *ptr, uint32_t numBlocks)
{
    DXT5AlphaBlock *alphablock = (DXT5AlphaBlock*)ptr;
    
    for (uint32_t i = 0; i < numBlocks; i++)
    {
        flip_dxt5_alpha(alphablock);

        alphablock++;
        flip_dxt5_alpha(alphablock);

        alphablock++;
    }
}

