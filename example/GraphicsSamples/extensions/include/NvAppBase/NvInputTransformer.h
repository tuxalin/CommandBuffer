//----------------------------------------------------------------------------------
// File:        NvAppBase/NvInputTransformer.h
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

#ifndef NV_INPUT_TRANSFORMER_H
#define NV_INPUT_TRANSFORMER_H

#include <NvSimpleTypes.h>
#include "NvPlatformContext.h"
#include "NvGamepad/NvGamepad.h"
#include "NV/NvMath.h"

/// \file
/// Automated input-to-camera motion mapping

/// Camera motion mode.
struct NvCameraMotionType {
    enum Enum {
        ORBITAL = 0, ///< Camera orbits the world origin
        FIRST_PERSON = 1, ///< Camera moves as in a 3D, first-person shooter
        PAN_ZOOM = 2, ///< Camera pans and zooms in 2D
        DUAL_ORBITAL = 3 ///< Two independent orbital transforms
    };
};

/// Camera transform mode.
struct NvCameraXformType {
    enum Enum {
        MAIN = 0, ///< Default transform
        SECONDARY = 1, ///< Secondary transform
        COUNT = 2 ///< Number of transforms
    };
};

/// Automatic mapping of input devices to camera movements.
/// Maps touch, mouse and gamepad into useful camera movements as represented
/// by a modelview transform.  Lower-level matrices are also available for 
/// custom interaction.
class NvInputTransformer
{
public:
    NvInputTransformer();
    ~NvInputTransformer();

    /// Reset.
    /// Reset all input values to default/starting points, restoring initial view.
    void reset();

    /// Set screen size.
    /// Set the current screen size, so that the touch/mouse mapping methods
    /// scale correctly.  Failure to keep this up to date with the screen size
    /// can lead to wildly incorrect mouse/touch mappings
    /// \param[in] width the new window/screen width
    /// \param[in] height the new window/screen height
    void setScreenSize(int32_t width, int32_t height) { m_width = width; m_height = height; }

    /// Set motion mode.
    /// Set the desired input-to-motion mapping mode
    /// \param[in] mode the desired mapping mode
    void setMotionMode(NvCameraMotionType::Enum mode) { m_motionMode = mode; }

    /// Get motion mode.
    /// Get the current input-to-motion mapping
    /// \return the current mapping mode
    NvCameraMotionType::Enum getMotionMode() { return m_motionMode; }

    /// Get the current modelview transform.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the current combined transform matrix for the given mapping mode.
    /// This matrix should be placed between the scene's modelview matrix and the
    /// camera projection matrix: FinalMat = Projection X #getModelViewMat() X ModelView
    nv::matrix4f getModelViewMat(NvCameraXformType::Enum xform = NvCameraXformType::MAIN);

    ///@{
    /// Get the component matrices.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the given basic component matrix for the current mode and settings.
    /// Most apps will not need these components, but will instead use #getModelViewMat
    const nv::matrix4f& getRotationMat(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_rotateMat; }
    const nv::matrix4f& getTranslationMat(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_translateMat; }
    const nv::matrix4f& getScaleMat(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_scaleMat; }
    ///@}

    /// Query to determine if the current configuration of the controls is causing animation
    /// Can be used in on-demand redraw cases to determine whether continuous redraw is likely
    /// to be required.
    /// \return True if the current configuration should cause animation, false if the transforms
    /// are not changing
    bool isAnimating();

    /// Get the current rotation vector.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the angles (in radians) used to create the rotation component matrix
    const nv::vec3f& getRotationVec(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_rotate; }

    /// Set the current rotation vector.
    /// Set the rotation angles.  Very useful for setting the initial view at application start.
    /// \param[in] vec the angles (in radians) used to create the rotation component matrix
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    void setRotationVec(const nv::vec3f& vec, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_rotate = vec; }

    /// Get the current translation vector.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the vector used to create the translation component matrix
    const nv::vec3f& getTranslationVec(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_translate; }

    /// Set the current translation vector.
    /// Set the translation.  Very useful for setting the initial view at application start.
    /// \param[in] vec the translation used to create the rotation component matrix
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    void setTranslationVec(const nv::vec3f& vec, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_translate = vec; }

    /// Get the current (uniform) scale factor.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the factor used to create the scale component matrix
    const float getScale(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_scale*m_xforms[xform].m_dscale; }

    /// Set the current (uniform) scale factor.
    /// Set the scaling.  Very useful for setting the initial view at application start.
    /// \param[in] scale the scale factor used to create the scale component matrix
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    void setScale(const float scale, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_scale = scale; }

    /// Get the current rotation velocity.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the current rotation velocity about each axis in radians per second
    const nv::vec3f& getRotationVel(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_rotateVel; }

