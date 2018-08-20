//----------------------------------------------------------------------------------
// File:        NvAppBase\android/MainAndroid.cpp
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
//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include "NvAndroidNativeAppGlue.h"
#include "NvAndroidWinUtil.h"
#include "NvThreadAnd.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "NvAppBase/NvAppBase.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NV/NvStopWatch.h"
#include "EngineAndroid.h"
#include "NV/NvLogs.h"
#include "NV/NvTokenizer.h"

class NvAndroidStopWatch: public NvStopWatch 
{
public:
    //! Constructor, default
    NvAndroidStopWatch() : 
        start_time(), diff_time( 0.0)
    { };

    // Destructor
    ~NvAndroidStopWatch()
    { };

public:
    //! Start time measurement
    void start() {
        gettimeofday( &start_time, 0);
        m_running = true;
    }

    //! Stop time measurement
    void stop() {
        diff_time = getDiffTime();
        m_running = false;
    }

    //! Reset time counters to zero
    void reset()
    {
        diff_time = 0;
        if( m_running )
            gettimeofday( &start_time, 0);
    }

    const float getTime() const {
        if(m_running) {
            return getDiffTime();
        } else {
            // time difference in seconds
            return  diff_time;
        }
    }

private:

    // helper functions
      
    //! Get difference between start time and current time
    float getDiffTime() const {
        struct timeval t_time;
        gettimeofday( &t_time, 0);
        return  (float) (( t_time.tv_sec - start_time.tv_sec) 
                    + (0.000001 * (t_time.tv_usec - start_time.tv_usec)) );
    }

    // member variables

    //! Start of measurement
    struct timeval  start_time;

    //! Time difference between the last start and stop
    float  diff_time;

    //! Number of times clock has been started
    //! and stopped to allow averaging
    int32_t clock_sessions;
};


bool getCommandLine(Engine* engine, char*& commandLine);

extern void NvInitSharedFoundation(void);
extern void NvReleaseSharedFoundation(void);

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    if (!app)
        return;

	// Make sure glue isn't stripped.
	app_dummy();

    NvInitSharedFoundation();

    NvAssetLoaderInit((void*)app->activity->assetManager);

    Engine* engine = new Engine(app);

	char* commandLine = NULL;
	if (getCommandLine(engine, commandLine)) {
		LOGI("Found command line %s", commandLine);
		// add command line arguments
		NvTokenizer cmdtok(commandLine);
		std::string sarg;
		while (cmdtok.getTokenString(sarg)) {
			LOGI("Argument %s", sarg.c_str());
			NvAppBase::commandLineAppend(sarg);
		}
	}


    NvAppBase* sdkapp = NvAppFactory();
	sdkapp->setPlatformContext(engine);
    engine->mAppBase = sdkapp;
	sdkapp->setThreadManager(new NvThreadManagerAnd);

	// Check for a requested size
	const int32_t defaultWidth = 1280;
	const int32_t defaultHeight = 720;
	int32_t width = defaultWidth;
	int32_t height = defaultHeight;
	if (!sdkapp->getRequestedWindowSize(width, height)) {
		// for safety, reset the height and width if the app returns false in case
		// they did a bad thing and changed the values but returned false
		width = defaultWidth;
		height = defaultHeight;
	}

	// Check if connected sink is HDR compatible and set flag.	
	engine->mWin->setHDRSinkFlag(engine->isDisplayHDRCompatible());

	// Check if the application has declared support for HDR
	engine->mWin->setHDRAppFlag(sdkapp->getHDRReady());
	
	// Set HDR in Base application notifying if the final output should be in HDR or not.
	// This flag can be used within the application to make decisions based on the nature of the final output.
	sdkapp->setHDREnable(engine->mWin->getHDRAppFlag() && engine->mWin->getHDRSinkFlag());

	if (!sdkapp->initialize(NvPlatformInfo(NvPlatformCategory::PLAT_MOBILE, NvPlatformOS::OS_ANDROID), width, height)) {
		// if we have a basic EGL failure, we need to exit immediately; nothing else we can do
		//nv_app_force_quit_no_cleanup(app);
	}

	engine->mWin = NvAndroidWinUtil::getDefaultWindow();

    // loop waiting for stuff to do.
    sdkapp->mainLoop();

    delete sdkapp;
    delete engine;

    NvReleaseSharedFoundation();

    NvAssetLoaderShutdown();

	exit(0);
}

// Timer and timing functions
NvStopWatch* NvAppBase::createStopWatch() {
    return new NvAndroidStopWatch;
}

uint64_t NvAppBase::queryPerformanceCounter() {
    struct timeval t_time;
    gettimeofday( &t_time, 0);
    return t_time.tv_sec * 1000000 + t_time.tv_usec;
}

uint64_t NvAppBase::queryPerformanceCounterFrequency() {
    return 1000000; // 1MHz
}

// If we cause an exception in JNI, we print the exception info to
// the log, we clear the exception to avoid a pending-exception
// crash, and we force the function to return.
#define EXCEPTION_RETURN(env) \
    if (env->ExceptionOccurred()) { \
        env->ExceptionDescribe(); \
        env->ExceptionClear(); \
        return false; \
    }

