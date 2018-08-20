//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/School.cpp
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
#include "School.h"
#include "SchoolStateManager.h"
#include "NV/NvQuaternion.h"

#include "NvInstancedModelExtGL.h"

Nv::VertexFormatBinder* School::ms_pInstancingVertexBinder = nullptr;

Nv::VertexFormatBinder* School::GetInstancingVertexBinder()
{
    if (ms_pInstancingVertexBinder == nullptr)
    {
        Nv::VertexFormatBinder* pNewBinder = new Nv::VertexFormatBinder();
        // Instancing Input (as defined in staticfis_VS.glsl)
        //layout(location = 7) in vec3 a_vInstancePos;
        //layout(location = 8) in vec3 a_vInstanceHeading;
        //layout(location = 9) in float a_fInstanceAnimTime;
        //layout(location = 10) in int a_iSchoolId;

        pNewBinder->AddInstanceAttrib(7, 3, GL_FLOAT, GL_FLOAT, GL_FALSE, 1, 0);
        pNewBinder->AddInstanceAttrib(8, 3, GL_FLOAT, GL_FLOAT, GL_FALSE, 1, 12);
        pNewBinder->AddInstanceAttrib(9, 1, GL_FLOAT, GL_FLOAT, GL_FALSE, 1, 24);
        pNewBinder->AddInstanceAttrib(10, 1, GL_UNSIGNED_INT, GL_INT, GL_FALSE, 1, 28);
        pNewBinder->SetStride(sizeof(FishInstanceData));

        ms_pInstancingVertexBinder = pNewBinder;
    }
    return ms_pInstancingVertexBinder;
}

School::School()
    : m_pInstancedModel(nullptr)
    , m_pInstanceData(nullptr)
    , m_currentVBOPolicy(Nv::VBO_INVALID)
    , m_instancesCapacity(0)
    , m_instancesActive(0)
    , m_schoolUBO_Id(0)
    , m_schoolUBO_Location(0)
    , m_fishHalfExtents(0.0f, 0.0f, 0.0f)
    , m_schoolGoal(0.0f, 0.0f, 0.0f)
    , m_lastCentroid(0.0f, 0.0f, 0.0f)
    , m_lastRadius(0.0f)
{}

School::School(const SchoolFlockingParams& params)
    : m_pInstancedModel(nullptr)
    , m_pInstanceData(nullptr)
    , m_currentVBOPolicy(Nv::VBO_INVALID)
    , m_instancesCapacity(0)
    , m_instancesActive(0)
    , m_schoolUBO_Id(0)
    , m_schoolUBO_Location(0)
    , m_flockParams(params)
    , m_fishHalfExtents(0.0f, 0.0f, 0.0f)
    , m_schoolGoal(0.0f, 0.0f, 0.0f)
    , m_lastCentroid(0.0f, 0.0f, 0.0f)
    , m_lastRadius(0.0f)
{}

School::~School()
{
    m_pInstancedModel = nullptr;
    if (nullptr != m_pInstanceData)
    {
        m_pInstanceData->Finalize();
        delete m_pInstanceData;
    }
    glDeleteBuffers(1, &m_schoolUBO_Id);
}

