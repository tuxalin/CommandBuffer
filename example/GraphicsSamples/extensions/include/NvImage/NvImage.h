//----------------------------------------------------------------------------------
// File:        NvImage/NvImage.h
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

#ifndef NV_IMAGE_H
#define NV_IMAGE_H

#include <NvSimpleTypes.h>
#include <vector>

// These enums DO and MUST match the Khronos/GL enum values, so that GL runs well
// Other APIs will map a subset for now
#define    NVIMAGE_BYTE    0x1400
#define    NVIMAGE_UNSIGNED_BYTE    0x1401
#define    NVIMAGE_SHORT    0x1402
#define    NVIMAGE_UNSIGNED_SHORT    0x1403
#define    NVIMAGE_INT    0x1404
#define    NVIMAGE_UNSIGNED_INT    0x1405
#define    NVIMAGE_FLOAT    0x1406
#define    NVIMAGE_HALF_FLOAT    0x140B

#define    NVIMAGE_DEPTH    0x1801
#define    NVIMAGE_STENCIL    0x1802

#define    NVIMAGE_RED    0x1903
#define    NVIMAGE_ALPHA    0x1906
#define    NVIMAGE_RGB    0x1907
#define    NVIMAGE_RGBA    0x1908
#define    NVIMAGE_LUMINANCE    0x1909
#define    NVIMAGE_LUMINANCE_ALPHA    0x190A

#define    NVIMAGE_UNSIGNED_SHORT_5_5_5_1    0x8034

#define    NVIMAGE_UNSIGNED_INT_8_8_8_8    0x8035
#define    NVIMAGE_UNSIGNED_INT_10_10_10_2    0x8036

#define    NVIMAGE_ALPHA8    0x803C
#define    NVIMAGE_ALPHA16    0x803E

#define    NVIMAGE_LUMINANCE8    0x8040
#define    NVIMAGE_LUMINANCE16    0x8042
#define    NVIMAGE_LUMINANCE8_ALPHA8    0x8045
#define    NVIMAGE_LUMINANCE16_ALPHA16    0x8048

#define    NVIMAGE_RGB5    0x8050
#define    NVIMAGE_RGB8    0x8051
#define    NVIMAGE_RGB16    0x8054
#define    NVIMAGE_RGB5_A1    0x8057
#define    NVIMAGE_RGBA8    0x8058
#define    NVIMAGE_RGB10_A2    0x8059
#define    NVIMAGE_RGBA12    0x805A
#define    NVIMAGE_RGBA16    0x805B

#define    NVIMAGE_BGR    0x80E0
#define    NVIMAGE_BGRA    0x80E1

#define    NVIMAGE_DEPTH_COMPONENT16    0x81A5
#define    NVIMAGE_DEPTH_COMPONENT32    0x81A7

#define    NVIMAGE_COMPRESSED_RED    0x8225
#define    NVIMAGE_COMPRESSED_RG    0x8226

#define    NVIMAGE_RG    0x8227
#define    NVIMAGE_RG_INTEGER    0x8228
#define    NVIMAGE_R8    0x8229
#define    NVIMAGE_R16    0x822A
#define    NVIMAGE_RG8    0x822B
#define    NVIMAGE_RG16    0x822C
#define    NVIMAGE_R16F    0x822D
#define    NVIMAGE_R32F    0x822E
#define    NVIMAGE_RG16F    0x822F
#define    NVIMAGE_RG32F    0x8230

#define    NVIMAGE_R8I    0x8231
#define    NVIMAGE_R8UI    0x8232
#define    NVIMAGE_R16I    0x8233
#define    NVIMAGE_R16UI    0x8234
#define    NVIMAGE_R32I    0x8235
#define    NVIMAGE_R32UI    0x8236

#define    NVIMAGE_RG8I    0x8237
#define    NVIMAGE_RG8UI    0x8238
#define    NVIMAGE_RG16I    0x8239
#define    NVIMAGE_RG16UI    0x823A
#define    NVIMAGE_RG32I    0x823B
#define    NVIMAGE_RG32UI    0x823C

#define    NVIMAGE_UNSIGNED_SHORT_5_6_5    0x8363

#define    NVIMAGE_UNSIGNED_INT_2_10_10_10_REV    0x8368

