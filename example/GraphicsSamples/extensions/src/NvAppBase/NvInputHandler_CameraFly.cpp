//----------------------------------------------------------------------------------
// File:        NvAppBase/NvInputHandler_CameraFly.cpp
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
#include "NvAppBase/NvInputHandler_CameraFly.h"
#include "NV/NvLogs.h"

// Not used currently , but possibly handy
nv::quaternion<float> QuaternionFromPitchYaw(float pitch, float yaw)
{
    float sinPitch = sin(pitch * 0.5f);
    float cosPitch = cos(pitch * 0.5f);

    float sinYaw = sin(yaw * 0.5f);
    float cosYaw = cos(yaw * 0.5f);

    // Pitch * Yaw
    return nv::quaternion<float>(sinPitch * cosYaw, cosPitch * sinYaw, sinPitch * sinYaw, cosPitch * cosYaw);

    // Yaw * Pitch
    //return nv::quaternion<float>(sinYaw * cosPitch, cosYaw * sinPitch, sinYaw * sinPitch, cosYaw * cosPitch);
}

NvInputHandler_CameraFly::NvInputHandler_CameraFly() 
{
    reset();
}

NvInputHandler_CameraFly::~NvInputHandler_CameraFly()
{
}

void NvInputHandler_CameraFly::reset()
{
    m_currentPos = nv::vec3f(0.0f, 0.0f, 0.0f);
    m_currentPitch = 0.0f;
    m_currentYaw = 0.0f;
    m_touchPointsCount = 0;
    m_lastInput = nv::vec2f(0.0f, 0.0f);
    m_lastMultitouchDist = 0;
    m_rotSpeed_Mouse = 0.001f;
    m_transSpeed_Mouse = 0.01f;
    m_xRotDelta_Mouse = 0.0f;
    m_yRotDelta_Mouse = 0.0f;
    m_zRotDelta_Mouse = 0.0f;
    m_xTransDelta_Mouse = 0.0f;
    m_yTransDelta_Mouse = 0.0f;
    m_zTransDelta_Mouse = 0.0f;
    m_rotSpeed_KB = 0.5f;
    m_transSpeed_KB = 1.0f;
    m_accelerate_KB = false;
    m_xPlusRotVel_KB = 0.0f;
    m_yPlusRotVel_KB = 0.0f;
    m_zPlusRotVel_KB = 0.0f;
    m_xNegRotVel_KB = 0.0f;
    m_yNegRotVel_KB = 0.0f;
    m_zNegRotVel_KB = 0.0f;
    m_yPlusTransVel_KB = 0.0f;
    m_xPlusTransVel_KB = 0.0f;
    m_zPlusTransVel_KB = 0.0f;
    m_yNegTransVel_KB = 0.0f;
    m_xNegTransVel_KB = 0.0f;
    m_zNegTransVel_KB = 0.0f;
    m_rotSpeed_GP = 1.0f;
    m_transSpeed_GP = 1.0f;
    m_accelerate_GP = false;
    m_xRotVel_GP = 0.0f;
    m_yRotVel_GP = 0.0f;
    m_zRotVel_GP = 0.0f;
    m_xTransVel_GP = 0.0f;
    m_yTransVel_GP = 0.0f;
    m_zTransVel_GP = 0.0f;
}