bool School::Initialize(uint32_t index,
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
                        Nv::NvSharedVBOGLPool* pVBOPool)
{
    m_index = index;
    m_modelIndex = modelIndex;

    // Initially our number of active fish will be the same as our capacity
    m_instancesActive = numFish;
    m_instancesCapacity = maxFish;

    SetVBOPolicy(vboPolicy, pVBOPool, numFrames);

    glGenBuffers(1, &m_schoolUBO_Id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_schoolUBO_Id);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SchoolUBO), NULL, GL_STATIC_DRAW);
    m_schoolUBO_Location = schoolUniformLocation;

    // Set up data defining which model to instance from
    SetModel(pModel, modelIndex, tailStartZ, modelTransform, extents);

    // Seed our random number generator for each different model of fish and
    // school
    m_rndState =
        (uint32_t)(((uint64_t)m_pInstancedModel >> 4) * ((uint64_t)this >> 4));

    // Set our initial position to our initial goal.  This allows us 
    // to choose a starting position but also generate a new goal position
    // on the first update.
    m_schoolGoal = ScaledRandomVector(m_flockParams.m_spawnZoneMax 
                                      - m_flockParams.m_spawnZoneMin) 
                                      + m_flockParams.m_spawnZoneMin;
    /*LOGI("School Initialize: Position=(%f, %f, %f); Goal=(%f, %f, %f)",
        position.x, position.y, position.z,
        m_schoolGoal.x, m_schoolGoal.y, m_schoolGoal.z);*/

    // Initialize the fish book-keeping structures
    m_fishAnimStates.resize(m_instancesCapacity);
    m_fishInstanceStates.resize(m_instancesCapacity);

    nv::vec3f centroid(0.0f, 0.0f, 0.0f);

    for (uint32_t fishIndex = 0; fishIndex < m_instancesActive; ++fishIndex)
    {
        FishInstanceData& fishInstData = m_fishInstanceStates[fishIndex];

        fishInstData.m_position = ScaledRandomVector(m_flockParams.m_spawnRange);
        fishInstData.m_position += position;
        if (fishInstData.m_position.y < m_fishHalfExtents.y)
        {
            fishInstData.m_position.y = m_fishHalfExtents.y;
        }
        fishInstData.m_heading = nv::vec3f(0.0f, 0.0f, 1.0f);
        fishInstData.m_tailTime = 0.0f;
        fishInstData.m_schoolId = m_index;

        FishAnimState& fishAnimState = m_fishAnimStates[fishIndex];
        fishAnimState.m_speed = 0;
        fishAnimState.m_animTime = 0.0f;
        fishAnimState.m_animStartOffset = Random01() * NV_PI * 2.0f;
        centroid += fishInstData.m_position;
    }
    if (m_instancesActive > 0)
    {
        m_lastCentroid = centroid / m_instancesActive;
    }
    else
    {
        m_lastCentroid = centroid;
    }

    return true;
}

void School::SetVBOPolicy(Nv::VBOPolicy vboPolicy, Nv::NvSharedVBOGLPool* pVBOPool, uint32_t numFrames)
{
    // If we already had a VBO, we need to release it before setting a policy
    // that will create a new one
    if (nullptr != m_pInstanceData)
    {
        m_pInstanceData->Finalize();
    }

    bool persistentMapping = false;
    switch (vboPolicy)
    {
    case Nv::VBO_SUBRANGE:
    {
        m_pInstanceData = new Nv::NvSharedVBOGL_MappedSubRanges();
        break;
    }
    case Nv::VBO_SUBRANGE_PERSISTENT:
    {
        persistentMapping = true;
        m_pInstanceData = new Nv::NvSharedVBOGL_MappedSubRanges();
        break;
    }
    case Nv::VBO_ORPHANED:
    {
        m_pInstanceData = new Nv::NvSharedVBOGL_Orphaning();
        break;
    }
    case Nv::VBO_POOLED:
    {
        Nv::NvSharedVBOGL_PooledSubRange* pSubRange = new Nv::NvSharedVBOGL_PooledSubRange();
        pSubRange->SetPool(pVBOPool, m_index);
        m_pInstanceData = pSubRange;
        break;
    }
    case Nv::VBO_POOLED_PERSISTENT:
    {
        persistentMapping = true;
        Nv::NvSharedVBOGL_PooledSubRange* pSubRange = new Nv::NvSharedVBOGL_PooledSubRange();
        pSubRange->SetPool(pVBOPool, m_index);
        m_pInstanceData = pSubRange;
        break;
    }
    default:
    {
        m_pInstanceData = nullptr;
        break;
    }
    }

    if (nullptr == m_pInstanceData)
    {
        m_currentVBOPolicy = Nv::VBO_INVALID;
        return;
    }
    m_currentVBOPolicy = vboPolicy;

    if (!m_pInstanceData->Initialize(sizeof(FishInstanceData) * m_instancesCapacity, numFrames, persistentMapping))
    {
        delete m_pInstanceData;
        m_pInstanceData = nullptr;
    }

    // Attach our instance data to our model, as well
    if (nullptr != m_pInstancedModel)
    {
        m_pInstancedModel->EnableInstancing(GetInstancingVertexBinder(), m_pInstanceData);
    }
}

