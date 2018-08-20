//----------------------------------------------------------------------------------
// File:        NvAppBase\win/MainWin32.cpp
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
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "NV/NvLogs.h"
#include "NV/NvStopWatch.h"
#include "NV/NvTokenizer.h"
#include "NvAppBase/NvAppBase.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvGamepad/win/NvGamepadXInput.h"
#include "NvThreadWin.h"

// From http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
// Global Variable NvOptimusEnablement (new in Driver Release 302) Starting
// with the Release 302 drivers, application developers can direct the
// Optimus driver at runtime to use the High Performance Graphics to render
// any application--even those applications for which there is no existing
// application profile. They can do this by exporting a global variable named
// NvOptimusEnablement. The Optimus driver looks for the existence and value
// of the export. Only the LSB of the DWORD matters at this time. A value
// of 0x00000001 indicates that rendering should be performed using High
// Performance Graphics. A value of 0x00000000 indicates that this method
// should be ignored.
extern "C" {   _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

static bool sUseStderr = false;

void NVPlatformLog(const char* fmt, ...) {

	const int length = 1024;
	char buffer[length];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf_s(buffer, length - 1, fmt, ap);
	OutputDebugString(buffer);
	OutputDebugString("\n");
	if (sUseStderr)
		fprintf(stderr, "%s\n", buffer);
	va_end(ap);
}

class NvWin32StopWatch : public NvStopWatch
{
public:
	//! Constructor, default
	NvWin32StopWatch() :
		start_time(), end_time(),
		diff_time(0.0), freq_set(false)
	{
		if (!freq_set) {
			// helper variable
			LARGE_INTEGER temp;

			// get the tick frequency from the OS
			QueryPerformanceFrequency((LARGE_INTEGER*)&temp);

			// convert to type in which it is needed
			freq = ((double)temp.QuadPart);

			// rememeber query
			freq_set = true;
		}
	};

	// Destructor
	~NvWin32StopWatch()
	{ };

	//! Start time measurement
	void start() {
		QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
		m_running = true;
	}

	//! Stop time measurement
	void stop() {
		QueryPerformanceCounter((LARGE_INTEGER*)&end_time);
		diff_time = (float)
			(((double)end_time.QuadPart - (double)start_time.QuadPart) / freq);
		m_running = false;
	}

	//! Reset time counters to zero
	void reset()
	{
		diff_time = 0;
		if (m_running)
			QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
	}

	const float getTime() const {
		if (m_running) {
			return getDiffTime();
		}
		else {
			// time difference in seconds
			return diff_time;
		}
	}

private:

	// helper functions

	//! Get difference between start time and current time
	float getDiffTime() const {
		LARGE_INTEGER temp;
		QueryPerformanceCounter((LARGE_INTEGER*)&temp);
		return (float)
			(((double)temp.QuadPart - (double)start_time.QuadPart) / freq);
	}

	// member variables

	//! Start of measurement
	LARGE_INTEGER  start_time;
	//! End of measurement
	LARGE_INTEGER  end_time;

	//! Time difference between the last start and stop
	float  diff_time;

	//! tick frequency
	double  freq;

	//! flag if the frequency has been set
	bool  freq_set;
};

extern void NvInitSharedFoundation(void);
extern void NvReleaseSharedFoundation(void);

// program entry
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Redirect STDERR to the parent console if one exists
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		sUseStderr = true;
		freopen("CONOUT$", "w", stderr);
	}

	NvInitSharedFoundation();

	NvAssetLoaderInit(NULL);

	// add command line arguments
	{
		NvTokenizer cmdtok(lpCmdLine);
		std::string sarg;
		while (cmdtok.getTokenString(sarg))
			NvAppBase::commandLineAppend(sarg);
	}

	NvAppBase *app = NvAppFactory();
	app->setThreadManager(new NvThreadManagerWin());

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

	app->initialize(NvPlatformInfo(NvPlatformCategory::PLAT_DESKTOP, NvPlatformOS::OS_WINDOWS), width, height);

	app->mainLoop();

	// Shut down the app before shutting down GL
	delete app;

	NvAssetLoaderShutdown();

	NvReleaseSharedFoundation();

	// Exit program
	exit(EXIT_SUCCESS);
}

// Timer and timing functions
NvStopWatch* NvAppBase::createStopWatch() {
	return new NvWin32StopWatch;
}

uint64_t NvAppBase::queryPerformanceCounter() {
	uint64_t temp;
	QueryPerformanceCounter((LARGE_INTEGER*)&temp);
	return temp;
}

uint64_t NvAppBase::queryPerformanceCounterFrequency() {
	uint64_t temp;
	QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
	return temp;
}

bool NvAppBase::showDialog(const char* title, const char * content, bool exitApp) {
	if (!isTestMode()) {
		MessageBox(NULL, content, title, MB_OK);
	}
	if (exitApp) {
		appRequestExit();
		getPlatformContext()->requestExit();
	}
	return true;
}

bool NvAppBase::writeScreenShot(int32_t width, int32_t height, const uint8_t* data, const std::string& path) {

	std::string filename = path + ".bmp";
	FILE* fp = NULL;
	errno_t errnum = fopen_s(&fp, filename.c_str(), "wb");
	if (!fp || errnum)
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

	uint32_t filesize = 14 + 40 + width * height * 3;
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

	uint32_t ncolours = 0;
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

	std::string filename = path + ".txt";
	FILE* fp = NULL;
	errno_t errnum = fopen_s(&fp, filename.c_str(), append ? "a" : "w");
	if (!fp || errnum)
		return false;

	va_start(ap, fmt);
	vfprintf_s(fp, fmt, ap);
	fprintf_s(fp, "\n");
	va_end(ap);

	fclose(fp);
	return true;
}


void NvAppBase::forceLinkHack() {
}