#define    NVIMAGE_COMPRESSED_RGB_S3TC_DXT1    0x83F0
#define    NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1    0x83F1
#define    NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3    0x83F2
#define    NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5    0x83F3

#define    NVIMAGE_COMPRESSED_ALPHA    0x84E9
#define    NVIMAGE_COMPRESSED_LUMINANCE    0x84EA
#define    NVIMAGE_COMPRESSED_LUMINANCE_ALPHA    0x84EB
#define    NVIMAGE_COMPRESSED_RGB    0x84ED
#define    NVIMAGE_COMPRESSED_RGBA    0x84EE

#define    NVIMAGE_TEXTURE_CUBE_MAP    0x8513
#define    NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define    NVIMAGE_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define    NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define    NVIMAGE_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define    NVIMAGE_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define    NVIMAGE_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A

#define    NVIMAGE_UNSIGNED_INT_S8_S8_8_8    0x86DA
#define    NVIMAGE_UNSIGNED_INT_8_8_S8_S8_REV    0x86DB

#define    NVIMAGE_RGBA32F    0x8814
#define    NVIMAGE_RGB32F    0x8815

#define    NVIMAGE_ALPHA32F    0x8816

#define    NVIMAGE_LUMINANCE32F    0x8818
#define    NVIMAGE_LUMINANCE_ALPHA32F    0x8819

#define    NVIMAGE_RGBA16F    0x881A
#define    NVIMAGE_RGB16F    0x881B

#define    NVIMAGE_ALPHA16F    0x881C

#define    NVIMAGE_LUMINANCE16F    0x881E
#define    NVIMAGE_LUMINANCE_ALPHA16F    0x881F

#define    NVIMAGE_R11F_G11F_B10F    0x8C3A

#define    NVIMAGE_UNSIGNED_INT_10F_11F_11F_REV    0x8C3B

#define    NVIMAGE_RGB9_E5    0x8C3D

#define    NVIMAGE_UNSIGNED_INT_5_9_9_9_REV    0x8C3E

#define    NVIMAGE_SRGB    0x8C40
#define    NVIMAGE_SRGB8    0x8C41
#define    NVIMAGE_SRGB_ALPHA    0x8C42
#define    NVIMAGE_SRGB8_ALPHA8    0x8C43
#define    NVIMAGE_SLUMINANCE_ALPHA    0x8C44
#define    NVIMAGE_SLUMINANCE8_ALPHA8    0x8C45
#define    NVIMAGE_SLUMINANCE    0x8C46
#define    NVIMAGE_SLUMINANCE8    0x8C47
#define    NVIMAGE_COMPRESSED_SRGB    0x8C48
#define    NVIMAGE_COMPRESSED_LUMINANCE_LATC1    0x8C70
#define    NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_LATC1    0x8C71
#define    NVIMAGE_COMPRESSED_LUMINANCE_ALPHA_LATC2    0x8C72
#define    NVIMAGE_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2    0x8C73
#define    NVIMAGE_DEPTH_COMPONENT32F    0x8CAC
#define    NVIMAGE_DEPTH32F_STENCIL8    0x8CAD
#define    NVIMAGE_RGBA32UI    0x8D70
#define    NVIMAGE_RGB32UI    0x8D71
#define    NVIMAGE_RGBA16UI    0x8D76
#define    NVIMAGE_RGB16UI    0x8D77
#define    NVIMAGE_RGBA8UI    0x8D7C
#define    NVIMAGE_RGB8UI    0x8D7D
#define    NVIMAGE_RGBA32I    0x8D82
#define    NVIMAGE_RGB32I    0x8D83
#define    NVIMAGE_RGBA16I    0x8D88
#define    NVIMAGE_RGB16I    0x8D89
#define    NVIMAGE_RGBA8I    0x8D8E
#define    NVIMAGE_RGB8I    0x8D8F
#define    NVIMAGE_RED_INTEGER    0x8D94
#define    NVIMAGE_RGB_INTEGER    0x8D98
#define    NVIMAGE_RGBA_INTEGER    0x8D99
#define    NVIMAGE_BGR_INTEGER    0x8D9A
#define    NVIMAGE_BGRA_INTEGER    0x8D9B
#define    NVIMAGE_COMPRESSED_RED_RGTC1    0x8DBB
#define    NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1    0x8DBC
#define    NVIMAGE_COMPRESSED_RG_RGTC2    0x8DBD
#define    NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2    0x8DBE
#define    NVIMAGE_RED_SNORM    0x8F90
#define    NVIMAGE_RG_SNORM    0x8F91
#define    NVIMAGE_RGB_SNORM    0x8F92
#define    NVIMAGE_RGBA_SNORM    0x8F93
#define    NVIMAGE_R8_SNORM    0x8F94
#define    NVIMAGE_RG8_SNORM    0x8F95
#define    NVIMAGE_RGB8_SNORM    0x8F96
#define    NVIMAGE_RGBA8_SNORM    0x8F97
#define    NVIMAGE_R16_SNORM    0x8F98
#define    NVIMAGE_RG16_SNORM    0x8F99
#define    NVIMAGE_RGB16_SNORM    0x8F9A
#define    NVIMAGE_RGBA16_SNORM    0x8F9B
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_4x4    0x93B0
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_5x4    0x93B1
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_5x5    0x93B2
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_6x5    0x93B3
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_6x6    0x93B4
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_8x5    0x93B5
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_8x6    0x93B6
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_8x8    0x93B7
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_10x5    0x93B8
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_10x6    0x93B9
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_10x8    0x93BA
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_10x10    0x93BB
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_12x10    0x93BC
#define    NVIMAGE_COMPRESSED_RGBA_ASTC_12x12    0x93BD
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4    0x93D0
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4    0x93D1
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5    0x93D2
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5    0x93D3
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6    0x93D4
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5    0x93D5
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6    0x93D6
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8    0x93D7
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5    0x93D8
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6    0x93D9
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8    0x93DA
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10    0x93DB
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10    0x93DC
#define    NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12    0x93DD

