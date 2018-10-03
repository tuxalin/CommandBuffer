//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/School.h
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
#ifndef SCHOOL_H_
#define SCHOOL_H_
#include "NV/NvMath.h"
#include <vector>

#include "NvGLUtils/NvModelExtGL.h"
#include "VertexFormatBinder.h"

#include "NvSharedVBOGL_MappedSubRanges.h"
#include "NvSharedVBOGL_Orphaning.h"
#include "NvSharedVBOGL_Pooled.h"

#include "Buffers.h"

namespace Nv
{
	class NvInstancedModelExtGL;
};

class SchoolStateManager;

/// Class to hold settings for controlling the behavior
/// of a school's flocking
class SchoolFlockingParams
{
public:
	SchoolFlockingParams()
		: m_maxSpeed(0.05f)
		, m_inertia(2.0f)
		, m_arrivalDistance(1.0f)
		, m_spawnZoneMin(-20.0f, 5.0f, -20.0f)
		, m_spawnZoneMax(20.0f, 25.0f, 20.0f)
		, m_neighborDistance(0.5f)
		, m_spawnRange(0.01f)
		, m_aggression(0.5f)
		, m_goalScale(0.03f)
		, m_alignmentScale(0.1f)
		, m_repulsionScale(0.5f)
		, m_cohesionScale(0.1f)
		, m_schoolAvoidanceScale(0.5f)
	{}

	SchoolFlockingParams(
		float maxSpeed,
		float inertia,
		float arrivalDistance,
		const nv::vec3f& spawnZoneMin,
		const nv::vec3f& spawnZoneMax,
		float neighborDistance,
		float spawnRange,
		float aggression,
		float goalScale,
		float alignmentScale,
		float repulsionScale,
		float cohesionScale,
		float schoolAvoidanceScale)
		: m_maxSpeed(maxSpeed)
		, m_inertia(inertia)
		, m_arrivalDistance(arrivalDistance)
		, m_spawnZoneMin(spawnZoneMin)
		, m_spawnZoneMax(spawnZoneMax)
		, m_neighborDistance(neighborDistance)
		, m_spawnRange(spawnRange)
		, m_aggression(aggression)
		, m_goalScale(goalScale)
		, m_alignmentScale(alignmentScale)
		, m_repulsionScale(repulsionScale)
		, m_cohesionScale(cohesionScale)
		, m_schoolAvoidanceScale(schoolAvoidanceScale)
	{}

	/// Maximum speed that a fish in the school will move (in m/s)
	float m_maxSpeed;

	/// Amount of influence the fish's current heading has on its future
	/// heading.
	float m_inertia;

	/// Distance from the goal position that the current centroid must be to trigger the 
	/// determination of a new goal position.
	float m_arrivalDistance;

	/// Maximum distance from the origin in each direction that a goal
	/// position will be created
	nv::vec3f m_spawnZoneMin;
	nv::vec3f m_spawnZoneMax;

	/// Maximum distance from a fish that another fish can be to still be
	/// considered a neighbor
	float m_neighborDistance;

	/// Maximum distance from the initial position that each fish will spawn
	/// in at
	float m_spawnRange;

	/// Tendency to approach other fish versus avoid them
	float m_aggression;

	/// Dials for controlling the relative strengths of all influencing factors
	/// on each fish's movement
	float m_goalScale;            /// Swim toward goal
	float m_alignmentScale;       /// Align heading with neighbors
	float m_repulsionScale;       /// Avoid neighbors
	float m_cohesionScale;        /// Keep the school together
	float m_schoolAvoidanceScale; /// Avoid other schools
};

/// School class holds data required to render a school of fish that share
/// a model, with per-instance data controlling the position and orientation
/// of each fish.  Implements flocking behavior for the school of fish.
class School
{
public:
	struct SchoolUBO
	{
		nv::matrix4f m_modelMatrix;
		float m_tailStart;
	};
	static Nv::VertexFormatBinder* GetInstancingVertexBinder();

