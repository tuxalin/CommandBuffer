//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvGLSLProgram.h
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

#ifndef NV_GLSL_PROGRAM_H
#define NV_GLSL_PROGRAM_H

#include <NvSimpleTypes.h>
#include "NV/NvPlatformGL.h"

/// \file
/// GLSL shader program wrapper

/// Convenience wrapper for GLSL shader programs.
/// Wraps shader programs and simplifies creation, setting uniforms and setting
/// vertex attributes.  Supports all forms of shaders, but has simple paths for
/// the common case of shader programs consisting of only vertex and fragment
/// shaders.
class NvGLSLProgram
{
public:
    /// Represents a piece of shader source and the shader type
    /// Used with creation functions to pass in arrays of multiple
    /// shader source types.
    struct ShaderSourceItem {
        const char* src; ///< Null-terminated string containing the shader source code
        GLint type; ///< The GL_*_SHADER enum representing the shader type
    };

    /// Default constructor.
    /// Creates an empty object with no shader program, because we cannot set the source
    /// in the constructor.  Doing so would require the possibility of failure, and we cannot
    /// fail a constructor.
    NvGLSLProgram();
    ~NvGLSLProgram();

    /// Creates and returns a shader object from a pair of filenames/paths
    /// Uses #NvAssetLoaderRead to load the files.  Convenience function.
    /// \param[in] vertFilename the filename and partial path to the text file containing the vertex shader source
    /// \param[in] fragFilename the filename and partial path to the text file containing the fragment shader source
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return a pointer to an #NvGLSLProgram on success and NULL on failure
    static NvGLSLProgram* createFromFiles(const char* vertFilename, const char* fragFilename, bool strict = false);

    /// Creates and returns a shader object from a pair of source strings
    /// Convenience function.
    /// \param[in] vertSrc the null-terminated string containing the vertex shader source
    /// \param[in] fragSrc the null-terminated string containing the fragment shader source
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return a pointer to an #NvGLSLProgram on success and NULL on failure
    static NvGLSLProgram* createFromStrings(const char* vertSrc, const char* fragSrc, bool strict = false);

    /// Creates and returns a shader object from a pair of arrays of source strings
    /// Convenience function.
    /// \param[in] vertSrcArray the array of null-terminated strings containing the vertex shader source
    /// \param[in] vertSrcCount the number of strings in vertSrcArray
    /// \param[in] fragSrcArray the array of null-terminated strings containing the fragment shader source
    /// \param[in] fragSrcCount the number of strings in fragSrcArray
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return a pointer to an #NvGLSLProgram on success and NULL on failure
    static NvGLSLProgram* createFromStrings(const char** vertSrcArray, int32_t vertSrcCount, 
        const char** fragSrcArray, int32_t fragSrcCount, bool strict = false);

    /// Sets a global shader text header to be prepended to all shaders created through
    /// this class.  This can be useful in cases where the shaders need a different version
    /// header on different platforms.
    /// \param[in] header the string to be appended or NULL if no header.  NULL also
    /// disables the feature as desired.  String is not copied or destroyed.
	static void setGlobalShaderHeader(const char* header) { ms_shaderHeader = header; }

	/// Returns the current global shader header or NULL if none is active
	/// \return the current global shader header or NULL if none is active
	static const char* getGlobalShaderHeader() { return ms_shaderHeader; }

    /// Initializes an existing shader object from a pair of filenames/paths
    /// Uses #NvAssetLoaderRead to load the files.  Convenience function.
    /// \param[in] vertFilename the filename and partial path to the text file containing the vertex shader source
    /// \param[in] fragFilename the filename and partial path to the text file containing the fragment shader source
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return true on success and false on failure
    bool setSourceFromFiles(const char* vertFilename, const char* fragFilename, bool strict = false);

    /// Creates and returns a shader object from a pair of source strings
    /// \param[in] vertSrc the null-terminated string containing the vertex shader source
    /// \param[in] fragSrc the null-terminated string containing the fragment shader source
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return true on success and false on failure
    bool setSourceFromStrings(const char* vertSrc, const char* fragSrc, bool strict = false);