/// \file
/// Image-handling support (loading, editing, GL textures)

/// Image loading, representation and handling
/// Support loading of images from DDS files and data, including
/// cube maps, arrays mipmap levels, formats, etc.
/// The class does NOT encapsulate a rendering texture object, only the
/// client side pixel data that could be used to create such a texture
class NvImage {
public:

    /// Sets whether to flip the image vertically.
    /// By default, the image library places the origin of images at the
    /// lower-left corner, to make it map exactly to OpenGL screen coords.
    /// This might make it incompatible with the texture coordinate conventions 
	/// of an imported model that places its origin at the upper-left corner (D3D/DDS).
	/// In that case, doing a flip during loading could be desirable.
	/// Note: for ASTC textures, flip is not supported and "VerticalFlip(false)" is required.
    /// \param[in] flip true if flip is required.
    static void VerticalFlip( bool flip);

    /// Returns whether the loader will flip the image vertically.
    /// By default, the image library places the origin of images at the
    /// lower-left corner, to make it map exactly to OpenGL screen coords.
    /// This might make it incompatible with the texture coordinate conventions 
	/// of an imported model that places its origin at the upper-left corner (D3D/DDS).
	/// In that case, doing a flip during loading could be desirable.
    /// \return true if flip is enabled.
    static bool GetVerticalFlip() { return vertFlip; }

    /// Create a new NvImage (no texture) directly from DDS file
    /// Uses #NvAssetLoaderRead for opening the file.  See the documentation for
    /// that package to understand the correct paths
    /// \param[in] filename the image filename (and path) to load
    /// \return a pointer to the NvImage representing the file or null on failure
    static NvImage* CreateFromDDSFile(const char* filename);

    NvImage();
    virtual ~NvImage();

    /// The image width in pixels
    /// \return the width of the image in pixels
    int32_t getWidth() const { return _width; }

    /// The image height in pixels
    /// \return the height of the image in pixels
    int32_t getHeight() const { return _height; }

    /// The image depth in pixels.
    /// This is the third dimension of a 3D/volume image, NOT the color-depth
    /// \return the depth of the image (0 for images with no depth)
    int32_t getDepth() const { return _depth; }

    /// The number of miplevels.
    /// \return the number of mipmap levels available for the image
    int32_t getMipLevels() const { return _levelCount; }

    /// The number of cubemap faces.
    /// \return the number of cubemap faces available for the image (0 for non-cubemap images)
    int32_t getFaces() const { return _cubeMap ? _layers : 0; }

    /// The number of layers in a texture array
    /// \return the number of layers for use in texture arrays
    int32_t getLayers() const { return _layers; }