void School::ResetToLocation(const nv::vec3f& loc)
{
    for (uint32_t fishIndex = 0; fishIndex < m_instancesActive; ++fishIndex)
    {
        FishInstanceData& fishInstData = m_fishInstanceStates[fishIndex];
        
        fishInstData.m_position = ScaledRandomVector(m_flockParams.m_spawnRange);
        fishInstData.m_position += loc;
        fishInstData.m_heading = ScaledRandomVector(1.0f);
        fishInstData.m_tailTime = 0.0f;
        fishInstData.m_schoolId = m_index;
    
        FishAnimState& fishAnimState = m_fishAnimStates[fishIndex];
        fishAnimState.m_speed = 0;
    }
    m_lastCentroid = loc;
    m_schoolGoal = loc;
}

void School::SetInstanceCount(uint32_t instances)
{
    m_instancesActive = instances; 
    if (nullptr != m_pInstancedModel)
    {
        m_pInstancedModel->SetInstanceCount(instances);
    }
}

bool School::SetModel(Nv::NvModelExtGL* pModel, 
                      uint32_t modelIndex,
                      float tailStartZ, 
                      const nv::matrix4f& modelTransform, 
                      const nv::vec3f& extents)
{
    m_modelIndex = modelIndex;

    if (nullptr != m_pInstancedModel)
    {
        delete m_pInstancedModel;
    }

    // Create the instanced version of our mesh, using it to tie together our
    // shared model and our instance data
    m_pInstancedModel = Nv::NvInstancedModelExtGL::Create(m_instancesCapacity, pModel);
    if (nullptr == m_pInstancedModel)
    {
        return false;
    }
    m_pInstancedModel->EnableInstancing(GetInstancingVertexBinder(), m_pInstanceData);
    m_pInstancedModel->GetModel()->UpdateBoneTransforms();

    const nv::matrix4f& offset = GetModel()->GetModel()->GetMesh(0)->GetMeshOffset();

    m_modelTransform = modelTransform * offset;

    // Store off our half extents to use in our update to detect ground collision.
    m_fishHalfExtents = extents;

    // Update the school's UBO
    m_schoolUBO_Data.m_modelMatrix = m_modelTransform;
    m_schoolUBO_Data.m_tailStart = tailStartZ;
    glBindBuffer(GL_UNIFORM_BUFFER, m_schoolUBO_Id);
    // Orphan our previous School Uniform buffer
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SchoolUBO), nullptr, GL_STATIC_DRAW); 
    // Allocate a new School Uniform buffer, initializing it with the data for the new model
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SchoolUBO), &m_schoolUBO_Data, GL_STATIC_DRAW);  

    m_pInstancedModel->GetModel()->UpdateBoneTransforms();

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return true;
}

uint32_t neighborOffset = 0;
uint32_t neighborSkip = 1;

