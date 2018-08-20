//----------------------------------------------------------------------------------
// File:        NvAppBase/NvInputHandler_CameraFly.h
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

#ifndef NV_INPUT_HANDLER_CAMERA_FLY_H
#define NV_INPUT_HANDLER_CAMERA_FLY_H

#include "NvInputHandler.h"
#include "NV/NvQuaternion.h"

/// \file

/// Maps touch, mouse and gamepad into useful state for
/// first person-ish fly camera (camera-relative pitch and 
/// translation, world yaw)
class NvInputHandler_CameraFly : public NvInputHandler
{
public:
    NvInputHandler_CameraFly();
    virtual ~NvInputHandler_CameraFly();

    /// Reset.
    /// Reset all input values to default/starting points, restoring initial view.
    virtual void reset();

    /// Update the state based on the current inputs, velocities, and delta time.
    /// \param[in] deltaTime the time since the last call to #update, in seconds
    virtual void update(float deltaTime);

    /// Pointer event input.
    /// Used to pass pointer input events to the handler.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] device The input device
    /// \param[in] action The input action
    /// \param[in] modifiers The input modifiers
    /// \param[in] count The number of elements in the #points array
    /// \param[in] points The input event points
    /// \return True if the event was used and "eaten" by the input handler and should
    /// not be processed by any other input system.
    virtual bool processPointer(NvInputDeviceType::Enum device, NvPointerActionType::Enum action, 
        uint32_t modifiers, int32_t count, NvPointerEvent* points);

    /// Gamepad event input.
    /// Used to pass gamepad state to the handler.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] changedPadFlags Flags indicating which gamepads have changed state
    /// \param[in] pad The gamepad device
    /// \return True if the event was used and "eaten" by the input transformer and should
    /// not be processed by any other input system.
    virtual bool processGamepad(uint32_t changedPadFlags, NvGamepad& pad);

    /// Key event input.
    /// Used to pass key input events to the handler.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] code The input keycode
    /// \param[in] action The input action
    /// \return True if the event was used and "eaten" by the input transformer and should
    /// not be processed by any other input system.
    virtual bool processKey(uint32_t code, NvKeyActionType::Enum action);

    /// Sets the current position of the camera in world space
    /// \param[in] pos The world position at which to place the camera
    void setPosition(const nv::vec3f& pos) { m_currentPos = pos; updateMatrices(); }

    /// Retrieves the current position of the camera in world space
    /// \return The current world-space position of the camera
    const nv::vec3f& getPosition() const { return m_currentPos; }

    /// Sets the current pitch of the camera
    /// \param pitch New pitch of the camera, in radians
    void setPitch(float pitch) { m_currentPitch = pitch; updateMatrices(); }

    /// Retrieves the current pitch of the camera
    /// \return The current pitch of the camera, in radians
    float getPitch() const { return m_currentPitch; }

    /// Sets the current yaw of the camera
    /// \param pitch New yaw of the camera, in radians
    void setYaw(float yaw) { m_currentYaw = yaw; updateMatrices(); }

    /// Retrieves the current yaw of the camera
    /// \return The current yaw of the camera, in radians
    float getYaw() const { return m_currentYaw; }

	/// Returns the current look-at vector of the camera
	/// \return the look-at vector
    nv::vec3f getLookVector() const { nv::vec4f look = -m_currentCameraMat.get_column(2); return nv::vec3f(look.x, look.y, look.z); }

    /// Retrieves the current world matrix of the camera's transform
    /// \return A matrix containing a representation of the camera's
    ///         current position and orientation in world space
    const nv::matrix4f& getCameraMatrix() const { return m_currentCameraMat; }

    /// Retrieves the current view matrix from the camera
    /// \return A matrix containing a representation of the transform
    ///         required to transform a position from world space into
    ///         camera view space
    const nv::matrix4f& getViewMatrix() const { return m_currentViewMat; }

    /// Sets the speed of rotation that maps to mouse movements
    /// \param speed Speed of mouse driven rotation, in radians per pixel
    void setMouseRotationSpeed(float speed) { m_rotSpeed_Mouse = speed; }

    /// Sets the speed of translation that maps to mouse movements
    /// \param speed Speed of mouse driven translation, in meters per pixel
    void setMouseTranslationSpeed(float speed) { m_transSpeed_Mouse = speed; }

    /// Sets the speed of rotation that maps to key presses
    /// \param speed Speed of keyboard driven rotation, in radians per second
    void setKeyboardRotationSpeed(float speed) { m_rotSpeed_KB = speed; }

    /// Sets the speed of translation that maps to key presses
    /// \param speed Speed of keyboard driven translation, in meters per second
    void setKeyboardTranslationSpeed(float speed) { m_transSpeed_KB = speed; }

    /// Sets the speed of rotation that maps to gamepad sticks
    /// \param speed Speed of gamepad driven rotation, in radians per second at full deflection
    void setGamepadRotationSpeed(float speed) { m_rotSpeed_GP = speed; }

    /// Sets the speed of translation that maps to gamepad sticks
    /// \param speed Speed of gamepad driven translation, in meters per second at full deflection
    void setGamepadTranslationSpeed(float speed) { m_transSpeed_GP = speed; }

protected:

    /// Update the camera and view matrices
    void updateMatrices();

    // Current state of camera transform
    nv::vec3f m_currentPos;
    float m_currentPitch;
    float m_currentYaw;

    // Current matrices
    nv::matrix4f m_currentCameraMat;
    nv::matrix4f m_currentViewMat;

    // Number of touch points currently active
    uint8_t m_touchPointsCount;

    // Position of the last input
    nv::vec2f m_lastInput;
    // Distance separating the last touch points if multitouch is active
    float m_lastMultitouchDist;

    // Mouse
    //      Mouse Input Scaling
    float m_rotSpeed_Mouse;
    float m_transSpeed_Mouse;

    //      Current Mouse Input Values
    float m_xRotDelta_Mouse;
    float m_yRotDelta_Mouse;
    float m_zRotDelta_Mouse;

    float m_xTransDelta_Mouse;
    float m_yTransDelta_Mouse;
    float m_zTransDelta_Mouse;

    // Keyboard
    //      Keyboard Input Scaling
    float m_rotSpeed_KB;
    float m_transSpeed_KB;
    bool  m_accelerate_KB;

    //      Current Keyboard Input Values
    // Keep separate values for positive and negative movement/rotation via keyboard
    // to handle missed keyboard events (keep them from getting stuck, fix state by
    // repeatedly hitting all keys, etc.
    int8_t m_xPlusRotVel_KB;
    int8_t m_yPlusRotVel_KB;
    int8_t m_zPlusRotVel_KB;
    int8_t m_xNegRotVel_KB;
    int8_t m_yNegRotVel_KB;
    int8_t m_zNegRotVel_KB;

    int8_t m_yPlusTransVel_KB;
    int8_t m_xPlusTransVel_KB;
    int8_t m_zPlusTransVel_KB;
    int8_t m_yNegTransVel_KB;
    int8_t m_xNegTransVel_KB;
    int8_t m_zNegTransVel_KB;

    // Gamepad
    int32_t m_lastActiveGamepad;

    //      Gamepad Input Scaling
    float m_rotSpeed_GP;
    float m_transSpeed_GP;
    bool  m_accelerate_GP;

    //      Current Gamepad Input Values
    float m_xRotVel_GP;
    float m_yRotVel_GP;
    float m_zRotVel_GP;

    float m_xTransVel_GP;
    float m_yTransVel_GP;
    float m_zTransVel_GP;
};

#endif // NV_INPUT_HANDLER_CAMERA_FLY_H