    /// The GL format of the image
    /// \return the format of the image data (NVIMAGE_RGB, NVIMAGE_BGR, etc)
    uint32_t getFormat() const { return _format; }

    /// The GL internal format of the image
    /// \return the suggested internal format for the data
    uint32_t getInternalFormat() const { return _internalFormat; }

    /// The GL type of the pixel data
    /// \return the type of the image data
    uint32_t getType() const { return _type; }

    /// The size (in bytes) of a selected mipmap level of the image
    /// \return the Size in bytes of a level of the image 
    /// \param[in] level the mipmap level whose size if to be returned
    int32_t getImageSize(int32_t level = 0) const;

    /// Whether or not the image is compressed
    /// \return boolean whether the data is a crompressed format
    bool isCompressed() const;

    /// Whether or not the image is a cubemap
    /// \return boolean whether the image represents a cubemap
    bool isCubeMap() const { return _cubeMap; }

    /// Whether or not the image's pixel format has an explicit alpha channel
    /// \return boolean whether the image has explicit alpha channel
    bool hasAlpha() const;

    /// Whether or not the image is an array texture
    /// \return boolean whether the image represents a texture array
    bool isArray() const { return _layers > 1; }

    /// Whether or not the image is a volume (3D) image
    /// \return boolean whether the image represents a volume
    bool isVolume() const { return _depth > 0; }

    ///@{
    /// Get a pointer to the pixel data for a given mipmap level.
    /// \param[in] level the mipmap level [0, getMipLevels)
    /// \return a pointer to the data
    const void* getLevel( int32_t level) const;
    void* getLevel( int32_t level);
    ///@}

    ///@{
    /// Get a pointer to the pixel data for a given mipmap level and cubemap face.
    /// \param[in] level the mipmap level [0, getMipLevels)
    /// \param[in] face the cubemap face (NVIMAGE_TEXTURE_CUBE_MAP_*_*)
    /// \return a pointer to the data
    const void* getLevel( int32_t level, uint32_t face) const;
    void* getLevel( int32_t level, uint32_t face);
    ///@}

    ///@{
    /// Get a pointer to the pixel data for a given mipmap level and array slice.
    /// \param[in] level the mipmap level [0, #getMipLevels)
    /// \param[in] slice the layer index [0, #getLayers)
    /// \return a pointer to the data
    const void* getLayerLevel( int32_t level, int32_t slice) const;
    void* getLayerLevel( int32_t level, int32_t slice);
    ///@}

	///@{
	/// Get a pointer to the overall pixel data block for the image.
	/// The block is contiguous and allows for easy loading into a single
	/// pre-allocated memory block for low-level 3D APIs
	/// \return a pointer to the data
	const uint8_t* getDataBlock() const { return _dataBlock; }
	uint8_t* getDataBlock() { return _dataBlock; }
	///@}

	/// Get the size of the overall pixel data block for the image.
	/// The block is contiguous and allows for easy loading into a single
	/// pre-allocated memory block for low-level 3D APIs
	/// \return the size of the block in bytes
	uint32_t getDataBlockSize() const { return _dataBlockSize; }

	///@{
	/// Get an offset in bytes from the block base to the pixel data for a given mipmap level.
	/// \param[in] level the mipmap level [0, getMipLevels)
	/// \return the offset in bytes
	uint32_t getLevelOffset(int32_t level) const;
	///@}

	///@{
	/// Get an offset in bytes from the block base to the pixel data for a given mipmap level and array slice.
	/// \param[in] level the mipmap level [0, #getMipLevels)
	/// \param[in] slice the layer index [0, #getLayers)
	/// \return the offset in bytes
	uint32_t getLevelOffset(int32_t level, uint32_t face) const;
	///@}

	///@{
	/// Get an offset in bytes from the block base to the pixel data for a given mipmap level and array slice.
	/// \param[in] level the mipmap level [0, #getMipLevels)
	/// \param[in] slice the layer index [0, #getLayers)
	/// \return the offset in bytes
	uint32_t getLayerLevelOffset(int32_t level, int32_t slice) const;
	///@}

    /// Loads an image from file-formatted data.
    /// Initialize an image from file-formatted memory; only DDS files are supported
    /// \param[in] fileData the block of memory representing the entire image file
    /// \param[in] size the size of the data block in bytes
    /// \param[in] fileExt the file extension string; must be "dds"
    /// \return true on success, false on failure
    bool loadImageFromFileData(const uint8_t* fileData, size_t size, const char* fileExt);

