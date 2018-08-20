//----------------------------------------------------------------------------------
// File:        NvAppBase/NvInputTransformer.cpp
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
#include "NvAppBase/NvInputTransformer.h"
#include "NV/NvLogs.h"

#define NV_MIN_SCALE_PCT    0.035f
#define NV_MAX_SCALE_PCT    500.0f
#define NV_CLAMP_SCALE(s)   std::min(std::max((s), NV_MIN_SCALE_PCT), NV_MAX_SCALE_PCT)

NvInputTransformer::NvInputTransformer() :
    m_motionMode(NvCameraMotionType::ORBITAL),
    m_touchDown(false),
    m_mode(TRANSLATE),
    m_lastInput(0,0),
    m_width(1),
    m_height(1),
    m_maxPointsCount(0)
{
    reset(); // so that everything related to the view goes in one place.
}

NvInputTransformer::~NvInputTransformer()
{
}

void NvInputTransformer::reset()
{
    m_xVel_kb = 0;
    m_zVel_kb = 0;
    m_zVel_mouse = 0;
    m_xVel_gp = 0;
    m_zVel_gp = 0;
    for (int32_t i = 0; i < (int32_t)NvCameraXformType::COUNT; i++) {
        m_xforms[i].m_translateVel = nv::vec3f(0.0f, 0.0f, 0.0f);
        m_xforms[i].m_rotateVel = nv::vec3f(0.0f, 0.0f, 0.0f);
        m_xforms[i].m_translate = nv::vec3f(0.0f, 0.0f, 0.0f);
        m_xforms[i].m_rotate = nv::vec3f(0.0f, 0.0f, 0.0f);
        m_xforms[i].m_scale = 1.0f;
        m_xforms[i].m_dscale = 1.0f;
        m_xforms[i].m_maxRotationVel = NV_PI;
        m_xforms[i].m_maxTranslationVel = 5.0f;
    }
}

bool NvInputTransformer::isAnimating()
{
    static nv::vec3f zero = nv::vec3f(0.0f);
    Transform& xf = m_xforms[NvCameraXformType::MAIN];
    if (xf.m_rotateVel.x != 0.0f || xf.m_rotateVel.y != 0.0f || xf.m_rotateVel.z != 0.0f || 
        xf.m_translateVel.x != 0.0f || xf.m_translateVel.y != 0.0f || xf.m_translateVel.z != 0.0f)
        return true;

    if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL) {
        return m_xforms[NvCameraXformType::SECONDARY].m_rotateVel != zero;
    }

    return false;
}

nv::matrix4f NvInputTransformer::getModelViewMat(NvCameraXformType::Enum xform) {
    Transform& xf = m_xforms[xform];
    if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
        return xf.m_rotateMat * xf.m_translateMat * xf.m_scaleMat;
    } else {
        return xf.m_translateMat * xf.m_rotateMat * xf.m_scaleMat;
    }
}

void NvInputTransformer::updateMats(NvCameraXformType::Enum xform)
{
    Transform& xf = m_xforms[xform];
    nv::rotationYawPitchRoll(xf.m_rotateMat, xf.m_rotate.y, xf.m_rotate.x, 0.0f);
    nv::translation(xf.m_translateMat, xf.m_translate.x, xf.m_translate.y, xf.m_translate.z);
    xf.m_scaleMat.make_identity();
    xf.m_scaleMat.set_scale(NV_CLAMP_SCALE(xf.m_scale*xf.m_dscale));
}