void School::Animate(float frameTime, SchoolStateManager* pStateManager, bool avoid)
{
    // We need to calculate a new centroid
    nv::vec3f newCentroid = nv::vec3f(0.0f, 0.0f, 0.0f);
    // ...and approximate radius
    float newRadius2 = 0.0f;
    
    float frameInertia = m_flockParams.m_inertia;
    const float targetFrameTime = 1.0f / 100.0f;
    if (frameTime < targetFrameTime) 
    {
        frameInertia *= sqrt(targetFrameTime / frameTime);
    }

    // Check to see if the school is close enough to its goal to start moving
    // to a new one
    float dist2goalSqr = nv::square_norm(m_schoolGoal - m_lastCentroid);
    if (dist2goalSqr <
        (m_flockParams.m_arrivalDistance * m_flockParams.m_arrivalDistance))
    {
        // Fish have arrived, choose new goal
        FindNewGoal();
    }

    float minAvoidanceSpeed = m_flockParams.m_maxSpeed * 0.5f;
    float maxAvoidanceSpeed = m_flockParams.m_maxSpeed * 2.0f;

    // For speed, use a squared distance when checking for neighbor status
    float neighborDistance2 =
        m_flockParams.m_neighborDistance * m_flockParams.m_neighborDistance;

    uint32_t numSchools = pStateManager->GetNumReadStates();
    SchoolState* pSchools = pStateManager->GetReadStates();

    // We will avoid, at most, 8 other schools
    const uint32_t cMaxSchoolsToAvoid = 8;
    uint32_t schoolsToAvoid[cMaxSchoolsToAvoid];
    uint32_t numSchoolsToAvoid = 0;
    
    SchoolState* pSchool = pSchools;
    if (avoid) 
    {
        // Find schools that are at least as aggressive as our school, and overlap our school
        for (uint32_t schoolIndex = 0; (schoolIndex < numSchools) && (numSchoolsToAvoid < cMaxSchoolsToAvoid); ++schoolIndex, ++pSchool)
        {
            if (schoolIndex == m_index)
            {
                // Don't avoid ourself
                continue;
            }

            // Should we avoid the school?
            if (pSchool->m_aggression < m_flockParams.m_aggression)
            {
                // No.
                continue;
            }

            nv::vec3f toSchool = pSchool->m_center - m_lastCentroid;
            float schoolDist2 = nv::square_norm(toSchool);
            float sumRadii = pSchool->m_radius + m_lastRadius;  // Add some buffer distance of avoidance?

            if (schoolDist2 < (sumRadii * sumRadii))
            {
                schoolsToAvoid[numSchoolsToAvoid] = schoolIndex;
                ++numSchoolsToAvoid;
            }
        }
    }

    int32_t clampedNeighborOffset = neighborOffset % m_instancesActive;

    for (uint32_t fishIndex = 0; fishIndex < m_instancesActive; ++fishIndex)
    {
        FishInstanceData& fishInstData = m_fishInstanceStates[fishIndex];
        nv::vec3f goalHeading = nv::normalize(m_schoolGoal - fishInstData.m_position);
        nv::vec3f alignmentSum(0.0f, 0.0f, 0.0f);
        uint32_t alignmentCount = 0;
        nv::vec3f repulsionSum(0.0f, 0.0f, 0.0f);
        uint32_t repulsionCount = 0;
        uint32_t accelerateCount = 0;
        uint32_t decelerateCount = 0;
        nv::vec3f cohesionSum(0.0f, 0.0f, 0.0f);
        uint32_t cohesionCount = 0;

        uint32_t neighborIndex = clampedNeighborOffset;
        uint32_t skip = 6 - neighborSkip;
        for (; neighborIndex < m_instancesActive; neighborIndex += skip)
        {
            if (neighborIndex == fishIndex) continue;
            FishInstanceData& neighbor = m_fishInstanceStates[neighborIndex];
            nv::vec3f deltaPos = neighbor.m_position - fishInstData.m_position;
            float dist2 = nv::dot(deltaPos, deltaPos);
            if (dist2 <= neighborDistance2)
            {
                // It's too close, so we need to avoid
                float dist = sqrt(dist2);

                // If we're so close that we can't get a good normalized direction, then speed up or slow
                // down.  Just to make sure they don't choose to do the same thing, we'll use the
                // order of their indices to choose which to do.
                if (dist < 0.001f)
                {
                    // The first one will accelerate
                    if (fishIndex > neighborIndex)
                    {
                        ++decelerateCount;
                    }
                    else
                    {
                        ++accelerateCount;
                    }
                }
                else
                {
                    // If our neighbor is to the side of use, we'll 
                    // use repulsion to move away.  If he's in front of
                    // or behind us, we'll decelerate or accelerate to
                    // move away, respectively.
                    nv::vec3f deltaNorm = deltaPos / dist;
                    float dotPosition = nv::dot(fishInstData.m_heading, deltaNorm);
                    if (dotPosition > 0.95f)
                    {
                        // Neighbor is forward
                        ++decelerateCount;
                    }
                    else if (dotPosition < -0.95f)
                    {
                        // Neighbor is behind
                        ++accelerateCount;
                    }
                    else
                    {
                        // Neighbor is to side
                        repulsionSum -= deltaNorm;
                        ++repulsionCount;
                    }
                }
            }
            else if (dist2 > (neighborDistance2 * 2.0f))
            {
                // It's far enough away that we should head towards him
                cohesionSum += deltaPos;
                ++cohesionCount;
            }
            else
            {
                // It's far enough to not need to avoid but close enough to not need to swim
                // towards, so just try to go the same direction
                alignmentSum += neighbor.m_heading;
                ++alignmentCount;
            }
        }
        nv::vec3f avoidanceVec(0.0f, 0.0f, 0.0f);
        if (avoid) {
            if (numSchoolsToAvoid > 0)
            {
                for (uint32_t avoidSchoolIndex = 0; avoidSchoolIndex < numSchoolsToAvoid; ++avoidSchoolIndex)
                {
                    // Get a pointer to the school state for the school we're avoiding
                    pSchool = pSchools + schoolsToAvoid[avoidSchoolIndex];

                    nv::vec3f fromSchool = fishInstData.m_position - pSchool->m_center;
                    float schoolDist2 = nv::square_norm(fromSchool);
                    if (schoolDist2 < 0.0001f)
                    {
                        // Way too close
                        avoidanceVec += nv::vec3f(0.0f, 1.0f, 0.0f);
                    }
                    else if (schoolDist2 <= (pSchool->m_radius * pSchool->m_radius))
                    {
                        avoidanceVec += nv::normalize(fromSchool / sqrt(schoolDist2)) * (pSchool->m_aggression - m_flockParams.m_aggression + 0.1f);
                    }
                }
                avoidanceVec /= numSchoolsToAvoid;
            }
        }
        // Add the distance between our last position and our last centroid, squared, to the
        // calculation for the new radius squared
        float distFromCentroid2 = nv::square_norm(fishInstData.m_position - m_lastCentroid);
        newRadius2 += distFromCentroid2;

        // Update our position with our current heading and speed before
        // calculating new values
        FishAnimState& fishAnimState = m_fishAnimStates[fishIndex];
        fishInstData.m_position += fishAnimState.m_speed * frameTime * fishInstData.m_heading;
        if (fishInstData.m_position.y < m_fishHalfExtents.y)
        {
            fishInstData.m_position.y = m_fishHalfExtents.y;
        }

        // Combine all of our "forces" into our new driving vector
        nv::vec3f desiredHeading = (goalHeading * m_flockParams.m_goalScale);
        // Repulsion overrides everything
        if (repulsionCount > 0)
        {
            desiredHeading += repulsionSum * m_flockParams.m_repulsionScale;
        }
        else
        {
            // Alignment and cohesion can work together
            desiredHeading += nv::normalize(alignmentSum) * m_flockParams.m_alignmentScale;
            desiredHeading += nv::normalize(cohesionSum) * m_flockParams.m_cohesionScale;
        }

        if (avoid) {
            // Always try to avoid other schools, if necessary
            desiredHeading += avoidanceVec * m_flockParams.m_schoolAvoidanceScale;
        }

        // Modify our current heading by the new influences
        if (frameInertia <= 0.0f)
        {
            fishInstData.m_heading = nv::normalize(desiredHeading);
        }
        else
        {
            fishInstData.m_heading = nv::normalize(fishInstData.m_heading + (desiredHeading / frameInertia));
        }

        // Headings too close to the vertical axis will cause rotational instability
        // in the shader, and possibly collapse of the reconstructed transform.
        // check to see if we're too close to the vertical, and if so, then try to
        // move in a reasonable direction away from it
        float vertical = nv::dot(fishInstData.m_heading, nv::vec3f(0.0f, 1.0f, 0.0f));
        if (vertical > 0.999f || vertical < -0.999f)
        {
            // Try biasing our heading toward the goal.  
            fishInstData.m_heading = nv::normalize(fishInstData.m_heading + (goalHeading * 0.2f));

            // Test the new vector
            vertical = nv::dot(fishInstData.m_heading, nv::vec3f(0.0f, 1.0f, 0.0f));
            if (vertical > 0.999f || vertical < -0.999f)
            {
                // also head away from the centroid.

                fishInstData.m_heading = nv::normalize(fishInstData.m_heading +
                    (nv::normalize(fishInstData.m_position - m_lastCentroid) * 0.2f));

                // Test the new vector
                vertical = nv::dot(fishInstData.m_heading, nv::vec3f(0.0f, 1.0f, 0.0f));
                if (vertical > 0.999f || vertical < -0.999f)
                {
                    // Still not good? Bias it by a horizontal axis.  It won't look good, but
                    // it's better than nothing.
                    fishInstData.m_heading = nv::normalize(
                        fishInstData.m_heading + nv::vec3f(0.0f, 0.0f, -0.4f));
                }
            }
        }

        // Accelerate if necessary
        // Assume that acceleration == speed/second
        // (i.e. fish can go from 0 to max speed in 1 second)
        if (decelerateCount > accelerateCount)
        {
            // Decelerate to avoid fish in front
            fishAnimState.m_speed -= m_flockParams.m_maxSpeed * frameTime;
            if (fishAnimState.m_speed < minAvoidanceSpeed)
            {
                fishAnimState.m_speed = minAvoidanceSpeed;
            }
        }
        else if (accelerateCount > 0)
        {
            // Assume that speed == acceleration 
            // (i.e. fish can go from 0 to max speed in 1 second)
            fishAnimState.m_speed += m_flockParams.m_maxSpeed * frameTime;
            if (fishAnimState.m_speed > maxAvoidanceSpeed)
            {
                fishAnimState.m_speed = maxAvoidanceSpeed;
            }
        }
        else if (fishAnimState.m_speed < m_flockParams.m_maxSpeed)
        {
            // Accelerate to maximum cruising speed
            fishAnimState.m_speed += m_flockParams.m_maxSpeed * frameTime;
            if (fishAnimState.m_speed > m_flockParams.m_maxSpeed)
            {
                fishAnimState.m_speed = m_flockParams.m_maxSpeed;
            }
        }
        else if (fishAnimState.m_speed > m_flockParams.m_maxSpeed)
        {
            // Decelerate to maximum cruising speed
            fishAnimState.m_speed -= m_flockParams.m_maxSpeed * frameTime;
            if (fishAnimState.m_speed < m_flockParams.m_maxSpeed)
            {
                fishAnimState.m_speed = m_flockParams.m_maxSpeed;
            }
        }

        // Increase our animation time, using our current speed to make the 
        // tail move at a reasonable rate
        fishAnimState.m_animTime += frameTime * fishAnimState.m_speed * fishAnimState.m_speed;
        fishInstData.m_tailTime = fishAnimState.m_animStartOffset + fishAnimState.m_animTime;

        // Add our new position to the calculation of the school centroid for
        // this frame
        newCentroid += fishInstData.m_position;
    }
    // Update our centroid based on the school's fish positions
    m_lastCentroid = newCentroid / m_instancesActive;

    // Give a bit of a buffer (20%) to the average radius to account for most 
    // of the school, but still ignore the outliers
    m_lastRadius = sqrt(newRadius2 / m_instancesActive) * 1.2f; 

    if (avoid) 
    {
        // Write our current state to the SchoolStateManager
        NV_ASSERT(m_index < pStateManager->GetNumWriteStates());

        SchoolState* pOurState = pStateManager->GetWriteStates() + m_index;
        pOurState->m_aggression = m_flockParams.m_aggression;
        pOurState->m_center = m_lastCentroid;
        pOurState->m_radius = m_lastRadius;
    }

    // If we are using a pooled VBO, then it is already mapped, so we can go ahead and copy into it in this thread
    if ((m_currentVBOPolicy == Nv::VBO_POOLED) || (m_currentVBOPolicy == Nv::VBO_POOLED_PERSISTENT))
    {
        UpdateInstanceDataBuffer();
    }
}