    /// Convert a flat "cross" image to  a cubemap
    /// Convert a suitable image from a cubemap cross to a cubemap
    /// \return true on success or false for unsuitable source images
    bool convertCrossToCubemap();

    /// Replace (destructive to all content) the storage data and format of the image
    /// Mainly used by image loaders to resize and reformat an already-created image
    /// \param[in] width of the new image
    /// \param[in] height of the new image
    /// \param[in] format NVIMAGE_ format of the new image
    /// \param[in] type NVIMAGE_ type of the component data in the new image
    /// \return true on success or false for failure
    bool reformatImage(int32_t width, int32_t height, uint32_t format, uint32_t type);

    bool setImage( int32_t width, int32_t height, uint32_t format, uint32_t type, const void* data);

    /// Enables or disables automatic swapping of BGR-order images to RGB
    /// \param[in] bgr true allows BGR directly.  False causes the loader
    /// to reformat to RGB
    static void setSupportsBGR(bool bgr) { m_supportsBGR = bgr; }

    /// Gets the status of automatic swapping of BGR-order images to RGB
    /// \return true if BGR images will be swapped, false if they will be passed through
    static bool getSupportsBGR() { return m_supportsBGR; }

    /// Enables or disables automatic expansion of DXT images to RGBA
    /// \param[in] expand true enables DXT-to-RGBA expansion.  False passes 
    /// DXT images through as-is
    static void setDXTExpansion(bool expand) { m_expandDXT = expand; }

    /// Gets the status of automatic DXT expansion
    /// \return true if DXT images will be expanded, false if they will be passed through
    static bool getDXTExpansion() { return m_expandDXT; }

    /// Format handlers descriptor for custom image loading
	struct FormatInfo {
		const char* extension; /// string with the filename extension (no .)
		bool(*reader)(const uint8_t* fileData, size_t size, NvImage& i); /// reader function
		bool(*writer)(uint8_t* fileData, size_t size, NvImage& i); /// optional writer function
	};

    /// Adds a file loader for the given file extension
    /// \param[in] loader the descriptor for the loading system
    /// \return true on success or false on failure (no more loader slots or extension
    /// already handled)
	static bool addFileLoader(const FormatInfo& loader);

protected:
    /// \privatesection

    int32_t _width;
    int32_t _height;
    int32_t _depth;
    int32_t _levelCount;
    int32_t _layers;
    uint32_t _format;
    uint32_t _internalFormat;
    uint32_t _type;
    int32_t _elementSize;
	int32_t _blockSize_x;
	int32_t _blockSize_y;

    bool _cubeMap;

	// the root block from which all levels are taken
	uint8_t* _dataBlock;
	//pointers to the levels
	uint8_t** _data;
	int32_t _dataArrayCount;
	int32_t _dataBlockSize;

    void freeData();
    void flipSurface(uint8_t *surf, int32_t width, int32_t height, int32_t depth);
    uint32_t componentSwapSurface(uint8_t *surf, int32_t width, int32_t height, int32_t depth);
	void expandDXT(uint8_t *surf, int32_t width, int32_t height, int32_t depth, int32_t srcSize);

    //
    // Static elements used to dispatch to proper sub-readers
    //
    //////////////////////////////////////////////////////////////
	enum { MAX_LOADERS = 8 };
	static uint32_t formatLoaderCount;
    static FormatInfo formatTable[MAX_LOADERS];
    static bool vertFlip;
    static bool m_expandDXT;
    static bool m_supportsBGR;

    static bool readDDS(const uint8_t* fileData, size_t size, NvImage& i);

    static void flip_blocks_dxtc1(uint8_t *ptr, uint32_t numBlocks);
    static void flip_blocks_dxtc3(uint8_t *ptr, uint32_t numBlocks);
    static void flip_blocks_dxtc5(uint8_t *ptr, uint32_t numBlocks);
    static void flip_blocks_bc4(uint8_t *ptr, uint32_t numBlocks);
    static void flip_blocks_bc5(uint8_t *ptr, uint32_t numBlocks);

    friend bool TranslateDX10Format( const void *ptr, NvImage &i, int32_t &bytesPerElement, bool &btcCompressed);
};

#endif //NV_IMAGE_H