void NvInputTransformer::update(float deltaTime) {
    if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL) {
        Transform& xfm = m_xforms[NvCameraXformType::MAIN];
        Transform& xfs = m_xforms[NvCameraXformType::SECONDARY];
        xfm.m_rotate += xfm.m_rotateVel*deltaTime;
        xfs.m_rotate += xfs.m_rotateVel*deltaTime;
        xfm.m_translate += xfm.m_translateVel * deltaTime;

        updateMats(NvCameraXformType::MAIN);
        updateMats(NvCameraXformType::SECONDARY);
    } else {
        Transform& xf = m_xforms[NvCameraXformType::MAIN];
        xf.m_rotate += xf.m_rotateVel*deltaTime;
        nv::vec3f transVel;
        if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
            // obviously, this should clamp to [-1,1] for the mul, but we don't care for sample use.
            xf.m_translateVel.x = xf.m_maxTranslationVel * (m_xVel_kb+m_xVel_gp);
            xf.m_translateVel.z = xf.m_maxTranslationVel * (m_zVel_mouse+m_zVel_kb+m_zVel_gp);
            transVel = nv::vec3f(nv::transpose(xf.m_rotateMat) * 
                nv::vec4f(-xf.m_translateVel.x, xf.m_translateVel.y, xf.m_translateVel.z, 0.0f));
        } else {
            transVel = xf.m_translateVel;
        }

        xf.m_translate += transVel * deltaTime;

        updateMats(NvCameraXformType::MAIN);
    }
}