    /// Creates and returns a shader object from a pair of source strings
    /// Convenience function.
    /// \param[in] vertSrc the null-terminated string containing the vertex shader source
    /// \param[in] fragSrc the null-terminated string containing the fragment shader source
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return a pointer to an #NvGLSLProgram on success and NULL on failure
    bool setSourceFromStrings(const char** vertSrcArray, int32_t vertSrcCount, const char** fragSrcArray, 
        int32_t fragSrcCount, bool strict = false);

    /// Creates and returns a shader object from an array of #ShaderSourceItem source objects
    /// \param[in] src an array of #ShaderSourceItem objects containing the shaders sources to
    /// be loaded.  Unlike the vert/frag-only creation functions, this version can accept additional
    /// shader types such as geometry and tessellation shaders (if supported)
    /// \param[in] count the number of elements in #src array
    /// \param[in] strict if set to true, then later calls to retrieve the locations of non-
    /// existent uniforms and vertex attributes will log a warning to the output
    /// \return true on success and false on failure
    bool setSourceFromStrings(ShaderSourceItem* src, int32_t count, bool strict = false);

    /// Binds the given shader program as current in the GL context
    void enable();

    /// Unbinds the given shader program from the GL context (binds shader 0)
    void disable();

    /// Binds a 2D texture to a shader uniform by name.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string name of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTexture2D(const char *name, int32_t unit, GLuint tex);

    /// Binds a 2D texture to a shader uniform by index.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTexture2D(GLint index, int32_t unit, GLuint tex);

    /// Binds a 2D Multisample texture to a shader uniform by name.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string name of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    /// \note Requires GL 3.2 or GLES 3.1
    void bindTexture2DMultisample(const char *name, int32_t unit, GLuint tex);

    /// Binds a 2D Multisample texture to a shader uniform by index.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    /// \note Requires GL 3.2 or GLES 3.1
    void bindTexture2DMultisample(GLint index, int32_t unit, GLuint tex);

    /// Binds a Rect texture to a shader uniform by name.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string name of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTextureRect(const char *name, int32_t unit, GLuint tex);

    /// Binds a Rect texture to a shader uniform by index.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTextureRect(GLint index, int32_t unit, GLuint tex);

    /// Binds an array texture to a shader uniform by name.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string name of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTextureArray(const char *name, int32_t unit, GLuint tex);

    /// Binds an array texture to a shader uniform by index.
    /// Binds the given texture to the supplied texture unit and the unit to the given uniform
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[unit] the texture unit which will be used for the texture
    /// \param[tex] the texture to be bound
    void bindTextureArray(GLint index, int32_t unit, GLuint tex);

    //@{
    /// Set scalar program uniform by string name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string with the name of the uniform
    /// \param[in] value scalar value of the uniform
    void setUniform1i(const char *name, int32_t value);
    void setUniform1f(const char *name, float value);
    //@}

    //@{
    /// Set 2-vec program uniform by string name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string with the name of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    void setUniform2i(const char *name, int32_t x, int32_t y);
    void setUniform2f(const char *name, float x, float y);
    //@}

    //@{
    /// Set 3-vec program uniform by string name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string with the name of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    /// \param[in] z third value of vector uniform
    void setUniform3i(const char *name, int32_t x, int32_t y, int32_t z);
    void setUniform3f(const char *name, float x, float y, float z);
    //@}

    //@{
    /// Set 4-vec program uniform by string name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string with the name of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    /// \param[in] z third value of vector uniform
    /// \param[in] w fourth value of vector uniform
    void setUniform4f(const char *name, float x, float y, float z, float w);
    //@}

    //@{
    /// Set vector program uniform array by string name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] value array of values
    /// \param[in] count number of values in array unform
    void setUniform3fv(const char *name, const float *value, int32_t count=1);
    void setUniform4fv(const char *name, const float *value, int32_t count=1);
    //@}

    ///@{
    /// Set scalar program uniforms by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] value value of scale uniform
    void setUniform1i(GLint index, int32_t value);
    void setUniform1f(GLint index, float value);
    //@}

    ///@{
    /// Set 2-vec program uniforms by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    void setUniform2i(GLint index, int32_t x, int32_t y);
    void setUniform2f(GLint index, float x, float y);
    //@}