	School();
	School(const SchoolFlockingParams& params);
	~School();

	/// Initialize the School with beginning state as well as system objects
	/// that it will need to allocate instancing data
	/// \param index Index of the school; used to reference the school's state in the SchoolStateManager
	/// \param pModel Source model to use as the template object when rendering
	///				  instances 
	/// \param modelIndex Index of the model used for the school.  Used to identify the correct
	///                   model when used with MultiDrawIndirect
	/// \param tailStartZ Z-position where the tail starts for the fish 
	///                   (for vertex animation)
	/// \param modelTransform Fixup transform to align the provided model to a
	///						  forward of -Z and up of +Y so that the meshes'
	///						  orientation matches the alignment determined by
	///						  flocking, as well as a translation that places
	///						  the center of the model at a local origin.
	/// \param extents Vector containing all positive elements indicating the
	///				   half extents of an axis-aligned bounding box fitting the
	///				   model
	/// \param numFish Initial number of fish to be contained in the school
	/// \param maxFish Maximum number of fish that the school can contain
	/// \param position Spawn position for the school.  Fish will be created
	///					near this point, within the spawn range specified in
	///					the SchoolFlockingParams object passed into the
	///					constructor.
	/// \param schoolUniformLocation Uniform Buffer Object Index in fish shaders
	///                 that contains the per-school data.
	/// \param meshUniformLocation Uniform Index in fish shaders
	///                 that contains the per-mesh data.
	/// \param vboPolicy Type of policy to use to manage VBO(s) for instance data
	/// \param numFrames The number of frames ahead the app may render; informs
	///                  the VBO policy how many entries it will need in its ring
	///                  buffers to ensure that a buffer has finished drawing from
	///                  before re-using it.
	/// \param pVBOPool Pointer to a VBOPool object to use if the chosen VBO Policy
	///                 requires a pool to be "allocated" from.
	/// \return True if the school could be initialized with the given values,
	///			false if an error occurred.
	bool Initialize(
		uint32_t index,
		Nv::NvModelExtGL* pModel,
		uint32_t modelIndex,
		float tailStartZ,
		const nv::matrix4f& modelTransform,
		const nv::vec3f& extents,
		uint32_t numFish,
		uint32_t maxFish,
		const nv::vec3f& position,
		GLint schoolUniformLocation,
		GLint meshUniformLocation,
		Nv::VBOPolicy vboPolicy,
		uint32_t numFrames,
		Nv::NvSharedVBOGLPool* pVBOPool);

	/// Sets the VBO behavior to use, along with providing a Pool if the policy should
	/// use a shared pool, and a number of frames' worth of data to hold
	/// \param vboPolicy Enum indicating which VBO policy should be used for the school
	/// \param pVBOPool Pointer to a VBO Pool if the indicated policy requres one
	/// \param numFrames Number of frames' worth of data to be able to hold in the VBO
	void SetVBOPolicy(Nv::VBOPolicy vboPolicy, Nv::NvSharedVBOGLPool* pVBOPool, uint32_t numFrames);

	/// Resets the school's "location" to the given coordinates, respawning
	/// all fish within the school's spawn range of the new position.
	/// \param loc Coordinates around which to center the school's fish spawning.
	void ResetToLocation(const nv::vec3f& loc);

	/// Sets the current number of instances (fish) in the school
	/// \param instances Number of instances of the fish mesh to flock/render
	void SetInstanceCount(uint32_t instances);