bool NvInputTransformer::processPointer(NvInputDeviceType::Enum device, NvPointerActionType::Enum action, 
        uint32_t modifiers, int32_t count, NvPointerEvent* points)
{
    Transform& xfm = m_xforms[NvCameraXformType::MAIN];
    Transform& xfs = m_xforms[NvCameraXformType::SECONDARY];
    float x = points[0].m_x;
    float y = points[0].m_y;
    uint32_t button = points[0].m_id;
    bool needsUpdate = false;

    if (action == NvPointerActionType::UP) {
        // if count == 0, it's 'cancel' with no 'release' location
        if (count==0) {
        } else {
        }
        m_touchDown = false;
        m_maxPointsCount=0;
        // lock in scaling
        if (m_motionMode != NvCameraMotionType::FIRST_PERSON) {
            xfm.m_scale = NV_CLAMP_SCALE(xfm.m_scale*xfm.m_dscale);
            xfm.m_dscale = 1.0f;
        }
    } else if (action == NvPointerActionType::MOTION) {
        if (m_touchDown) {
            if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
                xfm.m_rotate.x += ((y - m_lastInput.y) / (float) m_height) * xfm.m_maxRotationVel;
                xfm.m_rotate.y += ((x - m_lastInput.x) / (float) m_width) * xfm.m_maxRotationVel;
                needsUpdate = true;
            } else if (m_maxPointsCount==1) {
                switch(m_mode) {
                case ROTATE:
                    xfm.m_rotate.x += ((y - m_lastInput.y) / (float) m_height) * xfm.m_maxRotationVel;
                    xfm.m_rotate.y += ((x - m_lastInput.x) / (float) m_width) * xfm.m_maxRotationVel;
                    needsUpdate = true;
                    break;
                case SECONDARY_ROTATE:
                    xfs.m_rotate.x += ((y - m_lastInput.y) / (float) m_height) * xfs.m_maxRotationVel;
                    xfs.m_rotate.y += ((x - m_lastInput.x) / (float) m_width) * xfs.m_maxRotationVel;
                    needsUpdate = true;
                    break;
                case TRANSLATE:
                    xfm.m_translate.x += ((x - m_lastInput.x) / (float) m_width) * xfm.m_maxTranslationVel;
                    xfm.m_translate.y -= ((y - m_lastInput.y) / (float) m_height)* xfm.m_maxTranslationVel;
                    needsUpdate = true;
                    break;
                case ZOOM: {
                    float dy = y - m_firstInput.y; // negative for moving up, positive moving down.
                    if (dy > 0) // scale up...
                        xfm.m_dscale = 1.0f + dy/(m_height/16.0f);
                    else if (dy < 0) { // scale down...
                        xfm.m_dscale = 1.0f - (-dy/(m_height/2));
                        xfm.m_dscale *= xfm.m_dscale; // accelerate the shrink...
                    }
                    xfm.m_dscale = NV_CLAMP_SCALE(xfm.m_dscale);
                    needsUpdate = true;
                    break;
                }
                default:
                    break;
                }
            } else { // >1 points during this 'gesture'
                 if (m_maxPointsCount==2 && count==2) {
                    if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL) {
                        xfs.m_rotate.x += ((y - m_lastInput.y) / (float) m_height) * xfs.m_maxRotationVel;
                        xfs.m_rotate.y += ((x - m_lastInput.x) / (float) m_width) * xfs.m_maxRotationVel;
                        needsUpdate = true;
                    } else if (m_motionMode != NvCameraMotionType::FIRST_PERSON) {
                        // calc pinch dx,dy.
                        float dx = points[0].m_x - points[1].m_x;
                        float dy = points[0].m_y - points[1].m_y;

                        // first, handle setting scale values.
                        // firstInput holds gesture-start dx,dy
                        nv::vec2f curr = nv::vec2f(points[0].m_x - points[1].m_x, points[0].m_y - points[1].m_y);
                        xfm.m_dscale = nv::length(curr)/nv::length(m_firstInput); 
                        xfm.m_dscale = NV_CLAMP_SCALE(xfm.m_dscale);

                        // second, handle translation, calc new center of pinch
                        // lastInput handles prior move x/y centerpoint.
                        curr = nv::vec2f(points[0].m_x - (dx/2), points[0].m_y - (dy/2));
                        xfm.m_translate.x += ((curr.x - m_lastInput.x) / (float) m_width) * xfm.m_maxTranslationVel;
                        xfm.m_translate.y -= ((curr.y - m_lastInput.y) / (float) m_height)* xfm.m_maxTranslationVel;
                        LOGI("old center = %0.2f,%0.2f :: new center = %0.2f,%0.2f", m_lastInput.x, m_lastInput.y, curr.x, curr.y);
                        LOGI("trans = %0.2f,%0.2f", xfm.m_translate.x, xfm.m_translate.y);
                        m_lastInput = curr; // cache current center.

                        needsUpdate = true;
                    }
                }
            }
        }
    } else { // down or extra_down or extra_up
        if (action == NvPointerActionType::DOWN) {
            m_touchDown = true;
            m_maxPointsCount = 1;
            xfm.m_dscale = 1.0f; // for sanity reset to 1.

            if (m_motionMode == NvCameraMotionType::PAN_ZOOM)
                m_mode = TRANSLATE;
            else
                m_mode = ROTATE;

            if (m_motionMode != NvCameraMotionType::FIRST_PERSON) {
                if (device == NvInputDeviceType::MOUSE) {
                    if (m_motionMode == NvCameraMotionType::ORBITAL) {
                        if (button & NvMouseButton::MIDDLE)
                            m_mode = ZOOM;
                        else if (button & NvMouseButton::RIGHT)
                            m_mode = TRANSLATE;
                    } else if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL) {
                        if (button & NvMouseButton::LEFT)
                            m_mode = ROTATE;
                        else if (button & NvMouseButton::RIGHT)
                            m_mode = SECONDARY_ROTATE;
                    } else { // PAN_ZOOM
                        if (button & NvMouseButton::RIGHT)
                            m_mode = ZOOM;
                    }
                }
            }
            m_firstInput = nv::vec2f(points[0].m_x, points[0].m_y);
        } else if (action == NvPointerActionType::EXTRA_DOWN) {
            m_maxPointsCount = (uint8_t)count; // cache max fingers.
            if ((m_motionMode != NvCameraMotionType::FIRST_PERSON) && (m_maxPointsCount==2)) {
                if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL)
                    m_mode = SECONDARY_ROTATE;
                else
                    m_mode = ZOOM;
                // cache starting distance across pinch
                float dx = points[0].m_x - points[1].m_x;
                float dy = points[0].m_y - points[1].m_y;
                m_firstInput = nv::vec2f(dx, dy);
                // cache center of pinch.
                m_lastInput = nv::vec2f(points[0].m_x - (dx/2), points[0].m_y - (dy/2));
            }
        } else {
            // extra up.
        }
    }

    if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
        m_zVel_mouse = 0;
        if ( (count > 1) 
            || ((device == NvInputDeviceType::MOUSE) && (button & NvMouseButton::MIDDLE)
                    && m_touchDown) )
            m_zVel_mouse = +1;
    }

    if ((m_maxPointsCount==1) || (m_motionMode == NvCameraMotionType::FIRST_PERSON) ||
        (m_motionMode == NvCameraMotionType::DUAL_ORBITAL)) {
        m_lastInput = nv::vec2f(x,y);
    }

    if (needsUpdate) {
        updateMats(NvCameraXformType::MAIN);
        if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL)
            updateMats(NvCameraXformType::SECONDARY);
    }

    return true;
}

