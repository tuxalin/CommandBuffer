//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvGLSLProgram.cpp
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
#include "NvGLUtils/NvGLSLProgram.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NV/NvLogs.h"
#include <string>

bool NvGLSLProgram::ms_logAllMissing = false;
const char* NvGLSLProgram::ms_shaderHeader = NULL;

NvGLSLProgram::NvGLSLProgram()
    : m_program(0), m_strict(false)
{
}

NvGLSLProgram::~NvGLSLProgram()
{
    //LOGI("glDeleteProgram(%d)", m_program);
    glDeleteProgram(m_program);
    //CHECK_GL_ERROR();
}

NvGLSLProgram* NvGLSLProgram::createFromFiles(const char* vertFilename, const char* fragFilename, bool strict)
{
    NvGLSLProgram* prog = new NvGLSLProgram;

    if (prog->setSourceFromFiles(vertFilename, fragFilename, strict)) {
        return prog;
    } else {
        delete prog;
        return NULL;
    }
}
NvGLSLProgram* NvGLSLProgram::createFromStrings(const char* vertSrc, const char* fragSrc, bool strict)
{
    NvGLSLProgram* prog = new NvGLSLProgram;

    if (prog->setSourceFromStrings(vertSrc, fragSrc, strict)) {
        return prog;
    } else {
        delete prog;
        return NULL;
    }
}

NvGLSLProgram* NvGLSLProgram::createFromStrings(const char** vertSrcArray, int32_t vertSrcCount, 
    const char** fragSrcArray, int32_t fragSrcCount, bool strict)
{
    NvGLSLProgram* prog = new NvGLSLProgram;

    if (prog->setSourceFromStrings(vertSrcArray, vertSrcCount, fragSrcArray, fragSrcCount, strict)) {
        return prog;
    } else {
        delete prog;
        return NULL;
    }
}

bool NvGLSLProgram::setSourceFromFiles(const char* vertFilename, const char* fragFilename, bool strict)
{
    int32_t len;
    char* vertSrc = NvAssetLoaderRead(vertFilename, len);
    char* fragSrc = NvAssetLoaderRead(fragFilename, len);
    if (!vertSrc || !fragSrc) {
        NvAssetLoaderFree(vertSrc);
        NvAssetLoaderFree(fragSrc);
        return false;
    }

    bool success = setSourceFromStrings(vertSrc, fragSrc, strict);

    NvAssetLoaderFree(vertSrc);
    NvAssetLoaderFree(fragSrc);

    return success;
}

bool NvGLSLProgram::setSourceFromStrings(const char* vertSrc, const char* fragSrc, bool strict)
{
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    m_strict = strict;

    m_program = compileProgram(vertSrc, fragSrc);

    return m_program != 0;
}

bool NvGLSLProgram::setSourceFromStrings(const char** vertSrcArray, int32_t vertSrcCount, 
    const char** fragSrcArray, int32_t fragSrcCount, bool strict)
{
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    m_strict = strict;

    m_program = compileProgram(vertSrcArray, vertSrcCount, fragSrcArray, fragSrcCount);

    return m_program != 0;
}

bool NvGLSLProgram::setSourceFromStrings(ShaderSourceItem* src, int32_t count, bool strict)
{
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    m_strict = strict;

    m_program = compileProgram(src, count);

    return m_program != 0;
}

void NvGLSLProgram::enable()
{
    glUseProgram(m_program);
}

void NvGLSLProgram::disable()
{
    glUseProgram(0);
}

bool NvGLSLProgram::checkCompileError(GLuint shader, int32_t target)
{
    // check if shader compiled
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled || m_strict)
    {
        if (!compiled) {
            LOGI("Error compiling shader");
        }
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = new char[infoLen];
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                LOGI("Shader log:\n%s\n", buf);
                delete[] buf;
            }
        }
        if (!compiled) {
            glDeleteShader(shader);
            shader = 0;
            return false;
        }
    }
    return true;
}