void School::Update()
{
    UpdateInstanceDataBuffer();
}

void School::UpdateInstanceDataBuffer()
{
    if (!m_pInstanceData->BeginUpdate())
    {
        return;
    }

    FishInstanceData* pCurrInstance =
        (FishInstanceData*)m_pInstanceData->GetData();
    if (nullptr == pCurrInstance)
    {
        m_pInstanceData->EndUpdate();
        return;
    }
    memcpy(pCurrInstance, &m_fishInstanceStates[0], sizeof(FishInstanceData) * m_instancesActive);
    m_pInstanceData->EndUpdate();
}

uint32_t School::Render(uint32_t batchSize)
{
    uint32_t drawCallCount = 0;

    if (nullptr == m_pInstancedModel)
    {
        return drawCallCount;
    }

    // Activate our Uniform buffers
    glBindBufferBase(GL_UNIFORM_BUFFER, m_schoolUBO_Location, m_schoolUBO_Id);

    m_pInstancedModel->SetBatchSize(batchSize);
    drawCallCount += m_pInstancedModel->Render(0, 1, 2);
    m_pInstanceData->DoneRendering();
    return drawCallCount;
}

void School::FindNewGoal()
{
    m_schoolGoal = ScaledRandomVector(m_flockParams.m_spawnZoneMax
        - m_flockParams.m_spawnZoneMin)
        + m_flockParams.m_spawnZoneMin;
    /*LOGI("School Goal: (%f, %f, %f)",
    m_schoolGoal.x, m_schoolGoal.y, m_schoolGoal.z);*/
}