bool NvInputTransformer::processGamepad(uint32_t changedPadFlags, NvGamepad& pad)
{
    Transform& xfm = m_xforms[NvCameraXformType::MAIN];
    Transform& xfs = m_xforms[NvCameraXformType::SECONDARY];
    NvGamepad::State state;
    int32_t i;
    for (i = 0; i < NvGamepad::MAX_GAMEPADS; i++) {
        if (changedPadFlags & (1<<i)) {
            pad.getState(i, state);

            if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
                m_xVel_gp = state.mThumbLX;
                m_zVel_gp = -state.mThumbLY;

                xfm.m_translateVel.y = state.mLeftTrigger * xfm.m_maxTranslationVel;
                xfm.m_translateVel.y -= state.mRightTrigger * xfm.m_maxTranslationVel;

                xfm.m_rotateVel.y = state.mThumbRX * xfm.m_maxRotationVel;
                xfm.m_rotateVel.x = state.mThumbRY * xfm.m_maxRotationVel;
            } else if (m_motionMode == NvCameraMotionType::DUAL_ORBITAL) {
                xfm.m_rotateVel.y = state.mThumbRX * xfm.m_maxRotationVel;
                xfm.m_rotateVel.x = state.mThumbRY * xfm.m_maxRotationVel;

                xfs.m_rotateVel.y = state.mThumbLX * xfs.m_maxRotationVel;
                xfs.m_rotateVel.x = state.mThumbLY * xfs.m_maxRotationVel;

                xfm.m_translateVel.z = state.mLeftTrigger * xfm.m_maxTranslationVel;
                xfm.m_translateVel.z -= state.mRightTrigger * xfm.m_maxTranslationVel;
            } else {
                xfm.m_rotateVel.y = state.mThumbRX * xfm.m_maxRotationVel;
                xfm.m_rotateVel.x = state.mThumbRY * xfm.m_maxRotationVel;

                xfm.m_translateVel.x = state.mThumbLX * xfm.m_maxTranslationVel;
                xfm.m_translateVel.y = state.mLeftTrigger * xfm.m_maxTranslationVel;
                xfm.m_translateVel.y -= state.mRightTrigger * xfm.m_maxTranslationVel;
                xfm.m_translateVel.z = -state.mThumbLY * xfm.m_maxTranslationVel;
            }
        }
    }
    return true;
}

bool NvInputTransformer::processKey(uint32_t code, NvKeyActionType::Enum action)
{
    if (m_motionMode == NvCameraMotionType::FIRST_PERSON) {
        // FPS mode uses WASD for motion, so that UI can own arrows for focus control.
        switch(code) {
            case NvKey::K_W:
                if (action == NvKeyActionType::UP) {
                    if (m_zVel_kb == +1) // only turn our own value 'off'
                        m_zVel_kb = 0;
                } else {
                    m_zVel_kb = +1;
                }
                return true;
            case NvKey::K_S:
                if (action == NvKeyActionType::UP) {
                    if (m_zVel_kb == -1) // only turn our own value 'off'
                        m_zVel_kb = 0;
                } else {
                    m_zVel_kb = -1;
                }
                return true;

            case NvKey::K_A:
                if (action == NvKeyActionType::UP) {
                    if (m_xVel_kb == -1) // only turn our own value 'off'
                        m_xVel_kb = 0;
                } else {
                    m_xVel_kb = -1;
                }
                return true;
            case NvKey::K_D:
                if (action == NvKeyActionType::UP) {
                    if (m_xVel_kb == +1) // only turn our own value 'off'
                        m_xVel_kb = 0;
                } else {
                    m_xVel_kb = +1;
                }
                return true;
        }
    }

    return false;
}