bool NvAppBase::showDialog(const char* title, const char *contents, bool exitApp) {
    Engine* engine = (Engine*)getPlatformContext();
    struct android_app* app = engine->mApp;

       jstring jniTitle = app->appThreadEnv->NewStringUTF(title);
    EXCEPTION_RETURN(app->appThreadEnv);

       jstring jniContents = app->appThreadEnv->NewStringUTF(contents);
    EXCEPTION_RETURN(app->appThreadEnv);

    jclass thisClass = app->appThreadEnv->GetObjectClass(app->appThreadThis);
    EXCEPTION_RETURN(app->appThreadEnv);

    jmethodID showToastAlert = app->appThreadEnv->GetMethodID(thisClass, 
        "showAlert", "(Ljava/lang/String;Ljava/lang/String;Z)V");
    EXCEPTION_RETURN(app->appThreadEnv);

    if (!isTestMode())
    {
        app->appThreadEnv->CallVoidMethod(app->appThreadThis, showToastAlert, jniTitle, jniContents, exitApp);
        EXCEPTION_RETURN(app->appThreadEnv);
    }

    return true;
}

bool getCommandLine(Engine* engine, char*& commandLine) {
    struct android_app* app = engine->mApp;
    
    jclass thisClass = app->appThreadEnv->GetObjectClass(app->appThreadThis);
    EXCEPTION_RETURN(app->appThreadEnv);

    jfieldID field = app->appThreadEnv->GetFieldID( thisClass, "mCommandLine", "Ljava/lang/String;" );
    EXCEPTION_RETURN(app->appThreadEnv);

    jstring jCommandStr = (jstring)app->appThreadEnv->GetObjectField(app->appThreadThis, field);
    EXCEPTION_RETURN(app->appThreadEnv);

    const char* commandLineChars = app->appThreadEnv->GetStringUTFChars(jCommandStr, 0);
    EXCEPTION_RETURN(app->appThreadEnv);

    LOGI("commandLineChars = %s", commandLineChars);

    if (commandLineChars) {
        int length = strlen(commandLineChars) + 1;
        commandLine = new char[length];
        strncpy(commandLine, commandLineChars, length);
    } else {
        commandLine = new char[1];
        commandLine[0] = 0;
    }

    app->appThreadEnv->ReleaseStringUTFChars(jCommandStr, commandLineChars);
       EXCEPTION_RETURN(app->appThreadEnv);

    return true;
}

bool NvAppBase::writeScreenShot(int32_t width, int32_t height, const uint8_t* data, const std::string& path) {

    std::string filename = "/sdcard/" + path + ".bmp";
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp)
        return false;

    uint8_t* dest = new uint8_t[width * height * 4];

    uint8_t* tmpDest = dest;

    int32_t pixels = width * height;

    for (int i = 0; i < pixels; i++) {
        *(tmpDest++) = data[2];
        *(tmpDest++) = data[1];
        *(tmpDest++) = data[0];
        data += 4;
    }

    int8_t header = 'B';
    fwrite(&header, sizeof(header), 1, fp);

    header = 'M';
    fwrite(&header, sizeof(header), 1, fp);

    uint32_t filesize = 14 + 40 + width * height *3;
    fwrite(&filesize, sizeof(filesize), 1, fp);

    uint16_t reserved1 = 0;
    fwrite(&reserved1, sizeof(reserved1), 1, fp);

    uint16_t reserved2 = 0;
    fwrite(&reserved2, sizeof(reserved2), 1, fp);

    uint32_t offset = 14 + 40;
    fwrite(&offset, sizeof(offset), 1, fp);

    uint32_t size = 40;
    fwrite(&size, sizeof(size), 1, fp);

    int32_t w = width;
    fwrite(&w, sizeof(w), 1, fp);

    int32_t h = height;
    fwrite(&h, sizeof(h), 1, fp);

    uint16_t planes = 1;
    fwrite(&planes, sizeof(planes), 1, fp);

    uint16_t bits = 24;
    fwrite(&bits, sizeof(bits), 1, fp);

    uint32_t compression = 0;
    fwrite(&compression, sizeof(compression), 1, fp);

    uint32_t imagesize = w * h * 3;
    fwrite(&imagesize, sizeof(imagesize), 1, fp);

    int32_t xresolution = 0;
    fwrite(&xresolution, sizeof(xresolution), 1, fp);

    int32_t yresolution = 0;
    fwrite(&yresolution, sizeof(yresolution), 1, fp);

    uint32_t ncolours  = 0;
    fwrite(&ncolours, sizeof(ncolours), 1, fp);

    uint32_t importantcolours = 0;
    fwrite(&importantcolours, sizeof(importantcolours), 1, fp);

    fwrite(dest, w * h * 3, 1, fp);

    delete[] dest;

    fclose(fp);

    return true;
}

bool NvAppBase::writeLogFile(const std::string& path, bool append, const char* fmt, ...) {
    va_list ap;
  
    std::string filename = "/sdcard/" + path + ".txt";
    FILE* fp = fopen(filename.c_str(), append ? "a" : "w");
    if (!fp)
        return false;

    va_start(ap, fmt); 
    vfprintf(fp, fmt, ap);
    fprintf(fp, "\n");
    fclose(fp);

    return true;
}

void NvAppBase::forceLinkHack() {
}

void NVPlatformLog(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt); 
    __android_log_vprint(ANDROID_LOG_INFO,"NvSDK",fmt, ap);
}
