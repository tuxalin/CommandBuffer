//----------------------------------------------------------------------------------
// File:        NvAppBase\linux/MainLinux.cpp
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
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <stdarg.h>

#include "NvAppBase/NvAppBase.h"
#include "NV/NvStopWatch.h"
#include "NvGamepad/linux/NvGamepadLinux.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvThreadPosix.h"
#include "NV/NvLogs.h"

class NvLinuxStopWatch: public NvStopWatch
{
public:
    //! Constructor, default
    NvLinuxStopWatch() :
        start_time(), diff_time( 0.0)
    { };

    // Destructor
    ~NvLinuxStopWatch()
    { };

public:
    //! Start time measurement
    void start() {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
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
            start();
    }

    const float getTime() const {
        if(m_running) {
            return getDiffTime();
        } else {
            // time difference in milli-seconds
            return  diff_time;
        }
    }

private:

    // helper functions
      
    //! Get difference between start time and current time
    float getDiffTime() const {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return  (float) (( now.tv_sec - start_time.tv_sec)
                    + (0.000000001 * (now.tv_nsec - start_time.tv_nsec)) );
    }

    // member variables

    //! Start of measurement
    struct timespec  start_time;

    //! Time difference between the last start and stop
    float  diff_time;
};

extern void NvInitSharedFoundation(void);
extern void NvReleaseSharedFoundation(void);


extern void NvInitSharedFoundation(void);
extern void NvReleaseSharedFoundation(void);

// program entry
int32_t main(int32_t argc, char *argv[])
{
    NvInitSharedFoundation();

    NvAssetLoaderInit(NULL);

    // add command line arguments
    for (int i = 1; i < argc; i++) {
        NvAppBase::commandLineAppend(argv[i]);
    }

    NvAppBase *app = NvAppFactory();
    app->setThreadManager(new NvThreadManagerPosix());

    // Check for a requested size
    const int32_t defaultWidth = 1280;
    const int32_t defaultHeight = 720;
    int32_t width = defaultWidth;
    int32_t height = defaultHeight;
    if (!app->getRequestedWindowSize(width, height)) {
        // for safety, reset the height and width if the app returns false in case
        // they did a bad thing and changed the values but returned false
        width = defaultWidth;
        height = defaultHeight;
    }

    app->initialize(NvPlatformInfo(NvPlatformCategory::PLAT_DESKTOP, NvPlatformOS::OS_LINUX), width, height);
    
    app->mainLoop();

    // Shut down the app before shutting down GL
    delete app;

    NvAssetLoaderShutdown();

    NvReleaseSharedFoundation();

    // Exit program
    exit( EXIT_SUCCESS );
}

// Timer and timing functions
NvStopWatch* NvAppBase::createStopWatch() {
    return new NvLinuxStopWatch;
}

bool NvAppBase::showDialog(const char* title, const char *text, bool exitApp) {
    fprintf(stdout, "%s: %s\n", title, text);
    if (exitApp)
        exit(-1);
    return true;
}
bool NvAppBase::writeScreenShot(int32_t, int32_t, const uint8_t*, const std::string&) {
    return false;
}

bool NvAppBase::writeLogFile(const std::string&, bool, const char*, ...) {
    return false;
}

void NvAppBase::forceLinkHack() {
}

void NVPlatformLog(const char* fmt, ...) {
    va_list ap;
  
    va_start(ap, fmt); 
    vfprintf(stderr, fmt, ap);    
    fprintf(stderr, "\n");    
    va_end(ap);
}
