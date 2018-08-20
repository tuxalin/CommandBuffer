//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/ThreadedRenderingGL.cpp
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
#include "ThreadedRenderingGL.h"

#include "Commands.h"

#include "NvAppBase/NvFramerateCounter.h"
#include "NvAppBase/NvInputHandler_CameraFly.h"
#include "NvAssert.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvGLUtils/NvGLSLProgram.h"
#include "NvGLUtils/NvImageGL.h"
#include "NvGLUtils/NvMeshExtGL.h"
#include "NvGLUtils/NvModelExtGL.h"
#include "NvGLUtils/NvShapesGL.h"
#include "NvModel/NvModelExt.h"
#include "NvModel/NvModelSubMesh.h"
#include "NvUI/NvTweakBar.h"
#include <NvUI/NvBitFont.h>

#include <stdint.h>

#include <ctime>

int commandLogFunction(const char* fmt, ...)
{
	va_list args;
	char buffer[BUFSIZ];

	va_start(args, fmt);
	vsnprintf(buffer, sizeof buffer, fmt, args);
	va_end(args);

	NVPlatformLog(buffer);
	return 0;
}

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof( (a)[0] ))
#define NV_UNUSED( variable ) ( void )( variable )

#define USE_STATIC_THREAD_WORK 1
#define SIMPLE_DEMO 1
#define STRESS_TEST 0

// Currently the number of instances rendered of each model
#define MAX_INSTANCE_COUNT 100
#define INSTANCE_COUNT 100

#ifdef ANDROID
#define MAX_SCHOOL_COUNT 2000
#else
#define MAX_SCHOOL_COUNT 5000
#endif

#if STRESS_TEST
#define SCHOOL_COUNT MAX_SCHOOL_COUNT
#else 
#define SCHOOL_COUNT 50 // one of each fish model
#endif

// Static section for thread data structures

uint32_t s_threadMask = 0;

extern uint32_t neighborOffset;
extern uint32_t neighborSkip;

// Global function to pass to each animation worker thread which will extract the
// ThreadData from the argument passed in and use that to invoke the actual
// animate function on the application instance
#ifdef _WIN32
DWORD WINAPI animateJobFunctionThunk(VOID *arg)
#else
void* animateJobFunctionThunk(void *arg)
#endif
{
	ThreadedRenderingGL::ThreadData* data = (ThreadedRenderingGL::ThreadData*)arg;
	data->m_app->animateJobFunction(data->m_index);

	return 0;
}

#ifdef _WIN32
DWORD WINAPI helperJobFunctionThunk(VOID *arg)
#else
void* helperJobFunctionThunk(void *arg)
#endif
{
	auto* mainApp = (ThreadedRenderingGL*)arg;
	mainApp->helperJobFunction();

	return 0;
}

bool ThreadedRenderingGL::waitForWork(ThreadData& me, int threadIndex)
{
	// We use the m_frameStartCV condition variable to wake threads up 
	// and notify them that there is work available to be done.  The 
	// condition variable is protected by the m_frameStartLock mutex.
	m_frameStartLock->lockMutex();
	{
		// Sleep the thread until there is work to be done
		if (m_frameID == me.m_frameID) {
			m_frameStartCV->waitConditionVariable(
				m_frameStartLock);
		}

		me.m_frameID = m_frameID;

		// See if we were told to stop running while we
		// were sleeping
		if (!m_running)
		{
			// Yes, so release the mutex and pop out of our loop to exit
			m_frameStartLock->unlockMutex();
			return true;
		}
		// m_activeThreads controls how many threads will actually run in 
		// a frame.  
		if (threadIndex >= 0 && threadIndex >= (int)m_activeAnimationThreads)
		{
			// Our index is too high, then we need to go back to 
			// sleep for this frame by returning to the top of the
			// loop and waiting on the next frame to start.  Don't
			// forget to unlock the Mutex, though, as we'll lock
			// it at the start of the frame.
			m_frameStartLock->unlockMutex();
			return true;
		}
	}
	// We have work to do, but it's independent of other threads, so we 
	// don't need the frame start mutex anymore.
	m_frameStartLock->unlockMutex();
	return false;
}

void ThreadedRenderingGL::signalWorkComplete(uint32_t workCountDone)
{
	// Increment our m_doneCount by the number of schools that we've 
	// updated, making sure to synchronize access of the value with
	// our other worker threads.
	m_doneCountLock->lockMutex();
	{
		m_doneCount += workCountDone;
		// Notify the main thread that more schools are finished 
		// so that it can decide if it's time to render them
		m_doneCountCV->signalConditionVariable();
	}
	m_doneCountLock->unlockMutex();
}

void ThreadedRenderingGL::animateJobFunction(uint32_t threadIndex)
{
	NvThreadManager* threadManager = getThreadManagerInstance();
	NV_ASSERT(nullptr != threadManager);
	ThreadData& me = m_threads[threadIndex];
	me.m_frameID = 0;

	// Our m_running member gives us a mechanism to signal all worker threads
	// to quit when we need to shut them all down
	while (m_running)
	{
		if (waitForWork(me, threadIndex))
			continue;

		uint32_t schoolsDone = 0;

		{
			CPU_TIMER_SCOPE(CPU_TIMER_THREAD_BASE_TOTAL + threadIndex);
			// Quick debugging helper so that we can quickly see which threads 
			// activated this frame.
			s_threadMask |= 1 << threadIndex;

			// There are two ways that we can dole out work to be done by the threads:
			//    1) Each thread is given (number of schools / number of threads) to
			//       process and simply loops over its range of schools, updating
			//       the shared count of updated schools once, after all have been updated
			//    2) Each thread pulls a single school off of the work queue and
			//       updates it, then grabs the next one, repeating this process 
			//       until the "Needs Update" queue is empty, updating the shared
			//       count of updated schools after each one

#ifdef USE_STATIC_THREAD_WORK
			// In the first method, there is less contention for the frame start mutex
			// and there is no need for the "Needs Update" queue at all.  Also, there
			// are less waits between updating each school.  However, this results in 
			// less efficient thread utilization if the number of fish in each school is
			// different (i.e. each school has a different amount of work to do during
			// its update).  In our fixed school size case, this is not an issue, so 
			// thread utilization should not be significantly impacted.

			schoolsDone = me.m_schoolCount;
			if (!m_animPaused || m_bForceSchoolUpdate)
			{
				ThreadData& thread = m_threads[threadIndex];
				uint32_t schoolMax = me.m_baseSchoolIndex + schoolsDone;
				for (uint32_t i = me.m_baseSchoolIndex; i < schoolMax; i++)
				{
					// Updating in GL is just animating.  We cannot update our instance data in another thread.
					CPU_TIMER_SCOPE(CPU_TIMER_THREAD_BASE_ANIMATE + threadIndex);
					m_schools[i]->Animate(getClampedFrameTime(), &m_schoolStateMgr, m_avoidance);
				}
			}

#else
			// In the second method, we need to synchronize access to the "Needs 
			// Update" queue that contains all of the schools that still need
			// to be updated.  This can result in contention for that resource,
			// thus more waiting, but if the workload for each school varies from
			// that of other schools, it can result in higher thread utilization.
			m_needsUpdateQueueLock->lockMutex();
			while (m_running && !m_needsUpdateQueue.empty())
			{
				job = m_needsUpdateQueue.front();
				m_needsUpdateQueue.pop_front();
				m_needsUpdateQueueLock->unlockMutex();

				//ANIMATE
#if FISH_DEBUG
				LOGI("Thread %d animating school %d...\n", threadIndex, job.index);
#endif

				if (!m_animPaused || m_bForceSchoolUpdate)
				{
					CPU_TIMER_SCOPE(CPU_TIMER_THREAD_BASE_ANIMATE + threadIndex);
					job.school->Animate(getClampedFrameTime(), &m_schoolStateMgr, m_avoidance);
				}

				schoolsDone++;

#if FISH_DEBUG
				LOGI("Thread %d animated school %d.\n", threadIndex, job.index);
#endif

				m_needsUpdateQueueLock->lockMutex();
			}
			m_needsUpdateQueueLock->unlockMutex();
#endif
			signalWorkComplete(schoolsDone);
		}
	}

	LOGI("Thread %d Exit.\n", threadIndex);
}

void ThreadedRenderingGL::helperJobFunction()
{
	const uint32_t threadIndex = m_activeAnimationThreads;
	ThreadData& me = m_threads[threadIndex];
	me.m_frameID = 0;

	while (m_running)
	{
		if (waitForWork(me, -1))
			continue;

		CPU_TIMER_SCOPE(CPU_TIMER_THREAD_BASE_TOTAL + threadIndex);
		s_threadMask |= 1 << threadIndex;

		// Wait on our per-frame fence before calling into any of our schools
		// to update them
		if (nullptr != m_fences)
		{
			const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 1);
			auto* cmd = m_geometryCommands.addCommand<cmds::WaitFenceCommand>(key);
			cmd->fences = m_fences;
			cmd->currentFenceIndex = &m_currentFenceIndex;
			cmd->numDrawAheadFrames = m_numDrawAheadFrames;
			CB_DEBUG_COMMAND_TAG_MSG(cmd, "Wait on fences");
		}

#if !SIMPLE_DEMO
		// Update the camera position if we are following a school
		if (m_uiCameraFollow)
		{
			if (m_uiSchoolInfoId < m_activeSchools)
			{
				// Get the centroid of the school we're following
				School* pSchool = m_schools[m_uiSchoolInfoId];
				nv::vec3f camPos = pSchool->GetCentroid() - (m_pInputHandler->getLookVector() * pSchool->GetRadius() * 4);
				if (camPos.y < 0.01f)
				{
					camPos.y = 0.01f;
				}
				m_pInputHandler->setPosition(camPos);
			}
			else
			{
				m_uiCameraFollow = false;
				m_bUIDirty = true;
				updateUI();
			}
		}
#endif
		updateSchoolTankSizes();

		{
			const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 2);
			auto* cmd = m_geometryCommands.addCommand<BeginFrameCommand>(key);
			cmd->activeSchools = m_activeSchools;
			cmd->schoolStateMgr = &m_schoolStateMgr;

			// Get the current view matrix (according to user input through mouse,
			// gamepad, etc.)
			cmd->projUBO_Id = m_projUBO_Id;
			cmd->projUBO_Data = m_projUBO_Data;
			cmd->projUBO_Data.m_viewMatrix = m_pInputHandler->getViewMatrix();
			cmd->projUBO_Data.m_inverseViewMatrix = m_pInputHandler->getCameraMatrix();
			cmd->lightingUBO_Id = m_lightingUBO_Id;
			cmd->lightingUBO_Data = m_lightingUBO_Data;
			cmd->lightingUBO_Data.m_causticOffset = m_currentTime * m_causticSpeed;
			cmd->lightingUBO_Data.m_causticTiling = m_causticTiling;
			CB_DEBUG_COMMAND_TAG(cmd);

			if ((nullptr != m_pVBOPool) && (!m_animPaused || m_bForceSchoolUpdate))
			{
				// For the pooled VBO policy, we have to surround any schools' updates with
				// a BeginUpdate/EndUpdate for the master VBO pool.  This ensures that the
				// schools' sub-ranges are mapped and valid when the school's update is called.
				// We also need to iterate over all of the schools twice, once to update and once
				// to render, as we must Unmap the large, shared VBO before we can do any
				// rendering with it.
				auto* vboCmd = m_geometryCommands.appendCommand<cmds::VboPoolUpateCommand>(cmd);
				vboCmd->vboPool = m_pVBOPool;
				vboCmd->begin = true;
				CB_DEBUG_COMMAND_TAG(vboCmd);
			}
		}

		// note could also use depth for sorting or even chaining
		{
			const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eSkybox, 0);
			auto* cmd = m_geometryCommands.addCommand<cmds::DrawSkyboxCommand>(key);
			cmd->projUBO_Id = m_projUBO_Id;
			cmd->projUBO_Location = m_projUBO_Location;
			cmd->shader = m_shader_Skybox;
			cmd->gradientTex = m_skyboxGradientTex;
			cmd->sandTex = m_skyboxSandTex;
			CB_DEBUG_COMMAND_SET_MSG(cmd, "Draw Skybox");
		}
		{
			const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eSkybox, 1);
			auto* cmd = m_geometryCommands.addCommand<cmds::DrawGroundCommand>(key);
			cmd->projUBO_Id = m_projUBO_Id;
			cmd->projUBO_Location = m_projUBO_Location;
			cmd->lightingUBO_Id = m_lightingUBO_Id;
			cmd->lightingUBO_Location = m_lightingUBO_Location;
			cmd->shader = m_shader_GroundPlane;
			cmd->caustic1Tex = m_caustic1Tex;
			cmd->caustic2Tex = m_caustic2Tex;
			cmd->skyboxSandTex = m_skyboxSandTex;
			CB_DEBUG_COMMAND_SET_MSG(cmd, "Draw Ground");
		}

		signalWorkComplete(1);
	}
}