	/// Associates the given fish model with the school and provides some information
	/// about the model to aid in animation and flocking.
	/// \param pModel Pointer to the NvModelExtGL that will be used as a template
	///               for the school's NvInstancedModelExtGL
	/// \param modelIndex Index of the model used for the school.  Used to identify the correct
	///                   model when used with MultiDrawIndirect
	/// \param tailStartZ Position along the Z-axis at which the mesh's tail starts.
	///                   Vertices with a Z value greater than this value will be
	///                   offset in the X axis by the vertex shader (in an amount
	///                   proportional to their distance along the Z-axis from this
	///                   point) to simulate tailfin movement.
	/// \param modelTransform A matrix required to transform the model from its native
	///                       coordinate system to one that has the fish facing -Z and
	///                       with +Y up and centered at the local origin.
	/// \param extents A point defined by the maximum values of a bounding box that 
	///                contain the mesh, centered at the local origin
	/// \return True if the model was able to be used as the school's template mesh.
	///         False if an error occurred.
	bool SetModel(Nv::NvModelExtGL* pModel,
		uint32_t modelIndex,
		float tailStartZ,
		const nv::matrix4f& modelTransform,
		const nv::vec3f& extents);

	/// Retrieves the Instanced model used by the school for rendering
	/// \return A pointer to the instanced model or null if no model is set
	Nv::NvInstancedModelExtGL* GetModel() { return m_pInstancedModel; }

	/// Retrieves the index into the app's array of models of the model 
	/// used by the school for rendering
	/// \return Index into the app's array of available models
	uint32_t GetModelIndex() const { return m_modelIndex; }

	/// Retrieves the matrix required to convert the model's mesh to the 
	/// app's coordinate system and center it at its local origin
	/// \return The corrective and centering matrix for the mesh
	const nv::matrix4f& GetModelMatrix() const { return m_schoolUBO_Data.m_modelMatrix; }

	/// Retrieves the point along the Z-axis that represents the beginning of
	/// the fish's tail
	/// \return Position along the Z-axis at which the mesh's tail starts.
	///         Vertices with a Z value greater than this value will be
	///         offset in the X axis by the vertex shader (in an amount
	///         proportional to their distance along the Z-axis from this
	///         point) to simulate tailfin movement.
	float GetTailStart() const { return m_schoolUBO_Data.m_tailStart; }

	/// Updates the flocking simulation and stores the new state of each fish
	/// \param frameTime Elapsed time for the current frame in seconds
	/// \param pStateManager Pointer to the state manager that holds state
	///                      information for all schools in the simulation
	/// \param avoid Flag indicating whether the fish in this school should 
	///				 attempt to avoid other schools
	void Animate(float frameTime, SchoolStateManager* pStateManager, bool avoid);

	/// Updates the instance data buffer with the current state of the flocking 
	/// simulation.
	void Update(GeometryCommandBuffer& geometryCommands);

	/// Render the school using GL commands and structures
	/// \param batchSize Number of instances rendered per draw call
	/// \return Returns the number of draw calls invoked during the Render call
	uint32_t Render(const nv::matrix4f& projView, uint32_t batchSize, GeometryCommandBuffer& geometryCommands);

	/// Set the values used by the flocking simulation that drives the school
	/// \param params A SchoolFlockingParams object initialized with the
	///				  desired settings to use for flocking behavior
	void SetFlockingParams(const SchoolFlockingParams& params)
	{
		m_flockParams = params;
	}

	/// Returns the index number of the school
	uint32_t GetIndex() const { return m_index; }

	/// Get the current values being used by the school's flocking
	/// simulation
	/// \return The structure containing all settings for the school's
	///         flocking simulation.
	SchoolFlockingParams GetFlockingParams() { return m_flockParams; }

	/// Retrieve the number of fish in the school
	/// \return Number of fish active in the school
	uint32_t GetNumFish() const { return m_instancesActive; }

	/// Retrieve the last computed centroid for the school
	/// \return The most recently computed centroid, in world space, 
	///         of the school's fish.
	const nv::vec3f& GetCentroid() const { return m_lastCentroid; }

	/// Retrieve the last computed radius for the school
	/// \return The radius, in meters, of the bounding sphere
	///         surrounding the school's fish, centered at the centroid.
	float GetRadius() const { return m_lastRadius; }

	/// Retrieves the size of the instance data for a single fish
	/// \return The size, in bytes, of the instance data for a single instance of a fish
	static uint32_t GetInstanceDataStride() { return sizeof(FishInstanceData); }