    ///@{
    /// Set 3-vec program uniforms by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    /// \param[in] z third value of vector uniform
    void setUniform3i(GLint index, int32_t x, int32_t y, int32_t z);
    void setUniform3f(GLint index, float x, float y, float z);
    //@}

    ///@{
    /// Set 4-vec program uniforms by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] x first value of vector uniform
    /// \param[in] y second value of vector uniform
    /// \param[in] z third value of vector uniform
    /// \param[in] w fourth value of vector uniform
    void setUniform4f(GLint index, float x, float y, float z, float w);
    ///@}

    //@{
    /// Set vector program uniform array by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] value array of values
    /// \param[in] count number of values in array unform
    void setUniform3fv(GLint index, const float *value, int32_t count=1);
    void setUniform4fv(GLint index, const float *value, int32_t count=1);
    ///@}

    /// Set matrix array program uniform array by name
    /// Assumes that the given shader is bound via #enable
    /// \param[in] name the null-terminated string name of the uniform
    /// \param[in] m array of matrices
    /// \param[in] count number of values in array unform
    /// \param[in] transpose if true, the matrices are transposed on input
    void setUniformMatrix4fv(const GLchar *name, GLfloat *m, int32_t count=1, bool transpose=false);

    /// Set matrix array program uniform array by index
    /// Assumes that the given shader is bound via #enable
    /// \param[in] index the index of the uniform
    /// \param[in] m array of matrices
    /// \param[in] count number of values in array unform
    /// \param[in] transpose if true, the matrices are transposed on input
    void setUniformMatrix4fv(GLint index, GLfloat *m, int32_t count=1, bool transpose=false);

    /// Returns the index containing the named vertex attribute
    /// \param[in] uniform the null-terminated string name of the attribute
    /// \param[in] isOptional if true, the function logs an error if the attribute is not found
    /// \return the non-negative index of the attribute if found.  -1 if not found
    GLint getAttribLocation(const char* attribute, bool isOptional = false);

    /// Returns the index containing the named uniform
    /// \param[in] uniform the null-terminated string name of the uniform
    /// \param[in] isOptional if true, the function logs an error if the uniform is not found
    /// \return the non-negative index of the uniform if found.  -1 if not found
    GLint getUniformLocation(const char* uniform, bool isOptional = false);

    /// Returns the GL program object for the shader
    /// \return the GL shader object ID
    GLuint getProgram() { return m_program; }

    /// Relinks an existing shader program to update based on external changes
    bool relink();

    /// Enables logging of missing uniforms even for non-strict shaders
    /// \param[in] logMissing if set to true, missing uniforms are logged when set,
    /// even if the shader was not created with the strict flag
    static void setLogAllMissing(bool logMissing) { ms_logAllMissing = logMissing; }

protected:
    bool checkCompileError(GLuint object, int32_t target);
    GLuint compileProgram(const char *vsource, const char *fsource);
    GLuint compileProgram(const char** vertSrcArray, int32_t vertSrcCount, 
        const char** fragSrcArray, int32_t fragSrcCount);
    GLuint compileProgram(ShaderSourceItem* src, int32_t count);

    bool m_strict;
    GLuint m_program;

    static bool ms_logAllMissing;
    static const char* ms_shaderHeader;
};

/// Convenience class to automatically push and pop a shader prefix
/// using scoped auto-destruction.  The following code:
/// \code
///     {
///         NvScopedShaderPrefix prefix("prefix code");
///         // ... my block of prefixed code
///     }
/// \endcode
/// Is equivalent to:
/// \code
///     {
///         NvGLSLProgram::setGlobalShaderHeader("prefix code");
///         // ... my block of prefixed code
///         NvGLSLProgram::setGlobalShaderHeader(NULL);
///     }
/// \endcode
class NvScopedShaderPrefix
{
public:
    NvScopedShaderPrefix(const char* prefix) { NvGLSLProgram::setGlobalShaderHeader(prefix); }
    ~NvScopedShaderPrefix() { NvGLSLProgram::setGlobalShaderHeader(NULL); }
};

#endif // NV_GLSL_PROGRAM_H