void NvInputHandler_CameraFly::update(float deltaTime)
{
    // Calculate the deltas for each access to create our new relative transform
    float yaw = (m_yRotDelta_Mouse * m_rotSpeed_Mouse) + ((m_yPlusRotVel_KB - m_yNegRotVel_KB) * m_rotSpeed_KB * deltaTime) + (m_yRotVel_GP * m_rotSpeed_GP * deltaTime);
    float pitch = (m_xRotDelta_Mouse * m_rotSpeed_Mouse) + ((m_xPlusRotVel_KB - m_xNegRotVel_KB) * m_rotSpeed_KB * deltaTime) + (m_xRotVel_GP * m_rotSpeed_GP * deltaTime);

    float kbAccelerate = m_accelerate_KB? 5.0f : 1.0f;
    float gpAccelerate = m_accelerate_GP? 5.0f : 1.0f;
    float xDelta = (m_xTransDelta_Mouse * m_transSpeed_Mouse) + ((m_xPlusTransVel_KB - m_xNegTransVel_KB) * m_transSpeed_KB * kbAccelerate * deltaTime) + (m_xTransVel_GP * m_transSpeed_GP * gpAccelerate * deltaTime);
    float yDelta = (m_yTransDelta_Mouse * m_transSpeed_Mouse) + ((m_yPlusTransVel_KB - m_yNegTransVel_KB) * m_transSpeed_KB * kbAccelerate * deltaTime) + (m_yTransVel_GP * m_transSpeed_GP * gpAccelerate * deltaTime);
    float zDelta = (m_zTransDelta_Mouse * m_transSpeed_Mouse) + ((m_zPlusTransVel_KB - m_zNegTransVel_KB) * m_transSpeed_KB * kbAccelerate * deltaTime) + (m_zTransVel_GP * m_transSpeed_GP * gpAccelerate * deltaTime);

    // Translation provided by input is relative to the camera. 
    // Convert to world space translation before adding it.
    nv::matrix4f currentRotation = m_currentCameraMat;
    currentRotation.set_translate(nv::vec3f(0.0f, 0.0f, 0.0f));

    nv::vec4f worldTranslation = currentRotation * nv::vec4f(xDelta, yDelta, zDelta, 1.0f);

    m_currentPos.x += worldTranslation.x;
    m_currentPos.y += worldTranslation.y;
    m_currentPos.z += worldTranslation.z;

    // Update our current yaw and pitch, clamping as needed
    m_currentYaw += yaw;
    float twopi = (NV_PI * 2.0f);
    while (m_currentYaw > twopi)
    {
        m_currentYaw -= twopi;
    }

    m_currentPitch += pitch;
    if (m_currentPitch > NV_PI)
    {
        m_currentPitch = NV_PI;
    }
    else if (m_currentPitch < -NV_PI)
    {
        m_currentPitch = -NV_PI;
    }

    // Clear out the accumulated mouse values
    m_xRotDelta_Mouse = m_yRotDelta_Mouse = m_zRotDelta_Mouse = 0.0f;
    m_xTransDelta_Mouse = m_yTransDelta_Mouse = m_zTransDelta_Mouse = 0.0f;

    updateMatrices();
}

void NvInputHandler_CameraFly::updateMatrices()
{
    // Build our current camera matrix
    nv::matrix4f yawMat;
    nv::rotationY(yawMat, m_currentYaw);

    nv::matrix4f pitchMat;
    nv::rotationX(pitchMat, m_currentPitch);

    m_currentCameraMat = yawMat * pitchMat;
    m_currentCameraMat.set_translate(m_currentPos);

    // update our current view matrix
    m_currentViewMat = nv::inverse(m_currentCameraMat);
}

bool NvInputHandler_CameraFly::processPointer(NvInputDeviceType::Enum device, NvPointerActionType::Enum action,
        uint32_t modifiers, int32_t count, NvPointerEvent* points)
{
    uint32_t button = points[0].m_id;
    bool needsUpdate = false;
    nv::vec2f deltaMotion(0.0f, 0.0f);
    float pinchDelta = 0.0f;

    switch (action)
    {
    case NvPointerActionType::UP:
    {
        m_touchPointsCount = 0;
        break;
    } 
    case NvPointerActionType::MOTION:
    {
        if (m_touchPointsCount == 1)
        {
            // Single point, so update the delta...
            deltaMotion.x = points[0].m_x - m_lastInput.x;
            deltaMotion.y = points[0].m_y - m_lastInput.y;

            // ...and store the last position
            m_lastInput = nv::vec2f(points[0].m_x, points[0].m_y);
            needsUpdate = true;
        }
        else if ((m_touchPointsCount >= 2) && (count >= 2))
        { 
            // >1 points during this 'gesture'
            // Capture the pinch distance
            nv::vec2f multitouchDelta = nv::vec2f(points[0].m_x - points[1].m_x, points[0].m_y - points[1].m_y);
            float pinchDist = nv::length(multitouchDelta);
            pinchDelta = pinchDist - m_lastMultitouchDist;
            m_lastMultitouchDist = pinchDist;

            // Calculate the center of the pinch to get the motion delta
            nv::vec2f newCenter = nv::vec2f(points[0].m_x + points[1].m_x, points[0].m_y + points[1].m_y) * 0.5f;
            deltaMotion = newCenter - m_lastInput;
            m_lastInput = newCenter;

            needsUpdate = true;
        }
        break;
    }
    case NvPointerActionType::DOWN:
    {
        // Beginning single touch gesture
        m_touchPointsCount = 1;
        m_lastInput = nv::vec2f(points[0].m_x, points[0].m_y);
        m_lastMultitouchDist = 0.0f;
        break;
    } 
    case  NvPointerActionType::EXTRA_DOWN:
    {
        // Beginning multi touch gesture, track number of touches
        m_touchPointsCount = (uint8_t)count;

        m_lastInput = nv::vec2f(points[0].m_x + points[1].m_x, points[0].m_y + points[1].m_y) * 0.5f;

        // Cache initial length of pinch, also the current length
        nv::vec2f touchDelta = nv::vec2f(points[0].m_x - points[1].m_x, points[0].m_y - points[1].m_y);
        m_lastMultitouchDist = nv::length(touchDelta);

        break;
    }
    case NvPointerActionType::EXTRA_UP:
    {
        if (count == 1)
        {
            // We're dropping out of multi-touch gesture, so we need
            // to reinitialize our gesture start values, otherwise there
            // could be a jump when the next to last touch is removed
            m_lastInput = nv::vec2f(points[0].m_x, points[0].m_y);
            m_lastMultitouchDist = 0.0f;
        }
        m_touchPointsCount = (uint8_t)count;
        break;
    }
    }

    if (needsUpdate)
    {
        if (m_touchPointsCount == 1)
        {
            if (device == NvInputDeviceType::MOUSE)
            {
                if (button & NvMouseButton::LEFT)
                {
                    // Rotating in yaw(mouseX), pitch(mouseY)
                    m_yRotDelta_Mouse -= deltaMotion.x;
                    m_xRotDelta_Mouse -= deltaMotion.y;
                }
                else if (button & NvMouseButton::RIGHT)
                {
                    // Moving in x(mouseX), y(mouseY)
                    m_xTransDelta_Mouse -= deltaMotion.x;
                    m_yTransDelta_Mouse += deltaMotion.y;
                }
                else if (button & NvMouseButton::MIDDLE)
                {
                    // Moving in x(mouseX), z(mouseY)
                    m_xTransDelta_Mouse += deltaMotion.x;
                    m_zTransDelta_Mouse += deltaMotion.y;
                }
            }
            else
            {
                // Touch with a single point
                // Rotating in yaw(touchX), pitch(touchY)
                m_yRotDelta_Mouse += deltaMotion.x;
                m_xRotDelta_Mouse += deltaMotion.y;
            }
        }
        else
        {
            // Touch with multiple points
            // Translating in z(pinch)
            m_zTransDelta_Mouse -= pinchDelta;

            // Translating in x(touchX), y(touchY)
            m_xTransDelta_Mouse -= deltaMotion.x;
            m_yTransDelta_Mouse += deltaMotion.y;
        }
    }
    return true;
}