    /// Set the current rotation velocity.
    /// Note this this velocity will be overwritten by gamepad axis motion.  It is mainly
    /// useful for adding an initial rotation animation on the camera before the user has
    /// provided input
    /// \param[in] vec the current rotation velocity about each axis in radians per second.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    void setRotationVel(const nv::vec3f& vec, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_rotateVel = vec; }

    /// Get the current translation velocity.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the current translation velocity in units per second
    const nv::vec3f& getTranslationVel(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_translateVel; }

    /// Set the current translation velocity.
    /// Note this this velocity will be overwritten by gamepad axis motion.  It is mainly
    /// useful for adding an initial translation animation on the camera before the user has
    /// provided input
    /// \param[in] vec the current translation velocity in units per second.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    void setTranslationVel(const nv::vec3f& vec, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_translateVel = vec; }

    /// Get the current "max" rotation velocity.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the current "max" rotation velocity (the velocity produced when the
    /// corresponding gamepad axis is at full lock) about each axis in radians per second
    float getMaxRotationVel(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_maxRotationVel; }

    /// Set the current "max" rotation velocity.
    /// \param[in] maxVel the current "max" rotation velocity (the velocity produced when the
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// corresponding gamepad axis is at full lock) about each axis in radians per second
    void setMaxRotationVel(float maxVel, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_maxRotationVel = maxVel; }

    /// Get the current "max" translation velocity.
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// \return the current "max" translation velocity (the velocity produced when the
    /// corresponding gamepad axis is at full lock) in units per second
    float getMaxTranslationVel(NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { return m_xforms[xform].m_maxTranslationVel; }

    /// Set the current "max" translation velocity.
    /// \param[in] maxVel the current "max" translation velocity (the velocity produced when the
    /// \param[in] xform the index of the transform to be referenced.  For all
    /// modes except DUAL_ORBITAL, this can be left as the default
    /// corresponding gamepad axis is at full lock) in units per second
    void setMaxTranslationVel(float maxVel, NvCameraXformType::Enum xform = NvCameraXformType::MAIN)
    { m_xforms[xform].m_maxTranslationVel = maxVel; }

    /// Update the matrices.
    /// Update the matrices based on the current inputs, velocities, and delta time.
    /// \param[in] deltaTime the time since the last call to #update, in seconds
    void update(float deltaTime);

    /// Pointer event input.
    /// Used to pass pointer input events to the transformer.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] device the input device
    /// \param[in] action the input action
    /// \param[in] modifiers the input modifiers
    /// \param[in] count the number of elements in the #points array
    /// \param[in] points the input event points
    /// \return true if the event was used and "eaten" by the input transformer and should
    /// not be processed by any other input system.
    bool processPointer(NvInputDeviceType::Enum device, NvPointerActionType::Enum action, 
        uint32_t modifiers, int32_t count, NvPointerEvent* points);

    /// Gamepad event input.
    /// Used to pass gamepad state to the transformer.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] changedPadFlags the changes pad flags
    /// \param[in] pad the gamepad device
    /// \return true if the event was used and "eaten" by the input transformer and should
    /// not be processed by any other input system.
    bool processGamepad(uint32_t changedPadFlags, NvGamepad& pad);

    /// Key event input.
    /// Used to pass key input events to the transformer.  The signature explicitly matches
    /// that of the input callbacks used to provide input to an app or app framework for ease
    /// of calling
    /// \param[in] code the input keycode
    /// \param[in] action the input action
    /// \return true if the event was used and "eaten" by the input transformer and should
    /// not be processed by any other input system.
    bool processKey(uint32_t code, NvKeyActionType::Enum action);

private:
    /// \privatesection
    void updateMats(NvCameraXformType::Enum xform);

    NvCameraMotionType::Enum m_motionMode;
    bool m_touchDown;
    uint8_t m_maxPointsCount;
    enum ControlMode { TRANSLATE=0, ROTATE, SECONDARY_ROTATE, ZOOM } m_mode;
    nv::vec2f m_firstInput;
    nv::vec2f m_lastInput;
    int8_t m_xVel_kb;
    int8_t m_zVel_kb;
    int8_t m_zVel_mouse;
    float m_xVel_gp;
    float m_zVel_gp;

    int32_t m_width;
    int32_t m_height;
    
    class Transform {
    public:
        nv::vec3f m_translateVel;
        nv::vec3f m_rotateVel;
        float m_maxRotationVel;
        float m_maxTranslationVel;

        nv::vec3f m_translate;
        nv::vec3f m_rotate;
        float m_scale, m_dscale;

        nv::matrix4f m_translateMat;
        nv::matrix4f m_rotateMat;
        nv::matrix4f m_scaleMat;
    };

    Transform m_xforms[NvCameraXformType::COUNT];
};

#endif