class ThreadedRenderingModelLoader : public Nv::NvModelFileLoader
{
public:
	ThreadedRenderingModelLoader() {}
	virtual ~ThreadedRenderingModelLoader() {}
	virtual char* LoadDataFromFile(const char* fileName)
	{
		int32_t length;
		return NvAssetLoaderRead(fileName, length);
	}

	virtual void ReleaseData(char* pData)
	{
		NvAssetLoaderFree(pData);
	}
};

//-----------------------------------------------------------------------------
// PUBLIC METHODS, CTOR AND DTOR
nv::vec3f ThreadedRenderingGL::ms_tankMin(-30.0f, 5.0f, -30.0f);
nv::vec3f ThreadedRenderingGL::ms_tankMax(30.0f, 25.0f, 30.0f);

ThreadedRenderingGL::ThreadedRenderingGL() :
	NvSampleAppGL(),
	m_bFollowingSchool(false),
	m_frameStartLock(nullptr),
	m_frameStartCV(nullptr),
	m_needsUpdateQueueLock(nullptr),
	m_doneCount(0),
	m_doneCountLock(nullptr),
	m_doneCountCV(nullptr),
	m_activeAnimationThreads(0),
	m_bEnableFences(true),
	m_fences(nullptr),
	m_numDrawAheadFrames(3), // Must be >0
	m_currentFenceIndex(0),
	m_running(true),
	m_shader_GroundPlane(nullptr),
	m_shader_Skybox(nullptr),
	m_shader_Fish(nullptr),
	m_projUBO_Id(0),
	m_projUBO_Location(0),
	m_lightingUBO_Id(0),
	m_lightingUBO_Location(0),
	m_startingCameraPitchYaw(0.0f, 0.0f),
	m_maxSchools(MAX_SCHOOL_COUNT),
	m_schoolStateMgr(m_maxSchools),
	m_ESVBOPolicy(Nv::VBO_POOLED_PERSISTENT),
	m_currentVBOPolicy(Nv::VBO_INVALID),
	m_pVBOPool(nullptr),
	m_activeSchools(0),
	m_skyboxSandTex(0),
	m_skyboxGradientTex(0),
	m_caustic1Tex(0),
	m_caustic2Tex(0),
	m_causticTiling(0.1f),
	m_causticSpeed(0.3f),
	m_uiResetMode(RESET_RANDOM),
	m_pFishFireworksVar(nullptr),
	m_pFishplosionVar(nullptr),
	m_animPaused(false),
	m_avoidance(true),
	m_currentTime(0.0f),
	m_bForceSchoolUpdate(false),
	m_bUIDirty(true),
	m_uiSchoolCount(0),
	m_uiFishCount(0),
	m_uiTankSize(30),
	m_bTankSizeChanged(false),
	m_uiThreadCount(0),
	m_uiInstanceCount(INSTANCE_COUNT),
	m_uiBatchSize(m_uiInstanceCount),
	m_pBatchSlider(nullptr),
	m_pBatchVar(nullptr),
	m_uiRenderingTechnique(TECHNIQUE_GLES_POOLED),
	m_uiSchoolInfoId(0),
	m_uiCameraFollow(false),
	m_uiSchoolDisplayModelIndex(0),
	m_uiSchoolMaxSpeed(0.0f),
	m_uiSchoolInertia(0.0f),
	m_uiSchoolNeighborDistance(0.0f),
	m_uiSchoolGoalScale(0.0f),
	m_uiSchoolAlignmentScale(0.0f),
	m_uiSchoolRepulsionScale(0.0f),
	m_uiSchoolCohesionScale(0.0f),
	m_uiSchoolAvoidanceScale(0.0f),
	m_simpleTimingStats(nullptr),
	m_simpleStatsBox(nullptr),
	m_fullTimingStats(nullptr),
	m_fullStatsBox(nullptr),
	m_bDisplayLogos(true),
	m_logoNVIDIA(nullptr),
	m_logoGLES(nullptr),
	m_logoGL(nullptr),
	m_drawCallCount(0),
	m_statsCountdown(STATS_FRAMES),
	m_statsMode(STATS_SIMPLE),
	m_pStatsModeVar(nullptr),
	m_meanFPS(0.0f),
	m_meanCPUMainCmd(0.0f),
	m_meanCPUMainWait(0.0f),
	m_meanCPUMainCopyVBO(0.0f),
	m_meanGPUFrameMS(0.0f),
	m_frameID(0)
{
	cb::DrawKey::sanityChecks();

#if STRESS_TEST
	// Full complexity for the stress test
	neighborSkip = 5;
#endif
	ms_tankMax.x = ms_tankMax.z = (float)m_uiTankSize;
	ms_tankMin.x = ms_tankMin.z = -ms_tankMax.x;
	m_startingCameraPosition = (ms_tankMin + ms_tankMax) * 0.5f;
	m_startingCameraPosition.z += 40.0f;

	m_pInputHandler = new NvInputHandler_CameraFly();
	SetInputHandler(m_pInputHandler);
	m_pInputHandler->setPosition(m_startingCameraPosition);
	m_pInputHandler->setYaw(m_startingCameraPitchYaw.y);
	m_pInputHandler->setPitch(m_startingCameraPitchYaw.x);
#ifdef ANDROID
	m_pInputHandler->setMouseRotationSpeed(0.001f);  // Touch screen should be a bit less sensitive than mouse
#else
	m_pInputHandler->setMouseRotationSpeed(0.002f);
#endif
	m_pInputHandler->setMouseTranslationSpeed(0.03f);
	m_pInputHandler->setGamepadRotationSpeed(1.0f);
	m_pInputHandler->setGamepadTranslationSpeed(4.0f);
	m_pInputHandler->setKeyboardRotationSpeed(1.0f);
	m_pInputHandler->setKeyboardTranslationSpeed(4.0f);

	for (uint32_t i = 0; i < MODEL_COUNT; i++)
	{
		m_models[i] = nullptr;
	}

	for (uint32_t i = 0; i < MAX_THREAD_COUNT; i++)
	{
		m_threads[i].m_thread = NULL;
		m_threads[i].m_app = this;
		m_threads[i].m_index = i;
	}

	initializeSchoolDescriptions(50);

	// We have the option of not synchronizing access to our instancing buffers.  This will
	// result in higher performance with the risk of writing over data that is currently 
	// being used to render with.
	if (m_bEnableFences)
	{
		m_fences = new GLsync[m_numDrawAheadFrames];
		for (uint32_t i = 0; i < m_numDrawAheadFrames; ++i) { m_fences[i] = nullptr; }
	}

	// Required in all subclasses to avoid silent link issues
	forceLinkHack();
}

ThreadedRenderingGL::~ThreadedRenderingGL()
{
	LOGI("ThreadedRenderingGL: destroyed\n");
	if (nullptr != m_fences)
	{
		for (uint32_t i = 0; i < m_numDrawAheadFrames; ++i)
		{
			if (nullptr != m_fences[i])
			{
				glDeleteSync(m_fences[i]);
				m_fences[i] = nullptr;
			}
		}
	}
	cleanThreads();
	cleanRendering();
}

// Inherited methods
void ThreadedRenderingGL::configurationCallback(NvGLConfiguration& config)
{
	config.depthBits = 24;
	config.stencilBits = 0;
	config.apiVer = NvGLAPIVersionGL4_4();
}