GLuint NvGLSLProgram::compileProgram(const char *vsource, const char *fsource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* sourceItems[2];
    int sourceCount = 0;
    if (ms_shaderHeader)
        sourceItems[sourceCount++] = ms_shaderHeader;
    sourceItems[sourceCount++] = vsource;

    glShaderSource(vertexShader, sourceCount, sourceItems, 0);

    sourceCount = 0;
    if (ms_shaderHeader)
        sourceItems[sourceCount++] = ms_shaderHeader;
    sourceItems[sourceCount++] = fsource;

    glShaderSource(fragmentShader, sourceCount, sourceItems, 0);

    glCompileShader(vertexShader);
    if (!checkCompileError(vertexShader, GL_VERTEX_SHADER))
        return 0;

    glCompileShader(fragmentShader);
    if (!checkCompileError(fragmentShader, GL_FRAGMENT_SHADER))
        return 0;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // can be deleted since the program will keep a reference
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = new char[bufLength];
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGI("Could not link program:\n%s\n", buf);
                delete [] buf;
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    return program;
}

GLuint NvGLSLProgram::compileProgram(
        const char** vertSrcArray, int32_t vertSrcCount,
        const char** fragSrcArray, int32_t fragSrcCount)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    {
        const char** sourceItems = new const char*[vertSrcCount + 1];
        int sourceCount = 0;
        if (ms_shaderHeader)
            sourceItems[sourceCount++] = ms_shaderHeader;

        for (int i = 0; i < vertSrcCount; i++)
            sourceItems[sourceCount++] = vertSrcArray[i];

        glShaderSource(vertexShader, sourceCount, sourceItems, 0);

        delete[] sourceItems;
    }

    {
        const char** sourceItems = new const char*[fragSrcCount + 1];
        int sourceCount = 0;
        if (ms_shaderHeader)
            sourceItems[sourceCount++] = ms_shaderHeader;

        for (int i = 0; i < fragSrcCount; i++)
            sourceItems[sourceCount++] = fragSrcArray[i];

        glShaderSource(fragmentShader, sourceCount, sourceItems, 0);

        delete[] sourceItems;
    }

    glCompileShader(vertexShader);
    if (!checkCompileError(vertexShader, GL_VERTEX_SHADER))
        return 0;

    glCompileShader(fragmentShader);
    if (!checkCompileError(fragmentShader, GL_FRAGMENT_SHADER))
        return 0;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // can be deleted since the program will keep a reference
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = new char[bufLength];
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGI("Could not link program:\n%s\n", buf);
                delete [] buf;
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    return program;
}



GLuint NvGLSLProgram::compileProgram(ShaderSourceItem* src, int32_t count)
{
    GLuint program = glCreateProgram();

    int32_t i;
    for (i = 0; i < count; i++) {
        GLuint shader = glCreateShader(src[i].type);

        const char* sourceItems[2];
        int sourceCount = 0;
        if (ms_shaderHeader)
            sourceItems[sourceCount++] = ms_shaderHeader;
        sourceItems[sourceCount++] = (src[i].src);

        glShaderSource(shader, sourceCount, sourceItems, 0);
        glCompileShader(shader);
        if (!checkCompileError(shader, src[i].type))
            return 0;

        glAttachShader(program, shader);

        // can be deleted since the program will keep a reference
        glDeleteShader(shader);
    }

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = new char[bufLength];
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGI("Could not link program:\n%s\n", buf);
                delete [] buf;
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    return program;
}

bool NvGLSLProgram::relink()
{
    glLinkProgram(m_program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint bufLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = new char[bufLength];
            if (buf) {
                glGetProgramInfoLog(m_program, bufLength, NULL, buf);
                LOGI("Could not link program:\n%s\n", buf);
                delete [] buf;
            }
        }
        return false;
    }
    return true;
}

GLint NvGLSLProgram::getAttribLocation(const char* attribute, bool isOptional)
{
    GLint result = glGetAttribLocation(m_program, attribute);

    if (result == -1)
    {
        if((ms_logAllMissing || m_strict) && !isOptional) {
            LOGI
            (
                "could not find attribute \"%s\" in program %d",
                attribute,
                m_program
            );
        }
    }

    return result;
}

GLint NvGLSLProgram::getUniformLocation(const char* uniform, bool isOptional)
{
    GLint result = glGetUniformLocation(m_program, uniform);

    if (result == -1)
    {
        if((ms_logAllMissing || m_strict) && !isOptional) {
            LOGI
            (
                "could not find uniform \"%s\" in program %d",
                uniform,
                m_program
            );
        }
    }

    return result;
}

void NvGLSLProgram::bindTexture2D(const char *name, int32_t unit, GLuint tex)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1i(loc, unit);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, tex);
    }
}

void NvGLSLProgram::bindTexture2D(GLint index, int32_t unit, GLuint tex)
{
    glUniform1i(index, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, tex);
}

