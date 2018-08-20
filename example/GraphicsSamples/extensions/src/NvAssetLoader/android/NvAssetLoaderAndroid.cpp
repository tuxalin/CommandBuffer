//----------------------------------------------------------------------------------
// File:        NvAssetLoader\android/NvAssetLoaderAndroid.cpp
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
#include "NvAssetLoader/NvAssetLoader.h"
#include "NV/NvLogs.h"
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

static AAssetManager* s_assetManager = NULL;

bool NvAssetLoaderInit(void* platform)
{
    if (!platform)
        return false;

    s_assetManager = (AAssetManager*)platform;

    return true;
}

bool NvAssetLoaderShutdown()
{
    s_assetManager = NULL;
    return true;
}

bool NvAssetLoaderAddSearchPath(const char *)
{
    return true;
}

bool NvAssetLoaderRemoveSearchPath(const char *)
{
    return true;
}

char *NvAssetLoaderRead(const char *filePath, int32_t &length)
{
    char *buff = NULL;

    if (!s_assetManager)
        return NULL;

    AAsset *fileAsset = AAssetManager_open(s_assetManager, filePath, AASSET_MODE_BUFFER);

    if(fileAsset != NULL)
    {
        length = AAsset_getLength(fileAsset);
        buff = new char[length+1];
        int32_t numBytes = AAsset_read(fileAsset, buff, length);
        buff[length] = '\0';

        LOGI("Read asset '%s', %d bytes", filePath, numBytes);
        //LOGI(" %s\n", buff);

        AAsset_close(fileAsset);
    }

    return buff;
}

bool NvAssetLoaderFree(char* asset)
{
    delete[] asset;
    return true;
}

bool NvAssetLoaderFileExists(const char *filePath) {
    NvAssetFilePtr fp = NvAssetLoaderOpenFile(filePath);
    if (fp != NULL) {
        NvAssetLoaderCloseFile(fp);
        return true;
    }
    return false;
}

NvAssetFilePtr NvAssetLoaderOpenFile(const char* name) {
    if (!s_assetManager)
        return NULL;

    return (NvAssetFilePtr)AAssetManager_open(s_assetManager, name, AASSET_MODE_BUFFER);
}

void NvAssetLoaderCloseFile(NvAssetFilePtr fp) {
    AAsset_close((AAsset*)fp);
}

int32_t NvAssetFileGetSize(NvAssetFilePtr fp) {
    return AAsset_getLength((AAsset*)fp);
}

int32_t NvAssetFileSeek(NvAssetFilePtr fp, int32_t offset, NvAssetSeekBase whence) {
    return AAsset_seek((AAsset*)fp, offset, (int32_t)whence);
}

int32_t NvAssetFileRead(NvAssetFilePtr file, int32_t size, void* data) {
    return AAsset_read((AAsset*)file, data, size);
}

int64_t NvAssetFileGetSize64(NvAssetFilePtr fp) {
    return AAsset_getLength64((AAsset*)fp);
}

int64_t NvAssetFileRead64(NvAssetFilePtr file, int64_t size, void* data) {
    return AAsset_read((AAsset*)file, data, size);
}

int64_t NvAssetFileSeek64(NvAssetFilePtr fp, int64_t offset, NvAssetSeekBase whence) {
    return AAsset_seek64((AAsset*)fp, offset, (int32_t)whence);
}

bool NvAssetGetFilePath(const std::string fileName, std::string& path) {
	return false;
}