void ThreadedRenderingGL::initRendering(void)
{
	NV_APP_BASE_SHARED_INIT();

	// We require at least ES3 for our most basic functionality
	requireMinAPIVersion(NvGLAPIVersionES3());

	// We need to provide the mesh rendering system in NvGLUtils with instanced rendering
	// methods, as it does not currently support features newer than GLES2.
	Nv::NvGLInstancingSupport::glDrawElementsInstancedInternal =
		(Nv::NvGLInstancingSupport::PFNDrawElementsInstanced)getGLContext()->getGLProcAddress("glDrawElementsInstanced");
	Nv::NvGLInstancingSupport::glVertexAttribDivisorInternal =
		(Nv::NvGLInstancingSupport::PFNVertexAttribDivisor)getGLContext()->getGLProcAddress("glVertexAttribDivisor");

	// Process command line options
	{
		const std::vector<std::string>& cmd = getCommandLine();
		std::vector<std::string>::const_iterator iter = cmd.begin();
		for (std::vector<std::string>::const_iterator iter = cmd.begin(); iter != cmd.end(); ++iter)
		{
			//			if (*iter == "-idle")
		}
	}

	// Disable v-sync
	getAppContext()->setSwapInterval(0);

	for (int32_t i = 0; i < CPU_TIMER_COUNT; ++i)
	{
		m_CPUTimers[i].init();
	}

	m_GPUTimer.init();

	mFramerate->setReportFrames(20);

	NvAssetLoaderAddSearchPath("es3aep-kepler/ThreadedRenderingGL");
	ThreadedRenderingModelLoader loader;
	Nv::NvModelExt::SetFileLoader(&loader);

	// Load all shaders
	m_shader_GroundPlane = NvGLSLProgram::createFromFiles("src_shaders/groundplane_VS.glsl", "src_shaders/groundplane_FS.glsl");
	m_shader_Skybox = NvGLSLProgram::createFromFiles("src_shaders/skyboxcolor_VS.glsl", "src_shaders/skyboxcolor_FS.glsl");
	m_shader_Fish = NvGLSLProgram::createFromFiles("src_shaders/staticfish_VS.glsl", "src_shaders/staticfish_FS.glsl");

	if ((nullptr == m_shader_GroundPlane) ||
		(nullptr == m_shader_Skybox) ||
		(nullptr == m_shader_Fish))
	{
		showDialog("Fatal: Cannot Find Assets", "The shader assets cannot be loaded.\n"
			"Please ensure that the assets directory for the sample is in place\n"
			"and has not been moved.  Exiting.", true);
		return;
	}

	for (uint32_t i = 0; i < MODEL_COUNT; ++i)
	{
		Nv::NvModelExt* pModel =
			Nv::NvModelExt::CreateFromPreprocessed(ms_modelInfo[i].m_filename);
		if (nullptr != pModel)
		{

			Nv::NvModelExtGL* pModelGL = m_models[i] =
				Nv::NvModelExtGL::Create(pModel);
			if (nullptr == pModelGL)
			{
				continue;
			}
			pModelGL->SetDiffuseTextureLocation(0);
			pModelGL->UpdateBoneTransforms();

			// Either Assimp or our art is completely broken, so bone
			// transforms are garbage. 
			// Use the static shader and ignore skin until we can untangle it
			ModelDesc& desc = ms_modelInfo[i];
			desc.m_center = Rotate(desc.m_fixupXfm, pModelGL->GetCenter());
			desc.m_halfExtents = Rotate(desc.m_fixupXfm, pModelGL->GetMaxExt() - ms_modelInfo[i].m_center);
		}
	}

	initThreads();
#if CB_DEBUG_COMMANDS_PRINT
	m_geometryCommands.setLogFunction(&commandLogFunction);
#endif

	const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 0);
	auto* cmd = m_geometryCommands.addCommand<InitializeCommand>(key);
	cmd->threadedRenderingGL = this;
	CB_DEBUG_COMMAND_TAG_MSG(cmd, "Initialize Rendering");
}

void ThreadedRenderingGL::initGL()
{
	// Create shared uniform buffers
	m_projUBO_Location = 0; // Fixed in shaders
	glGenBuffers(1, &m_projUBO_Id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_projUBO_Id);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjUBO), &m_projUBO_Data, GL_STREAM_DRAW);

	m_lightingUBO_Location = 1; // Fixed in shaders
	glGenBuffers(1, &m_lightingUBO_Id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightingUBO_Id);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingUBO), &m_lightingUBO_Data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Load the skybox
	m_skyboxSandTex = NvImageGL::UploadTextureFromDDSFile("textures/sand.dds");
	m_skyboxGradientTex = NvImageGL::UploadTextureFromDDSFile("textures/Gradient.dds");
	m_caustic1Tex = NvImageGL::UploadTextureFromDDSFile("textures/caustic1.dds");
	m_caustic2Tex = NvImageGL::UploadTextureFromDDSFile("textures/caustic2.dds");

	// Assign some values which apply to the entire scene and update once per frame.
	m_lightingUBO_Data.m_lightPosition = nv::vec4f(1.0f, 1.0f, 1.0f, 0.0f);
	m_lightingUBO_Data.m_lightAmbient = nv::vec4f(0.4f, 0.4f, 0.4f, 1.0f);
	m_lightingUBO_Data.m_lightDiffuse = nv::vec4f(0.7f, 0.7f, 0.7f, 1.0f);
	m_lightingUBO_Data.m_causticOffset = m_currentTime * m_causticSpeed;
	m_lightingUBO_Data.m_causticTiling = m_causticTiling;

	// This must be done before SetNumSchools() is called so that the
	// multidraw model gets all of the school info for new schools
	{
		setVBOPolicy(m_ESVBOPolicy);
	}

	setNumSchools(SCHOOL_COUNT);

	resetFish(false);
}

uint32_t ThreadedRenderingGL::setNumSchools(uint32_t numSchools)
{
	if (numSchools > m_maxSchools)
	{
		numSchools = m_maxSchools;
	}
	if (nullptr != m_pVBOPool)
	{
		m_pVBOPool->SetNumSubRanges(numSchools);
		m_bForceSchoolUpdate = numSchools != m_activeSchools;
	}

	if (m_schoolDescs.size() < numSchools)
	{
		initializeSchoolDescriptions(numSchools);
	}

	nv::vec3f location(0.0f, 0.0f, 0.0f);

	if (m_schools.size() < numSchools)
	{
		// We need to increase the size of our array of schools and initialize the new ones
		uint32_t schoolIndex = m_schools.size();
		m_schools.resize(numSchools);

		int32_t newSchools = numSchools - schoolIndex;

		if (newSchools > 0)
		{
			for (; schoolIndex < m_schools.size(); ++schoolIndex)
			{
				SchoolDesc& desc = m_schoolDescs[schoolIndex];
				School* pSchool = new School(desc.m_flocking);
				if (m_uiResetMode == RESET_RANDOM)
				{
					nv::vec3f tankSize = ms_tankMax - ms_tankMin;
					location = nv::vec3f((float)rand() / (float)RAND_MAX * tankSize.x,
						(float)rand() / (float)RAND_MAX * tankSize.y,
						(float)rand() / (float)RAND_MAX * tankSize.z);
					location += ms_tankMin;
				}
				// Account for scaling in the transforms and extents that we are providing
				// to the school
				nv::matrix4f finalTransform;
				finalTransform.set_scale(desc.m_scale);
				finalTransform *= ms_modelInfo[desc.m_modelId].getCenteringTransform();

				if (!pSchool->Initialize(
					schoolIndex,
					m_models[desc.m_modelId],
					desc.m_modelId,
					ms_modelInfo[desc.m_modelId].m_tailStartZ * desc.m_scale,
					finalTransform,
					ms_modelInfo[desc.m_modelId].m_halfExtents * desc.m_scale,
					desc.m_numFish,
					MAX_INSTANCE_COUNT,
					location, 2, 4, m_currentVBOPolicy, m_numDrawAheadFrames, m_pVBOPool))
				{
					return 0;
				}
				m_schools[schoolIndex] = pSchool;
			}
		}
	}
	m_activeSchools = numSchools;

	// Update our readout of total number of active fish
	m_uiFishCount = m_activeSchools * m_uiInstanceCount;
	updateUI();
	return m_activeSchools;
}

void ThreadedRenderingGL::initializeSchoolDescriptions(uint32_t numSchools)
{
	uint32_t schoolIndex = m_schoolDescs.size();
	m_schoolDescs.resize(numSchools);
	for (; schoolIndex < numSchools; ++schoolIndex)
	{
		SchoolDesc& desc = m_schoolDescs[schoolIndex];
		desc = ms_schoolInfo[schoolIndex % MODEL_COUNT];
		desc.m_numFish = INSTANCE_COUNT;
	}
}

void ThreadedRenderingGL::setVBOPolicy(Nv::VBOPolicy policy)
{
	if (m_currentVBOPolicy == policy)
	{
		// Policy already set as requested
		return;
	}

	// If the previous policy had a pool, stash it off so that we
	// can free it after all of the dependent VBOs have been released
	Nv::NvSharedVBOGLPool* pOldPool = m_pVBOPool;

	if ((policy == Nv::VBO_POOLED) || (policy == Nv::VBO_POOLED_PERSISTENT))
	{
		m_pVBOPool = new Nv::NvSharedVBOGL_Pooled();
		m_pVBOPool->Initialize(School::GetInstanceDataStride() * MAX_INSTANCE_COUNT,
			m_activeSchools,
			m_numDrawAheadFrames,
			(policy == Nv::VBO_POOLED_PERSISTENT));
	}
	else
	{
		m_pVBOPool = nullptr;
	}
	m_currentVBOPolicy = policy;

	// Stash this policy as the new policy for whichever mode we're currently in
	m_ESVBOPolicy = policy;


	// Update the VBO policy for all initialized schools, even if they are not currently active
	SchoolSet::iterator schoolIt = m_schools.begin();
	SchoolSet::iterator schoolEnd = m_schools.end();
	for (; schoolIt != schoolEnd; ++schoolIt)
	{
		(*schoolIt)->SetVBOPolicy(policy, m_pVBOPool, m_numDrawAheadFrames);
	}

	if (nullptr != pOldPool)
	{
		pOldPool->Finalize();
		delete pOldPool;
	}

	// Since all of their existing VBOs have been destroyed, each
	// school will need to update again to fill in its instance data
	m_bForceSchoolUpdate = true;
}

void ThreadedRenderingGL::updateSchoolTankSizes()
{
	if (m_bTankSizeChanged)
	{
		SchoolSet::iterator schoolIt = m_schools.begin();
		SchoolSet::iterator schoolEnd = m_schools.end();

		for (; schoolIt != schoolEnd; ++schoolIt)
		{
			School* pSchool = (*schoolIt);
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_spawnZoneMin = ms_tankMin;
			params.m_spawnZoneMax = ms_tankMax;
			pSchool->SetFlockingParams(params);

			// Poke them and force them to start heading to a new spot
			// within the new constraints of the "tank"
			pSchool->FindNewGoal();
		}
		m_bTankSizeChanged = false;
	}
}

uint32_t ThreadedRenderingGL::setAnimationThreadNum(uint32_t numThreads)
{
	if (MAX_ANIMATION_THREAD_COUNT < numThreads)
	{
		numThreads = MAX_ANIMATION_THREAD_COUNT;
	}
	m_activeAnimationThreads = numThreads;

#if FISH_DEBUG
	LOGI("Active Animation Thread Count = %d", m_activeAnimThreads);
#endif

	updateUI();

	return m_activeAnimationThreads;
}