	/// Calculates a new goal for the school, abandoning any previously set goal location.
	void FindNewGoal();

	std::pair<GLuint, GLuint> GetUniformBuffer() const { return std::make_pair(m_schoolUBO_Id, m_schoolUBO_Location); }

	void SetMaterial(cb::TranslucencyType translucency, uint32_t materialId);

private:
	/// Updates the current instance data buffer with the current states of
	/// each fish in the school in preparation for rendering.
	void UpdateInstanceDataBuffer();

	static Nv::VertexFormatBinder* ms_pInstancingVertexBinder;

	/// Index of the school to identify it in the SchoolStateManager
	uint32_t m_index;

	/// Model to be instanced to represent the many fish in the school
	Nv::NvInstancedModelExtGL* m_pInstancedModel;

	/// Index of the model used by this school in the app's array of source models
	uint32_t m_modelIndex;

	/// Vertex Buffer of instance data that describes the per-fish parameters,
	/// such as position, rotation, etc.
	Nv::NvSharedVBOGL* m_pInstanceData;
	Nv::VBOPolicy m_currentVBOPolicy;

	/// Number of instances that can be held in the current instance data
	/// buffer
	uint32_t m_instancesCapacity;

	/// Current number of fish active, thus the number of instance data
	/// structures that have valid data
	uint32_t m_instancesActive;

	/// Structure that defines the layout of the data in the instance data
	/// buffer
	struct FishInstanceData
	{
		nv::vec3f m_position;
		nv::vec3f m_heading;
		float     m_tailTime;
		uint32_t  m_schoolId;
	};
	typedef std::vector<FishInstanceData> FishInstanceDataSet;
	/// Portion of the current state of the fish that will be copied to the
	/// instance data buffer
	FishInstanceDataSet m_fishInstanceStates;

	/// Uniform buffer object providing school-specific parameters to the
	/// shader
	SchoolUBO   m_schoolUBO_Data;       // Actual values for the UBO
	GLuint      m_schoolUBO_Id;         // UBO Id
	GLint       m_schoolUBO_Location;   // UBO Index in the shader

	/// Initial transform that needs to be applied to the mesh to get it into
	/// our sample's coordinate space
	nv::matrix4f m_modelTransform;

	/// Half the extents of the fish model.  Will be used to keep the fish from
	/// intersecting the ground, etc.;
	nv::vec3f m_fishHalfExtents;

	////////////////////////////////
	// Flocking Data
	////////////////////////////////

	/// Current target location which the school is moving towards
	nv::vec3f m_schoolGoal;

	/// location of the centroid of the school at the last update
	nv::vec3f m_lastCentroid;

	/// Radius of the school in the last update
	float m_lastRadius;

	SchoolFlockingParams m_flockParams;

	/// Current animation state of each fish in the school
	struct FishAnimState
	{
		float m_speed;
		float m_animTime;
		float m_animStartOffset;
	};
	typedef std::vector<FishAnimState> FishAnimStateSet;
	/// Portion of the current state of the fish that has to do
	/// with animation and won't be used in the instance data buffer
	FishAnimStateSet m_fishAnimStates;

	/// Thread safe Random number generation
	uint32_t m_rndState;
	float Random01()
	{
		m_rndState = (m_rndState * 71359) + 468029;
		return (m_rndState / (float)0xFFFFFFFF);
	}

	/// Helper functions for generating random vectors within a box
	nv::vec3f ScaledRandomVector(float scale)
	{
		return nv::vec3f(Random01() * scale, Random01() * scale,
			Random01() * scale);
	}

	nv::vec3f ScaledRandomVector(float scaleX, float scaleY, float scaleZ)
	{
		return nv::vec3f(Random01() * scaleX, Random01() * scaleY,
			Random01() * scaleZ);
	}

	nv::vec3f ScaledRandomVector(const nv::vec3f& scale)
	{
		return nv::vec3f(Random01() * scale.x, Random01() * scale.y,
			Random01() * scale.z);
	}
};

#endif // SCHOOL_H_