bool NvInputHandler_CameraFly::processGamepad(uint32_t changedPadFlags, NvGamepad& pad)
{
    NvGamepad::State state;
    int32_t i;

    for (i = 0; i < NvGamepad::MAX_GAMEPADS; i++)
    {
        uint32_t currPadFlag = 1 << i;
        if ((changedPadFlags & currPadFlag) == currPadFlag)
        {
            pad.getState(i, state);
            m_yRotVel_GP = -state.mThumbRX;
            m_xRotVel_GP = -state.mThumbRY;

            m_zTransVel_GP = state.mThumbLY;
			m_xTransVel_GP = state.mThumbLX;
			m_yTransVel_GP = state.mRightTrigger - state.mLeftTrigger;
            m_accelerate_GP = (state.mButtons & NvGamepad::BUTTON_RIGHT_SHOULDER) != 0;
			return true;
        }
    }
    return true;
}

bool NvInputHandler_CameraFly::processKey(uint32_t code, NvKeyActionType::Enum action)
{
    // FPS mode uses WASD for motion, so that UI can own arrows for focus control.
    switch(code) 
    {
    case NvKey::K_W:
    {
        if (action == NvKeyActionType::UP)
        {
            if (m_zNegTransVel_KB > 0)
            {
                --m_zNegTransVel_KB;
            }
        }
        else if (action == NvKeyActionType::DOWN)
        {
            ++m_zNegTransVel_KB;
        }
        return true;
    }
    case NvKey::K_S:
    {
        if (action == NvKeyActionType::UP)
        {
            if (m_zPlusTransVel_KB > 0)
            {
                --m_zPlusTransVel_KB;
            }
        }
        else if (action == NvKeyActionType::DOWN)
        {
            ++m_zPlusTransVel_KB;
        }
        return true;
    }
    case NvKey::K_A:
    {
        if (action == NvKeyActionType::UP)
        {
            if (m_xNegTransVel_KB > 0)
            {
                --m_xNegTransVel_KB;
            }
        }
        else if (action == NvKeyActionType::DOWN)
        {
            ++m_xNegTransVel_KB;
        }
        return true;
    }
    case NvKey::K_D:
    {
        if (action == NvKeyActionType::UP)
        {
            if (m_xPlusTransVel_KB > 0)
            {
                --m_xPlusTransVel_KB;
            }
        }
        else if (action == NvKeyActionType::DOWN)
        {
            ++m_xPlusTransVel_KB;
        }
        return true;
    }
    }
    if ((code & NvKey::K_SHIFT_ANY) != 0)
    {
        if (action == NvKeyActionType::UP)
        {
            m_accelerate_KB = false;
        }
        else if (action == NvKeyActionType::DOWN)
        {
            m_accelerate_KB = true;
        }
    }

    return false;
}