// The sample has a dropdown menu in the TweakBar that allows the user to
// choose the API to use to render with, along with the policy used for
// managing VBO objects.
// These are the options in that menu 
static NvTweakEnum<uint32_t> RENDER_TECHNIQUES[] =
{
	{ "(GLES) VBO SubRanges", ThreadedRenderingGL::TECHNIQUE_GLES_SUBRANGE },
	{ "(GLES) Persistent VBO SubRanges", ThreadedRenderingGL::TECHNIQUE_GLES_SUBRANGE_PERSISTENT },
	{ "(GLES) Orphaned VBOs", ThreadedRenderingGL::TECHNIQUE_GLES_ORPHANED },
	{ "(GLES) VBO Pool", ThreadedRenderingGL::TECHNIQUE_GLES_POOLED },
	{ "(GLES) Persistent VBO Pool", ThreadedRenderingGL::TECHNIQUE_GLES_POOLED_PERSISTENT },
	{ "(AZDO) VBO Pool", ThreadedRenderingGL::TECHNIQUE_GLAZDO_POOLED },
	{ "(AZDO) Persistent VBO Pool", ThreadedRenderingGL::TECHNIQUE_GLAZDO_POOLED_PERSISTENT }
};


// The sample has a dropdown menu in the TweakBar that allows the user to
// reset the schools of fish and the camera to their starting position and
// state. These are the options in that menu 
static NvTweakEnum<uint32_t> RESET_OPTIONS[] =
{
	{ "Fish Fireworks", ThreadedRenderingGL::RESET_RANDOM },
	{ "Fishsplosion", ThreadedRenderingGL::RESET_ORIGIN }
};

// The sample has a dropdown menu in the TweakBar that allows the user to
// select a mode for displaying on-screen stats
// These are the options in that menu (for more details, please see the
// sample documentation)
static NvTweakEnum<uint32_t> STATSMODE_OPTIONS[] =
{
	{ "None", ThreadedRenderingGL::STATS_NONE},
	{ "Simple", ThreadedRenderingGL::STATS_SIMPLE },
	{ "Full", ThreadedRenderingGL::STATS_FULL }
};

void ThreadedRenderingGL::initUI(void)
{
	if (mTweakBar)
	{
		mTweakBar->setCompactLayout(true);
		mTweakBar->SetNumRows(22);

		NvTweakVarBase* var;
		mTweakBar->addLabel("Fish Settings", true);
		mTweakBar->addValue("Number of Schools", m_uiSchoolCount, MAX_ANIMATION_THREAD_COUNT, m_maxSchools, 10, UIACTION_SCHOOLCOUNT);
		mTweakBar->addValue("Fish per School", m_uiInstanceCount, 1, MAX_INSTANCE_COUNT, 1, UIACTION_INSTCOUNT);
		mTweakBar->addValue("Max Roam Distance", m_uiTankSize, 1, 60, 1, UIACTION_TANKSIZE);

		mTweakBar->addPadding();
		mTweakBar->addLabel("Rendering Settings", true);
		mTweakBar->addValue("Number of Worker Threads", m_uiThreadCount, 1, MAX_THREAD_COUNT, 1, UIACTION_ANIMTHREADCOUNT);
		mTweakBar->addValue("Batch Size (Fish per Draw Call)", m_uiBatchSize, 1, MAX_INSTANCE_COUNT, 1, UIACTION_BATCHSIZE,
			&m_pBatchSlider, &m_pBatchVar);
		mTweakBar->addMenu("Mode", m_uiRenderingTechnique, &(RENDER_TECHNIQUES[0]),
			TECHNIQUE_GLAZDO_POOLED,
			UIACTION_RENDERINGTECHNIQUE);

		mTweakBar->addPadding();
		mTweakBar->addLabel("Animation Settings", true);
		var = mTweakBar->addValue("Pause Animation", m_animPaused, UIACTION_ANIMPAUSED);
		addTweakKeyBind(var, NvKey::K_P);

		var = mTweakBar->addValue("Use Avoidance", m_avoidance);
		addTweakKeyBind(var, NvKey::K_R);

		mTweakBar->addValue("Flocking Complexity", neighborSkip, 1, 5);

		mTweakBar->addLabel("Reset Schools", true);
		m_pFishFireworksVar = mTweakBar->addButton("Fish Fireworks", UIACTION_RESET_FISHFIREWORKS);
		addTweakButtonBind(m_pFishFireworksVar, NvGamepad::BUTTON_Y);
		addTweakKeyBind(m_pFishFireworksVar, NvKey::K_F);

		m_pFishplosionVar = mTweakBar->addButton("Fishsplosion", UIACTION_RESET_FISHPLOSION);
		addTweakKeyBind(m_pFishplosionVar, NvKey::K_X);

		mTweakBar->addPadding();
		// mTweakBar->addLabel("On-screen Stats", true);
		m_pStatsModeVar = mTweakBar->addButton("Cycle Stats", UIACTION_STATSTOGGLE);
		addTweakKeyBind(m_pStatsModeVar, NvKey::K_C, NvKey::K_C);
		addTweakButtonBind(m_pStatsModeVar, NvGamepad::BUTTON_LEFT_SHOULDER, NvGamepad::BUTTON_LEFT_SHOULDER);

		m_uiSchoolInfoId = 0;
		SchoolDesc& desc = m_schoolDescs[m_uiSchoolInfoId];
		m_uiSchoolDisplayModelIndex = desc.m_modelId;

#if !SIMPLE_DEMO
		// Choose the currently selected/editable school
		mTweakBar->addValue("Edit School", m_uiSchoolInfoId, 0, m_maxSchools - 1, 1, UIACTION_SCHOOLINFOID);

		mTweakBar->subgroupStart();

		// Detailed info display for a single school
		mTweakBar->SetNumRows(28);
		mTweakBar->addValue("Follow School", m_uiCameraFollow, UIACTION_CAMERAFOLLOWSCHOOL);

		// Add name of the model associated with the current school
		mTweakBar->addValue("Model", m_uiSchoolDisplayModelIndex, 0, MODEL_COUNT - 1, 1, UIACTION_SCHOOLMODELINDEX);
		std::vector<NvTweakEnum<uint32_t> > modelMenuItems;
		for (uint32_t modelIndex = 0; modelIndex < MODEL_COUNT; ++modelIndex)
		{
			NvTweakEnum<uint32_t> tweak;
			tweak.m_name = ms_modelInfo[modelIndex].m_name;
			tweak.m_value = modelIndex;
			modelMenuItems.push_back(tweak);
		}
		if (m_schoolDescs.empty())
		{
			InitializeSchoolDescriptions(1);
		}
		mTweakBar->addMenu("", m_uiSchoolDisplayModelIndex, &(modelMenuItems[0]), modelMenuItems.size(), UIACTION_SCHOOLMODELINDEX);
		m_uiSchoolMaxSpeed = desc.m_flocking.m_maxSpeed;
		mTweakBar->addValue("Speed", m_uiSchoolMaxSpeed, 0.25f, 10.0f, 0.25f, UIACTION_SCHOOLMAXSPEED);
		m_uiSchoolInertia = desc.m_flocking.m_inertia;
		mTweakBar->addValue("Inertia", m_uiSchoolInertia, 1.0f, 40.0f, 1.0f, UIACTION_SCHOOLINERTIA);
		m_uiSchoolNeighborDistance = desc.m_flocking.m_neighborDistance;
		mTweakBar->addValue("Neighbor Distance", m_uiSchoolNeighborDistance, 0.01f, 8.0f, 0.01f, UIACTION_SCHOOLNEIGHBORDIST);
		m_uiSchoolGoalScale = desc.m_flocking.m_goalScale;
		mTweakBar->addValue("Goal", m_uiSchoolGoalScale, 0.0f, 10.0f, 0.1f, UIACTION_SCHOOLGOAL);
		m_uiSchoolAlignmentScale = desc.m_flocking.m_alignmentScale;
		mTweakBar->addValue("Alignment", m_uiSchoolAlignmentScale, 0.0f, 10.0f, 0.1f, UIACTION_SCHOOLALIGNMENT);
		m_uiSchoolRepulsionScale = desc.m_flocking.m_repulsionScale;
		mTweakBar->addValue("Repulsion", m_uiSchoolRepulsionScale, 0.0f, 10.0f, 0.1f, UIACTION_SCHOOLREPULSION);
		m_uiSchoolCohesionScale = desc.m_flocking.m_cohesionScale;
		mTweakBar->addValue("Cohesion", m_uiSchoolCohesionScale, 0.0f, 10.0f, 0.1f, UIACTION_SCHOOLCOHESION);
		m_uiSchoolAvoidanceScale = desc.m_flocking.m_schoolAvoidanceScale;
		mTweakBar->addValue("Avoidance", m_uiSchoolAvoidanceScale, 0.0f, 10.0f, 0.1f, UIACTION_SCHOOLAVOIDANCE);
		mTweakBar->subgroupEnd();
#endif
	}

	// UI elements for displaying triangle statistics
	if (mFPSText)
	{
		NvUIRect fpsRect;
		mFPSText->GetScreenRect(fpsRect); // base off of fps element.
		// pre-size the rectangle with fake text
		NvUIRect textRect;

		m_fullTimingStats = new NvUIText("______________________________----------\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",
			NvUIFontFamily::SANS, (mFPSText->GetFontSize() * 2) / 3, NvUITextAlign::LEFT);
		m_fullTimingStats->SetColor(NV_PACKED_COLOR(255, 255, 255, 255));
		m_fullTimingStats->SetShadow();
		m_fullTimingStats->GetScreenRect(textRect);
		m_fullStatsBox = new NvUIContainer(textRect.width, textRect.height, new NvUIGraphicFrame("popup_frame.dds", 24, 24));
		m_fullStatsBox->Add(m_fullTimingStats, 8.0f, 8.0f);
		mUIWindow->Add(m_fullStatsBox, fpsRect.left + fpsRect.width - textRect.width, fpsRect.top + fpsRect.height);
		m_fullStatsBox->SetVisibility(m_statsMode == STATS_FULL);

		m_simpleTimingStats = new NvUIText("__________________----------\n\n\n\n\n\n\n\n\n\n\n\n\n",
			NvUIFontFamily::SANS, mFPSText->GetFontSize(), NvUITextAlign::LEFT);
		m_simpleTimingStats->SetColor(NV_PACKED_COLOR(218, 218, 0, 255));
		m_simpleTimingStats->SetShadow();
		m_simpleTimingStats->GetScreenRect(textRect);
		m_simpleStatsBox = new NvUIContainer(textRect.width, textRect.height, new NvUIGraphicFrame("popup_frame.dds", 24, 24));
		m_simpleStatsBox->Add(m_simpleTimingStats, 8.0f, 8.0f);
		mUIWindow->Add(m_simpleStatsBox, fpsRect.left + fpsRect.width - textRect.width, fpsRect.top + fpsRect.height);
		m_simpleStatsBox->SetVisibility(m_statsMode == STATS_SIMPLE);
	}

	// Initially create our logos and add them to the window, but hidden in the upper corner.  
	// Let the reshape call fix up the size and location
	m_logoNVIDIA = initLogoTexture("textures/LogoNVIDIA.dds");
	m_logoGLES = initLogoTexture("textures/LogoGLES.dds");
	m_logoGL = initLogoTexture("textures/LogoGL.dds");
}