void NvGLSLProgram::bindTexture2DMultisample(const char *name, int32_t unit, GLuint tex)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1i(loc, unit);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(0x9100/*GL_TEXTURE_2D_MULTISAMPLE*/, tex);
    }
}

void NvGLSLProgram::bindTexture2DMultisample(GLint index, int32_t unit, GLuint tex)
{
    glUniform1i(index, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(0x9100/*GL_TEXTURE_2D_MULTISAMPLE*/, tex);
}

void NvGLSLProgram::bindTextureRect(const char *name, int32_t unit, GLuint tex)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1i(loc, unit);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(0x84F5/*GL_TEXTURE_RECT*/, tex);
    }
}

void NvGLSLProgram::bindTextureRect(GLint index, int32_t unit, GLuint tex)
{
    glUniform1i(index, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(0x84F5/*GL_TEXTURE_RECT*/, tex);
}

void NvGLSLProgram::bindTextureArray(const char *name, int32_t unit, GLuint tex)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1i(loc, unit);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(0x8c1a, tex); // GL_TEXTURE_2D_ARRAY
    }
}

void NvGLSLProgram::bindTextureArray(GLint index, int32_t unit, GLuint tex)
{
    glUniform1i(index, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(0x8c1a, tex); // GL_TEXTURE_2D_ARRAY

}

void
NvGLSLProgram::setUniform1i(const char *name, int32_t value)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1i(loc, value);
    }
}

void
NvGLSLProgram::setUniform1i(GLint index, int32_t value)
{
    if (index >= 0) {
        glUniform1i(index, value);
    }
}

void
NvGLSLProgram::setUniform2i(const char *name, int32_t x, int32_t y)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform2i(loc, x, y);
    }
}

void
NvGLSLProgram::setUniform2i(GLint index, int32_t x, int32_t y)
{
    if (index >= 0) {
        glUniform2i(index, x, y);
    }
}

void
NvGLSLProgram::setUniform3i(const char *name, int32_t x, int32_t y, int32_t z)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform3i(loc, x, y, z);
    }
}

void
NvGLSLProgram::setUniform3i(GLint index, int32_t x, int32_t y, int32_t z)
{
    if (index >= 0) {
        glUniform3i(index, x, y, z);
    }
}

void
NvGLSLProgram::setUniform1f(const char *name, float value)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform1f(loc, value);
    }
}

void
NvGLSLProgram::setUniform1f(GLint index, float value)
{
    if (index >= 0) {
        glUniform1f(index, value);
    }
}

void
NvGLSLProgram::setUniform2f(const char *name, float x, float y)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform2f(loc, x, y);
    }
}

void
NvGLSLProgram::setUniform2f(GLint index, float x, float y)
{
    if (index >= 0) {
        glUniform2f(index, x, y);
    }
}

void
NvGLSLProgram::setUniform3f(const char *name, float x, float y, float z)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform3f(loc, x, y, z);
    }
}

void
NvGLSLProgram::setUniform3f(GLint index, float x, float y, float z)
{
    if (index >= 0) {
        glUniform3f(index, x, y, z);
    }
}

void
NvGLSLProgram::setUniform4f(const char *name, float x, float y, float z, float w)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform4f(loc, x, y, z, w);
    }
}

void
NvGLSLProgram::setUniform4f(GLint index, float x, float y, float z, float w)
{
    if (index >= 0) {
        glUniform4f(index, x, y, z, w);
    }
}

void
NvGLSLProgram::setUniform3fv(const char *name, const float *value, int32_t count)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform3fv(loc, count, value);
    }

}

void
NvGLSLProgram::setUniform3fv(GLint index, const float *value, int32_t count)
{
    if (index >= 0) {
        glUniform3fv(index, count, value);
    }

}

void
NvGLSLProgram::setUniform4fv(const char *name, const float *value, int32_t count)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniform4fv(loc, count, value);
    }
}

void
NvGLSLProgram::setUniform4fv(GLint index, const float *value, int32_t count)
{
    if (index >= 0) {
        glUniform4fv(index, count, value);
    }
}

void
NvGLSLProgram::setUniformMatrix4fv(const char *name, float *m, int32_t count, bool transpose)
{
    GLint loc = getUniformLocation(name, false);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, count, transpose, m);
    }
}

void
NvGLSLProgram::setUniformMatrix4fv(GLint index, float *m, int32_t count, bool transpose)
{
    if (index >= 0) {
        glUniformMatrix4fv(index, count, transpose, m);
    }
}