NvUIGraphic* ThreadedRenderingGL::initLogoTexture(const char* pTexFilename)
{
	float logoAlpha = 0.5f;
	std::string texName = pTexFilename;
	NvUIGraphic* pLogo;
	//m_logoNVIDIA = new NvUIGraphic(texName);
	{
		//NvUITexture* tex = NvUITexture::CacheTexture(texName);
		NvUITexture* tex = new NvUITexture(texName);
		pLogo = new NvUIGraphic(tex);

	}
	pLogo->SetAlpha(logoAlpha);
	pLogo->SetVisibility(false);
	mUIWindow->Add(pLogo, 0, 0);
	return pLogo;
}

void ThreadedRenderingGL::resetFish(bool fishsplosion)
{
	if (fishsplosion)
	{
		nv::vec3f location = (ms_tankMin + ms_tankMax) * 0.5f;
		for (uint32_t schoolIndex = 0; schoolIndex < m_schools.size(); ++schoolIndex)
		{
			School* pSchool = m_schools[schoolIndex];
			pSchool->ResetToLocation(location);
		}
	}
	else
	{
		//  Fish Fireworks reset
		for (uint32_t schoolIndex = 0; schoolIndex < m_schools.size(); ++schoolIndex)
		{
			School* pSchool = m_schools[schoolIndex];
			nv::vec3f tankSize = ms_tankMax - ms_tankMin;
			nv::vec3f location((float)rand() / (float)RAND_MAX * tankSize.x,
				(float)rand() / (float)RAND_MAX * tankSize.y,
				(float)rand() / (float)RAND_MAX * tankSize.z);
			location += ms_tankMin;
			pSchool->ResetToLocation(location);
		}
	}
}

NvUIEventResponse ThreadedRenderingGL::handleReaction(const NvUIReaction &react)
{
	// Simple input hook to ensure that all settings are compatible after
	// a user has chosen to change something.  We'll use a flag to indicate that
	// we actually modified something so that we can force the other UI elements
	// to update to match their new state before we return.
	bool bStateModified = false;
	switch (react.code)
	{
	case UIACTION_RESET_FISHPLOSION:
	{
		resetFish(true);
		m_pFishplosionVar->reset();
		bStateModified = true;
		break;
	}
	case UIACTION_RESET_FISHFIREWORKS:
	{
		resetFish(false);
		m_pFishFireworksVar->reset();
		bStateModified = true;
		break;
	}
	case UIACTION_RESET:
	{
		switch (react.ival)
		{
		case RESET_ORIGIN:
		{
			resetFish(true);
			break;
		}
		case RESET_RANDOM:
		{
			resetFish(false);
			break;
		}
		}

		// Reset the camera
		m_pInputHandler->setPosition(m_startingCameraPosition);
		m_pInputHandler->setYaw(m_startingCameraPitchYaw.y);
		m_pInputHandler->setPitch(m_startingCameraPitchYaw.x);
		break;
	}
	case UIACTION_SCHOOLCOUNT:
	{
		m_uiSchoolCount = setNumSchools(react.ival);
		m_bForceSchoolUpdate = true;
		bStateModified = true;
		break;
	}
	case UIACTION_INSTCOUNT:
	{
		// Update our readout of total number of active fish
		m_uiFishCount = m_activeSchools * m_uiInstanceCount;

		if (m_pBatchSlider->GetValue() > m_uiInstanceCount) {
			m_pBatchSlider->SetValue(m_uiInstanceCount);
			*m_pBatchVar = m_uiInstanceCount;
		}

		m_pBatchSlider->SetMaxValue(m_uiInstanceCount);
		m_pBatchVar->SetMaxValue(m_uiInstanceCount);

		bStateModified = true;
		m_bUIDirty = true;
		updateUI();
		if (nullptr != m_pVBOPool)
		{
			m_pVBOPool->SetSubRangeSize(School::GetInstanceDataStride() * m_uiInstanceCount);
		}
		for (uint32_t schoolIndex = 0; schoolIndex < m_schools.size(); ++schoolIndex)
		{
			School* pSchool = m_schools[schoolIndex];
			pSchool->SetInstanceCount(m_uiInstanceCount);
		}


		m_bForceSchoolUpdate = true;
		break;
	}
	case UIACTION_TANKSIZE:
	{
		ms_tankMax.x = ms_tankMax.z = (float)m_uiTankSize;
		ms_tankMin.x = ms_tankMin.z = -ms_tankMax.x;
		m_bTankSizeChanged = true;
		break;
	}
	case UIACTION_BATCHSIZE:
	{
		break;
	}
	case UIACTION_SCHOOLINFOID:
	{
		m_uiSchoolInfoId = react.ival;
		if (m_schoolDescs.size() <= m_uiSchoolInfoId)
		{
			initializeSchoolDescriptions(m_uiSchoolInfoId + 1);
		}
		SchoolDesc& desc = m_schoolDescs[m_uiSchoolInfoId];
		m_uiSchoolDisplayModelIndex = desc.m_modelId;
		m_uiSchoolMaxSpeed = desc.m_flocking.m_maxSpeed;
		m_uiSchoolInertia = desc.m_flocking.m_inertia;
		m_uiSchoolNeighborDistance = desc.m_flocking.m_neighborDistance;
		m_uiSchoolGoalScale = desc.m_flocking.m_goalScale;
		m_uiSchoolAlignmentScale = desc.m_flocking.m_alignmentScale;
		m_uiSchoolRepulsionScale = desc.m_flocking.m_repulsionScale;
		m_uiSchoolCohesionScale = desc.m_flocking.m_cohesionScale;
		m_uiSchoolAvoidanceScale = desc.m_flocking.m_schoolAvoidanceScale;
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLMODELINDEX:
	{
		uint32_t modelId = react.ival;
		m_uiSchoolDisplayModelIndex = modelId;
		m_schoolDescs[m_uiSchoolInfoId].m_modelId = modelId;
		m_schoolDescs[m_uiSchoolInfoId].m_scale = ms_schoolInfo[modelId].m_scale;

		if (m_schools.size() > m_uiSchoolInfoId)
		{
			m_schools[m_uiSchoolInfoId]->SetModel(
				m_models[modelId],
				modelId,
				ms_modelInfo[modelId].m_tailStartZ * ms_schoolInfo[modelId].m_scale,
				ms_modelInfo[modelId].getCenteringTransform(),
				ms_modelInfo[modelId].m_halfExtents);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLMAXSPEED:
	{
		m_uiSchoolMaxSpeed = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_maxSpeed = m_uiSchoolMaxSpeed;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_maxSpeed = m_uiSchoolMaxSpeed;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLINERTIA:
	{
		m_uiSchoolInertia = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_inertia = m_uiSchoolInertia;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_inertia = m_uiSchoolInertia;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLNEIGHBORDIST:
	{
		m_uiSchoolNeighborDistance = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_neighborDistance = m_uiSchoolNeighborDistance;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_neighborDistance = m_uiSchoolNeighborDistance;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLGOAL:
	{
		m_uiSchoolGoalScale = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_goalScale = m_uiSchoolGoalScale;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_goalScale = m_uiSchoolGoalScale;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLALIGNMENT:
	{
		m_uiSchoolAlignmentScale = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_alignmentScale = m_uiSchoolAlignmentScale;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_alignmentScale = m_uiSchoolAlignmentScale;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLREPULSION:
	{
		m_uiSchoolRepulsionScale = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_repulsionScale = m_uiSchoolRepulsionScale;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_repulsionScale = m_uiSchoolRepulsionScale;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLCOHESION:
	{
		m_uiSchoolCohesionScale = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_cohesionScale = m_uiSchoolCohesionScale;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_cohesionScale = m_uiSchoolCohesionScale;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_SCHOOLAVOIDANCE:
	{
		m_uiSchoolAvoidanceScale = react.fval;
		m_schoolDescs[m_uiSchoolInfoId].m_flocking.m_schoolAvoidanceScale = m_uiSchoolAvoidanceScale;
		if (m_schools.size() > m_uiSchoolInfoId)
		{
			School* pSchool = m_schools[m_uiSchoolInfoId];
			SchoolFlockingParams params = pSchool->GetFlockingParams();
			params.m_schoolAvoidanceScale = m_uiSchoolAvoidanceScale;
			pSchool->SetFlockingParams(params);
		}
		bStateModified = true;
		break;
	}
	case UIACTION_ANIMTHREADCOUNT:
	{
		m_uiThreadCount = setAnimationThreadNum(react.ival) + 1;
		bStateModified = true;
		break;
	}
	case UIACTION_STATSTOGGLE:
	{
		m_statsMode = (m_statsMode + 1) % STATS_COUNT;

		switch (m_statsMode)
		{
		case STATS_FULL:
			if (nullptr != m_fullStatsBox)
			{
				m_fullStatsBox->SetVisibility(true);
			}
			if (nullptr != m_simpleStatsBox)
			{
				m_simpleStatsBox->SetVisibility(false);
			}
			break;
		case STATS_SIMPLE:
			if (nullptr != m_fullStatsBox)
			{
				m_fullStatsBox->SetVisibility(false);
			}
			if (nullptr != m_simpleStatsBox)
			{
				m_simpleStatsBox->SetVisibility(true);
			}
			break;
		default:
			if (nullptr != m_fullStatsBox)
			{
				m_fullStatsBox->SetVisibility(false);
			}
			if (nullptr != m_simpleStatsBox)
			{
				m_simpleStatsBox->SetVisibility(false);
			}
			break;
		}
		m_pStatsModeVar->reset();
		bStateModified = true;
		break;
	}
	case UIACTION_ANIMPAUSED:
	{
		m_bForceSchoolUpdate = true;
		break;
	}
	case UIACTION_RENDERINGTECHNIQUE:
	{
		Nv::VBOPolicy policy;
		switch (react.ival)
		{
		case TECHNIQUE_GLES_SUBRANGE:
			policy = Nv::VBO_SUBRANGE;
			break;
		case TECHNIQUE_GLES_SUBRANGE_PERSISTENT:
			policy = Nv::VBO_SUBRANGE_PERSISTENT;
			break;
		case TECHNIQUE_GLES_ORPHANED:
			policy = Nv::VBO_ORPHANED;
			break;
		case TECHNIQUE_GLES_POOLED:
			policy = Nv::VBO_POOLED;
			break;
		case TECHNIQUE_GLES_POOLED_PERSISTENT:
			policy = Nv::VBO_POOLED_PERSISTENT;
			break;
		case TECHNIQUE_GLAZDO_POOLED:
		case TECHNIQUE_GLAZDO_POOLED_PERSISTENT:
			policy = m_ESVBOPolicy;
			break;
		}

		const cb::DrawKey key(0); // execute at the end of the frame as vbo may still be in bound
		auto* cmd = m_geometryCommands.addCommand<SetVBOPolicyCommand>(key);
		cmd->policy = policy;
		cmd->threadedRenderingGL = this;
		CB_DEBUG_COMMAND_TAG(cmd);

		bStateModified = true;
		break;
	}
	}
	m_bUIDirty |= bStateModified;
	updateUI();
	return nvuiEventNotHandled;
}

bool ThreadedRenderingGL::handleGamepadButtonChanged(uint32_t button, bool down)
{
	// allow fishsplosion with closed UI
	if (!mUIWindow->GetVisibility() && (button == NvGamepad::BUTTON_Y))
	{
		if (down)
		{
			resetFish(true);
		}
		return true;
	}

	if (button == NvGamepad::BUTTON_X)
	{
		if (down)
		{
			mUIWindow->SetVisibility(!mUIWindow->GetVisibility());
			mTweakBar->SetVisibility(false);
		}
		return true;
	}
	return false;
}


uint32_t ThreadedRenderingGL::getCurrentRenderingTechnique()
{
	{
		switch (m_ESVBOPolicy)
		{
		case Nv::VBO_SUBRANGE:
			return TECHNIQUE_GLES_SUBRANGE;
		case Nv::VBO_SUBRANGE_PERSISTENT:
			return TECHNIQUE_GLES_SUBRANGE_PERSISTENT;
		case Nv::VBO_ORPHANED:
			return TECHNIQUE_GLES_ORPHANED;
		case Nv::VBO_POOLED:
			return TECHNIQUE_GLES_POOLED;
		case Nv::VBO_POOLED_PERSISTENT:
			return TECHNIQUE_GLES_POOLED_PERSISTENT;
		}
	}
	return TECHNIQUE_GLES_SUBRANGE;
}

void ThreadedRenderingGL::updateUI()
{
	if (m_uiSchoolCount != m_activeSchools)
	{
		m_uiSchoolCount = m_activeSchools;
		m_bUIDirty = true;
	}

	uint32_t currentRenderingTechnique = getCurrentRenderingTechnique();
	if (m_uiRenderingTechnique != currentRenderingTechnique)
	{
		m_uiRenderingTechnique = currentRenderingTechnique;
		m_bUIDirty = true;
	}

	if (m_bUIDirty && (nullptr != mTweakBar))
	{
		mTweakBar->syncValues();
		m_bUIDirty = false;
	}
}

void ThreadedRenderingGL::reshape(int32_t width, int32_t height)
{
	glViewport(0, 0, NvSampleApp::m_width, NvSampleApp::m_height);

	//setting the perspective projection matrix
	nv::perspective(m_projUBO_Data.m_projectionMatrix, NV_PI / 3.0f,
		static_cast<float>(NvSampleApp::m_width) /
		static_cast<float>(NvSampleApp::m_height),
		0.1f, 100.0f);

	//setting the inverse perspective projection matrix
	m_projUBO_Data.m_inverseProjMatrix =
		nv::inverse(m_projUBO_Data.m_projectionMatrix);
	NvUIRect fpsRect;
	mFPSText->GetScreenRect(fpsRect); // base off of fps element.
	NvUIRect textRect;
	m_simpleTimingStats->GetScreenRect(textRect);
	mUIWindow->Remove(m_simpleStatsBox);
	mUIWindow->Add(m_simpleStatsBox, fpsRect.left + fpsRect.width - textRect.width, fpsRect.top + fpsRect.height);
	m_fullTimingStats->GetScreenRect(textRect);
	mUIWindow->Remove(m_fullStatsBox);
	mUIWindow->Add(m_fullStatsBox, fpsRect.left + fpsRect.width - textRect.width, fpsRect.top + fpsRect.height);
	updateLogos();
}

void ThreadedRenderingGL::updateLogos()
{
	if (nullptr == mUIWindow)
	{
		return;
	}

	// Use the smallest dimension to determine scaling for logos
	bool bFitToHeight = mUIWindow->GetHeight() < mUIWindow->GetWidth();

	// Offset the logos from the sides of the window
	float logoPadding = 20.0f;

	// Scale the logo to a percentage of the main window
	float logoRelSize = 0.1f;

	float logoOrigWidth;
	float logoOrigHeight;
	// Initialize both of our display dimensions based on the current window size,
	// then we will leave the primary dimension alone, but overwrite the dependant one
	float logoDisplayWidth = mUIWindow->GetWidth() * logoRelSize;
	float logoDisplayHeight = mUIWindow->GetHeight() * logoRelSize;

	// Nvidia Logo
	if (nullptr != m_logoNVIDIA)
	{
		logoOrigWidth = m_logoNVIDIA->GetTex()->GetWidth();
		logoOrigHeight = m_logoNVIDIA->GetTex()->GetHeight();
		if (bFitToHeight)
		{
			// When fitting to height, we keep our calculated height value and then calculate
			// a new width that will preserve the original aspect ratio
			logoDisplayWidth = (logoDisplayHeight / logoOrigHeight) * logoOrigWidth;
		}
		else
		{
			// When fitting to width, we keep our calculated width value and then calculate
			// a new height that will preserve the original aspect ratio
			logoDisplayHeight = (logoDisplayWidth / logoOrigWidth) * logoOrigHeight;
		}
		m_logoNVIDIA->SetDimensions(logoDisplayWidth, logoDisplayHeight);
		mUIWindow->Remove(m_logoNVIDIA);
		mUIWindow->Add(m_logoNVIDIA, mUIWindow->GetWidth() - logoPadding - logoDisplayWidth, mUIWindow->GetHeight() - logoPadding - logoDisplayHeight);
		m_logoNVIDIA->SetVisibility(m_bDisplayLogos);
	}

	// GLES Logo
	if (nullptr != m_logoGLES)
	{
		logoOrigWidth = m_logoGLES->GetTex()->GetWidth();
		logoOrigHeight = m_logoGLES->GetTex()->GetHeight();
		if (bFitToHeight)
		{
			// When fitting to height, we keep our calculated height value and then calculate
			// a new width that will preserve the original aspect ratio
			logoDisplayWidth = (logoDisplayHeight / logoOrigHeight) * logoOrigWidth;
		}
		else
		{
			// When fitting to width, we keep our calculated width value and then calculate
			// a new height that will preserve the original aspect ratio
			logoDisplayHeight = (logoDisplayWidth / logoOrigWidth) * logoOrigHeight;
		}
		m_logoGLES->SetDimensions(logoDisplayWidth, logoDisplayHeight);
		mUIWindow->Remove(m_logoGLES);
		mUIWindow->Add(m_logoGLES, logoPadding, mUIWindow->GetHeight() - logoPadding - logoDisplayHeight);
		m_logoGLES->SetVisibility(m_bDisplayLogos);
	}

	// GL Logo
	if (nullptr != m_logoGL)
	{
		logoOrigWidth = m_logoGL->GetTex()->GetWidth();
		logoOrigHeight = m_logoGL->GetTex()->GetHeight();
		if (bFitToHeight)
		{
			// When fitting to height, we keep our calculated height value and then calculate
			// a new width that will preserve the original aspect ratio
			logoDisplayWidth = (logoDisplayHeight / logoOrigHeight) * logoOrigWidth;
		}
		else
		{
			// When fitting to width, we keep our calculated width value and then calculate
			// a new height that will preserve the original aspect ratio
			logoDisplayHeight = (logoDisplayWidth / logoOrigWidth) * logoOrigHeight;
		}
		m_logoGL->SetDimensions(logoDisplayWidth, logoDisplayHeight);
		mUIWindow->Remove(m_logoGL);
		mUIWindow->Add(m_logoGL, logoPadding, mUIWindow->GetHeight() - logoPadding - logoDisplayHeight);
	}
}

float ThreadedRenderingGL::getClampedFrameTime() {
	float delta = getFrameDeltaTime();
	if (delta > 0.2f)
		delta = 0.2f;
	return delta;
}

void ThreadedRenderingGL::draw(void)
{
	neighborOffset = (neighborOffset + 1) % (6 - neighborSkip);

	s_threadMask = 0;

	m_currentTime += getClampedFrameTime();

#if FISH_DEBUG
	LOGI("\n################################################################");
	LOGI("BEGINNING OF FRAME");
#endif

	// Render the requested content (from dropdown menu in TweakBar UI)
	{
		CPU_TIMER_SCOPE(CPU_TIMER_MAIN_WAIT);

		for (uint32_t i = 0; i < MAX_ANIMATION_THREAD_COUNT; i++) {
			ThreadData& thread = m_threads[i];
			thread.m_cmdBufferOpen = false;
			thread.m_drawCallCount = 0;
		}

#ifdef USE_STATIC_THREAD_WORK
		// If we're assigning a fixed number of schools for each thread
		// to update, set up each of the ThreadData structures with 
		// the appropriate ranges
		uint32_t schoolsPerThread = m_activeSchools / m_activeAnimationThreads;
		uint32_t remainderSchools = m_activeSchools % m_activeAnimationThreads;
		uint32_t baseSchool = 0;
		for (uint32_t i = 0; i < m_activeAnimationThreads; i++) {
			ThreadData& thread = m_threads[i];
			thread.m_baseSchoolIndex = baseSchool;
			thread.m_schoolCount = schoolsPerThread;
			// distribute the remainder evenly
			if (remainderSchools > 0) {
				thread.m_schoolCount++;
				remainderSchools--;
			}

			baseSchool += thread.m_schoolCount;
		}
#endif

		m_doneCount = 0;
		m_drawCallCount = 0;

#ifndef USE_STATIC_THREAD_WORK
		// If each thread will request a new school to update after
		// it has completed its current one, we need to populate
		// our queue with the work required.  Since we are processing
		// a sequential vector of schools, we could simply have a
		// "next index" variable that we synchronize on and 
		// increment, but this implementation illustrates a more
		// general way to deal with a work queue.
		m_NeedsUpdateQueueLock->lockMutex();
		{
			uint32_t schoolIndex = 0;
			for (; schoolIndex < m_activeSchools; ++schoolIndex)
			{
				ThreadJob job;
				job.school = m_schools[schoolIndex];
				job.index = schoolIndex;

				m_NeedsUpdateQueue.push_back(job);
			}
		}
		m_NeedsUpdateQueueLock->unlockMutex();
#endif

		// Work is ready to begin.  Signal the threads that we're
		// ready for them to start updating schools
		m_frameStartLock->lockMutex();
		{
			m_frameID++;
			m_frameStartCV->broadcastConditionVariable();
		}
		m_frameStartLock->unlockMutex();

		// Now wait until all schools have finished doing their updates
		// before moving on to rendering.
		m_doneCountLock->lockMutex();
		{
			const uint32_t extraWorkCount = 1;
			while (m_doneCount != (m_activeSchools + extraWorkCount))
			{
				m_doneCountCV->waitConditionVariable(m_doneCountLock);
			}
		}
		m_doneCountLock->unlockMutex();
	}

	// Rendering
	{
		CPU_TIMER_SCOPE(CPU_TIMER_MAIN_CMD_BUILD);
		GPU_TIMER_SCOPE();
		m_geometryCommands.sort();
		// nothing to pass as GL doesn't have any contexts
		m_geometryCommands.submit(nullptr);
	}
	{
		// Able to cheat somewhat here, since we know that all fish will use the same shader to render
		{
			m_shader_Fish->enable();
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (nullptr != m_pVBOPool)
		{
			if (!m_animPaused || m_bForceSchoolUpdate)
			{
				m_pVBOPool->EndUpdate();
			}
			{
				for (uint32_t schoolIndex = 0; schoolIndex < m_activeSchools; ++schoolIndex)
				{
					School* pSchool = m_schools[schoolIndex];
					{
						CPU_TIMER_SCOPE(CPU_TIMER_MAIN_CMD_BUILD);
						GPU_TIMER_SCOPE();
						m_drawCallCount += pSchool->Render(m_uiBatchSize);
					}
				}
			}
			m_pVBOPool->DoneRendering();
		}
		else
		{
			for (uint32_t schoolIndex = 0; schoolIndex < m_activeSchools; ++schoolIndex)
			{
				School* pSchool = m_schools[schoolIndex];
				{
					CPU_TIMER_SCOPE(CPU_TIMER_MAIN_COPYVBO);
					if (!m_animPaused || m_bForceSchoolUpdate)
					{
						pSchool->Update();
					}
				}
				{
					CPU_TIMER_SCOPE(CPU_TIMER_MAIN_CMD_BUILD);
					GPU_TIMER_SCOPE();
					m_drawCallCount += pSchool->Render(m_uiBatchSize);
				}
			}
		}
		{
			m_shader_Fish->disable();
		}
		m_bForceSchoolUpdate = false;
	}

	// We've completed rendering, so create our per-frame fence, if we're using them.
	if (nullptr != m_fences)
	{
		m_fences[m_currentFenceIndex] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}
	else
	{
		// HACK!
		// Trigger some kind of driver optimization that ups our frame rate.  For 
		// some reason, if fences aren't used at all, the driver seems to sync somewhere.
		// by simply creating a throw away fence, we speed things up.
		glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

#if FISH_DEBUG
	LOGI("END OF FRAME");
	LOGI("################################################################\n");
#endif
	updateStats();
}

//-----------------------------------------------------------------------------
// PRIVATE METHODS

void ThreadedRenderingGL::cleanRendering(void)
{
}

void ThreadedRenderingGL::initThreads(void)
{
	NV_ASSERT(nullptr != pDevice);

	NvThreadManager* threadManager = getThreadManagerInstance();
	NV_ASSERT(nullptr != threadManager);

	NV_ASSERT(m_FrameStartLock == NULL);
	NV_ASSERT(m_FrameStartCV == NULL);
	NV_ASSERT(m_NeedsUpdateQueueLock == NULL);
	NV_ASSERT(m_DoneCountLock == NULL);
	NV_ASSERT(m_DoneCountCV == NULL);

	// Create our synchronization objects
	m_frameStartLock =
		threadManager->initializeMutex(false, NvMutex::MutexLockLevelInitial);
	m_frameStartCV =
		threadManager->initializeConditionVariable();
	m_needsUpdateQueueLock =
		threadManager->initializeMutex(false, NvMutex::MutexLockLevelInitial);
	m_doneCountLock =
		threadManager->initializeMutex(false, NvMutex::MutexLockLevelInitial);
	m_doneCountCV = threadManager->initializeConditionVariable();

	NV_ASSERT(m_FrameStartLock != NULL);
	NV_ASSERT(m_FrameStartCV != NULL);
	NV_ASSERT(m_NeedsUpdateQueueLock != NULL);
	NV_ASSERT(m_DoneCountLock != NULL);
	NV_ASSERT(m_DoneCountCV != NULL);

	// Initialize each of our animation worker threads
	for (intptr_t i = 0; i < MAX_ANIMATION_THREAD_COUNT; i++)
	{
		ThreadData& thread = m_threads[i];
		if (thread.m_thread != NULL)
			delete thread.m_thread;

		void* threadIndex = reinterpret_cast<void*>(i);
		m_threads[i].m_thread =
			threadManager->createThread(animateJobFunctionThunk, &thread,
				&(m_threadStacks[i]),
				THREAD_STACK_SIZE,
				NvThread::DefaultThreadPriority);

		NV_ASSERT(thread.m_thread != NULL);
		thread.m_thread->startThread();
	}
	// Initialize our helper thread
	{
		const int i = MAX_THREAD_COUNT - 1;
		ThreadData& thread = m_threads[i];
		thread.m_thread = threadManager->createThread(helperJobFunctionThunk, this,
			&(m_threadStacks[i]),
			THREAD_STACK_SIZE,
			NvThread::DefaultThreadPriority);
		NV_ASSERT(thread.m_thread != NULL);
		thread.m_thread->startThread();
	}

	m_uiThreadCount = setAnimationThreadNum(MAX_ANIMATION_THREAD_COUNT) + 1;
}

void ThreadedRenderingGL::cleanThreads(void)
{
	NV_ASSERT(nullptr != pDevice);

	NvThreadManager* threadManager = getThreadManagerInstance();
	NV_ASSERT(nullptr != threadManager);

	m_running = false;
	if (m_frameStartCV)
		m_frameStartCV->broadcastConditionVariable();

	for (uint32_t i = 0; i < MAX_THREAD_COUNT; i++)
	{
		if (m_threads[i].m_thread != NULL)
		{
			m_threads[i].m_thread->waitThread();
			threadManager->destroyThread(m_threads[i].m_thread);
			m_threads[i].m_thread = NULL;
		}
	}

	if (m_needsUpdateQueueLock)
		threadManager->finalizeMutex(m_needsUpdateQueueLock);
	if (m_doneCountLock)
		threadManager->finalizeMutex(m_doneCountLock);
	if (m_doneCountCV)
		threadManager->finalizeConditionVariable(m_doneCountCV);

	m_frameStartLock = NULL;
	m_frameStartCV = NULL;
	m_needsUpdateQueueLock = NULL;
	m_doneCountLock = NULL;
	m_doneCountCV = NULL;
}

// Static Data to define available models
static const nv::matrix4f sc_yawNeg90(
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

static const nv::matrix4f sc_yaw180(
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

static const nv::vec3f sc_zeroVec(0.0f, 0.0f, 0.0f);

// Initialize the model desc array with data that we know, leaving the bounding
// box settings as zeroes.  We'll fill those in when the models are loaded.
ThreadedRenderingGL::ModelDesc ThreadedRenderingGL::ms_modelInfo[MODEL_COUNT] =
{
	{ "Black & White Fish", "models/Black_White_Fish.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.10f },
	{ "Blue Fish 1", "models/Blue_Fish.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Blue Fish 2", "models/Blue_Fish_02.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Blue Fish 3", "models/Blue_Fish_03.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Blue Fish 4", "models/Blue_Fish_04.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Blue Fish 5", "models/Blue_Fish_05.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Blue Fish 6", "models/Blue_Fish_06.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.15f },
	{ "Blue Fish 7", "models/Blue_Fish_07.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.35f },
	{ "Blue Fish 8", "models/Blue_Fish_08.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Blue Fish 9", "models/Blue_Fish_09.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.20f },
	{ "Cyan Fish", "models/Cyan_Fish.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Pink Fish", "models/Pink_Fish.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.20f },
	{ "Red Fish", "models/Red_Fish.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.28f },
	{ "Violet Fish", "models/Violet_Fish.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Yellow Fish 1", "models/Yellow_Fish.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.40f },
	{ "Yellow Fish 2", "models/Yellow_Fish_02.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.15f },
	{ "Yellow Fish 3", "models/Yellow_Fish_03.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Yellow Fish 4", "models/Yellow_Fish_04.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Yellow Fish 5", "models/Yellow_Fish_05.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Yellow Fish 6", "models/Yellow_Fish_06.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Yellow Fish 7", "models/Yellow_Fish_07.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Yellow Fish 8", "models/Yellow_Fish_08.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.23f },
	{ "Yellow Fish 9", "models/Yellow_Fish_09.nve", sc_yawNeg90, sc_zeroVec, sc_zeroVec, 0.25f },
	{ "Yellow Fish 10", "models/Yellow_Fish_10.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.30f },
	{ "Yellow Fish 11", "models/Yellow_Fish_11.nve", sc_yaw180, sc_zeroVec, sc_zeroVec, 0.32f }
};

SchoolFlockingParams ThreadedRenderingGL::ms_fishTypeDefs[FISHTYPE_COUNT] =
{
	//     |       |Goal|      Spawn Zone       |Neighbor|Spawn|          |<***************** Strengths ****************>|             
	// Spd |Inertia|Size|    Min         Max    |Distance|Range|Aggression| Goal |Alignment|Repulsion |Cohesion|Avoidance|
	SchoolFlockingParams(1.5f,  16.0f, 8.0f, ms_tankMin, ms_tankMax,  4.00f,  0.01f,   0.9f,    0.1f,   0.1f,    0.10f,      0.5f,     0.1f), // EXTRALARGE
	SchoolFlockingParams(1.5f,  16.0f, 6.0f, ms_tankMin, ms_tankMax,  3.50f,  0.01f,   0.8f,    0.1f,   0.1f,    0.10f,      0.5f,     1.0f), // LARGESLOW
	SchoolFlockingParams(2.5f,  16.0f, 6.0f, ms_tankMin, ms_tankMax,  3.00f,  0.01f,   0.7f,    0.1f,   0.1f,    0.15f,      0.5f,     1.0f), // LARGE
	SchoolFlockingParams(3.5f,  12.0f, 5.0f, ms_tankMin, ms_tankMax,  2.50f,  0.01f,   0.6f,    0.2f,   0.2f,    0.10f,      0.5f,     1.0f), // LARGEFAST
	SchoolFlockingParams(2.5f,  14.0f, 5.0f, ms_tankMin, ms_tankMax,  2.00f,  0.01f,   0.5f,    0.1f,   0.1f,    0.15f,      0.5f,     2.0f), // MEDIUMSLOW
	SchoolFlockingParams(3.5f,  12.0f, 4.0f, ms_tankMin, ms_tankMax,  1.60f,  0.01f,   0.4f,    0.1f,   0.1f,    0.15f,      0.5f,     2.0f), // MEDIUM
	SchoolFlockingParams(6.0f,  10.0f, 3.0f, ms_tankMin, ms_tankMax,  1.40f,  0.01f,   0.3f,    0.2f,   0.1f,    0.10f,      0.5f,     2.0f), // MEDIUMFAST
	SchoolFlockingParams(5.0f,  10.0f,10.0f, ms_tankMin, ms_tankMax,  1.50f,  0.01f,   0.1f,    0.1f,   0.1f,    0.15f,      0.5f,     3.0f), // MEDIUMSPARSE
	SchoolFlockingParams(3.0f,   8.0f, 3.0f, ms_tankMin, ms_tankMax,  1.00f,  0.01f,   0.2f,    0.1f,   0.2f,    0.10f,      0.5f,     4.0f), // SMALLSLOW
	SchoolFlockingParams(5.0f,   5.0f, 2.0f, ms_tankMin, ms_tankMax,  0.25f,  0.01f,   0.1f,    0.1f,   0.4f,    0.15f,      0.5f,     5.0f), // SMALL
	SchoolFlockingParams(7.0f,   4.0f, 1.0f, ms_tankMin, ms_tankMax,  0.25f,  0.01f,   0.1f,    0.2f,   0.5f,    0.40f,      0.1f,     6.0f)  // SMALLFAST
};

ThreadedRenderingGL::SchoolDesc ThreadedRenderingGL::ms_schoolInfo[MODEL_COUNT] =
{
	// ModelId,           NumFish, Scale, 
	{ MODEL_BLACK_WHITE_FISH,  75, 2.00f, ms_fishTypeDefs[FISHTYPE_LARGEFAST] },
	{ MODEL_BLUE_FISH,         75, 2.00f, ms_fishTypeDefs[FISHTYPE_LARGE] },
	{ MODEL_BLUE_FISH_02,     100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_BLUE_FISH_03,     100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMSLOW] },
	{ MODEL_BLUE_FISH_04,     100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_BLUE_FISH_05,     100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_BLUE_FISH_06,     100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMFAST] },
	{ MODEL_BLUE_FISH_07,     200, 0.50f, ms_fishTypeDefs[FISHTYPE_SMALLFAST] },
	{ MODEL_BLUE_FISH_08,     200, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMSPARSE] },
	{ MODEL_BLUE_FISH_09,      50, 3.00f, ms_fishTypeDefs[FISHTYPE_EXTRALARGE] },
	{ MODEL_CYAN_FISH,        100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_PINK_FISH,        150, 0.75f, ms_fishTypeDefs[FISHTYPE_SMALLSLOW] },
	{ MODEL_RED_FISH,         50,  3.00f, ms_fishTypeDefs[FISHTYPE_LARGESLOW] },
	{ MODEL_VIOLET_FISH,      250, 0.50f, ms_fishTypeDefs[FISHTYPE_SMALLFAST] },
	{ MODEL_YELLOW_FISH,      100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMFAST] },
	{ MODEL_YELLOW_FISH_02,   100, 1.50f, ms_fishTypeDefs[FISHTYPE_MEDIUMSLOW] },
	{ MODEL_YELLOW_FISH_03,   100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMFAST] },
	{ MODEL_YELLOW_FISH_04,   100, 0.75f, ms_fishTypeDefs[FISHTYPE_MEDIUMFAST] },
	{ MODEL_YELLOW_FISH_05,   150, 0.80f, ms_fishTypeDefs[FISHTYPE_SMALLSLOW] },
	{ MODEL_YELLOW_FISH_06,   100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_YELLOW_FISH_07,   100, 1.20f, ms_fishTypeDefs[FISHTYPE_MEDIUMSLOW] },
	{ MODEL_YELLOW_FISH_08,   100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] },
	{ MODEL_YELLOW_FISH_09,   150, 0.80f, ms_fishTypeDefs[FISHTYPE_SMALLSLOW] },
	{ MODEL_YELLOW_FISH_10,   150, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUMSPARSE] },
	{ MODEL_YELLOW_FISH_11,   100, 1.00f, ms_fishTypeDefs[FISHTYPE_MEDIUM] }
};

static void sprintComma(uint32_t val, char* str) {
	uint32_t billions = val / 1000000000;
	val = val % 1000000000;

	uint32_t millions = val / 1000000;
	val = val % 1000000;

	uint32_t thousands = val / 1000;

	uint32_t units = val % 1000;

	if (billions) {
		sprintf(str, "%d,%03d,%03d,%03d", billions, millions, thousands, units);
	}
	else if (millions) {
		sprintf(str, "%d,%03d,%03d", millions, thousands, units);
	}
	else if (thousands) {
		sprintf(str, "%d,%03d", thousands, units);
	}
	else {
		sprintf(str, "%d", units);
	}
}

void ThreadedRenderingGL::updateStats()
{
	if (!m_statsCountdown)
	{
		float frameConv = 1000.0f / STATS_FRAMES;
		m_meanFPS = mFramerate->getMeanFramerate();
		m_meanCPUMainCmd = m_CPUTimers[CPU_TIMER_MAIN_CMD_BUILD].getScaledCycles() *
			frameConv;
		m_meanCPUMainWait = m_CPUTimers[CPU_TIMER_MAIN_WAIT].getScaledCycles() *
			frameConv;
		m_meanCPUMainCopyVBO = m_CPUTimers[CPU_TIMER_MAIN_COPYVBO].getScaledCycles() *
			frameConv;

		for (uint32_t i = 0; i < activeThreadCount(); i++) {
			ThreadTimings& t = m_threadTimings[i];
			t.cmd = m_CPUTimers[CPU_TIMER_THREAD_BASE_CMD_BUILD + i].getScaledCycles() *
				frameConv;
			t.anim = m_CPUTimers[CPU_TIMER_THREAD_BASE_ANIMATE + i].getScaledCycles() *
				frameConv;
			t.update = m_CPUTimers[CPU_TIMER_THREAD_BASE_UPDATE + i].getScaledCycles() *
				frameConv;
			t.tot = m_CPUTimers[CPU_TIMER_THREAD_BASE_TOTAL + i].getScaledCycles() *
				frameConv;
		}

		m_meanGPUFrameMS = m_GPUTimer.getScaledCycles() / STATS_FRAMES;
		m_GPUTimer.reset();

		for (int32_t i = 0; i < CPU_TIMER_COUNT; ++i)
		{
			m_CPUTimers[i].reset();
		}

		m_statsCountdown = STATS_FRAMES;

		char str[1024];
		buildSimpleStatsString(str, 1024);
		m_simpleTimingStats->SetString(str);

		buildFullStatsString(str, 1024);
		m_fullTimingStats->SetString(str);
	}
	else
	{
		m_statsCountdown--;
	}
}

void ThreadedRenderingGL::buildSimpleStatsString(char* buffer, int32_t size)
{
	setlocale(LC_NUMERIC, "");
	char fishCountStr[32];
	sprintComma(m_uiFishCount, fishCountStr);
	char fishRateStr[32];
	uint32_t fishRate = (uint32_t)(m_uiFishCount * m_meanFPS);
	sprintComma(fishRate, fishRateStr);
	char drawCallRateStr[32];
	uint32_t drawCallRate = (uint32_t)(m_drawCallCount * m_meanFPS);
	sprintComma(drawCallRate, drawCallRateStr);

	int32_t offset = sprintf(buffer,
		NvBF_COLORSTR_WHITE
		"Fish/frame: %s\n"
		NvBF_COLORSTR_NORMAL
		NVBF_STYLESTR_BOLD
		"Draw Calls/sec: %s\n"
		NvBF_COLORSTR_WHITE
		NVBF_STYLESTR_NORMAL
		"CPU Thd0: %5.1fms\n"
		"ThdID, Time\n",
		fishCountStr,
		drawCallRateStr,
		m_meanCPUMainCmd + m_meanCPUMainCopyVBO);

	for (uint32_t i = 0; i < activeThreadCount(); ++i) {
		offset += sprintf(buffer + offset,
			"Thr%01d ( %5.1fms)\n",
			i + 1, m_threadTimings[i].tot);
	}
}

void ThreadedRenderingGL::buildFullStatsString(char* buffer, int32_t size)
{
	setlocale(LC_NUMERIC, "");
	char fishCountStr[32];
	sprintComma(m_uiFishCount, fishCountStr);
	char fishRateStr[32];
	uint32_t fishRate = (uint32_t)(m_uiFishCount * m_meanFPS);
	sprintComma(fishRate, fishRateStr);
	char drawCallRateStr[32];
	uint32_t drawCallRate = (uint32_t)(m_drawCallCount * m_meanFPS);
	sprintComma(drawCallRate, drawCallRateStr);

	int32_t offset = sprintf(buffer,
		NvBF_COLORSTR_WHITE
		"Fish/frame: %s\n"
		"Fish/sec: %s\n"
		"Draw Calls/sec: %s\n"
		"CPU Thd0 CmdBuf: %5.1fms\n"
		"CPU Thd0 Wait: %5.1fms\n"
		"CPU Thd0 CopyVBO: %5.1fms\n"
		"GPU: %5.1fms\n"
		"ThdID, CmdBuf,   Anim,  Update,  TOTAL\n",
		fishCountStr,
		fishRateStr,
		drawCallRateStr, m_meanCPUMainCmd, m_meanCPUMainWait, m_meanCPUMainCopyVBO,
		m_meanGPUFrameMS);

	for (uint32_t i = 0; i < activeThreadCount(); ++i) {
		offset += sprintf(buffer + offset,
			"Thr%01d ( %5.1fms, %5.1fms, %5.1fms, %5.1fms)\n",
			i + 1, m_threadTimings[i].cmd, m_threadTimings[i].anim, m_threadTimings[i].update, m_threadTimings[i].tot);
	}
}

//-----------------------------------------------------------------------------

const cb::RenderContext::function_t ThreadedRenderingGL::InitializeCommand::kDispatchFunction = &ThreadedRenderingGL::InitializeCommand::execute;
const cb::RenderContext::function_t ThreadedRenderingGL::BeginFrameCommand::kDispatchFunction = &ThreadedRenderingGL::BeginFrameCommand::execute;
const cb::RenderContext::function_t ThreadedRenderingGL::EndFrameCommand::kDispatchFunction = &ThreadedRenderingGL::EndFrameCommand::execute;
const cb::RenderContext::function_t ThreadedRenderingGL::SetVBOPolicyCommand::kDispatchFunction = &ThreadedRenderingGL::SetVBOPolicyCommand::execute;

//-----------------------------------------------------------------------------
// FUNCTION NEEDED BY THE FRAMEWORK

NvAppBase* NvAppFactory()
{
	return new ThreadedRenderingGL();
}

