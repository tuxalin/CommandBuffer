//----------------------------------------------------------------------------------
// File:        NvAppBase\glfw/InputCallbacksGLFW.cpp
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

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "NvAppBase/NvPlatformContext.h"

NvInputCallbacks* sCallbacks = NULL;
static std::map<int32_t, NvKey::Enum> sKeyCodeRemap;

void prepareKeyCodeRemap() {
    sKeyCodeRemap[GLFW_KEY_UNKNOWN] = NvKey::K_UNKNOWN;
    sKeyCodeRemap[GLFW_KEY_SPACE] = NvKey::K_SPACE;
    sKeyCodeRemap[GLFW_KEY_APOSTROPHE] = NvKey::K_APOSTROPHE;
    sKeyCodeRemap[GLFW_KEY_COMMA] = NvKey::K_COMMA;
    sKeyCodeRemap[GLFW_KEY_PERIOD] = NvKey::K_PERIOD;
    sKeyCodeRemap[GLFW_KEY_MINUS] = NvKey::K_MINUS;
    for (int32_t i=0; i<=9; i++)
        sKeyCodeRemap[GLFW_KEY_0+i] = (NvKey::Enum)(NvKey::K_0+(uint32_t)i); // these happen to be same values...
    sKeyCodeRemap[GLFW_KEY_SEMICOLON] = NvKey::K_SEMICOLON;
    sKeyCodeRemap[GLFW_KEY_EQUAL] = NvKey::K_EQUAL;
    for (int32_t i=0; i<26; i++)
        sKeyCodeRemap[GLFW_KEY_A+i] = (NvKey::Enum)(NvKey::K_A+(uint32_t)i); // these happen to be same values...
    sKeyCodeRemap[GLFW_KEY_LEFT_BRACKET] = NvKey::K_LBRACKET;
    sKeyCodeRemap[GLFW_KEY_BACKSLASH] = NvKey::K_BACKSLASH;
    sKeyCodeRemap[GLFW_KEY_RIGHT_BRACKET] = NvKey::K_RBRACKET;
    sKeyCodeRemap[GLFW_KEY_GRAVE_ACCENT] = NvKey::K_ACCENT_GRAVE;
    sKeyCodeRemap[GLFW_KEY_ESCAPE] = NvKey::K_ESC;
    sKeyCodeRemap[GLFW_KEY_ENTER] = NvKey::K_ENTER;
    sKeyCodeRemap[GLFW_KEY_TAB] = NvKey::K_TAB;
    sKeyCodeRemap[GLFW_KEY_BACKSPACE] = NvKey::K_BACKSPACE;
    sKeyCodeRemap[GLFW_KEY_INSERT] = NvKey::K_INSERT;
    sKeyCodeRemap[GLFW_KEY_DELETE] = NvKey::K_DELETE;
    sKeyCodeRemap[GLFW_KEY_RIGHT] = NvKey::K_ARROW_RIGHT;
    sKeyCodeRemap[GLFW_KEY_LEFT] = NvKey::K_ARROW_LEFT;
    sKeyCodeRemap[GLFW_KEY_DOWN] = NvKey::K_ARROW_DOWN;
    sKeyCodeRemap[GLFW_KEY_UP] = NvKey::K_ARROW_UP;
    sKeyCodeRemap[GLFW_KEY_PAGE_UP] = NvKey::K_PAGE_UP;
    sKeyCodeRemap[GLFW_KEY_PAGE_DOWN] = NvKey::K_PAGE_DOWN;
    sKeyCodeRemap[GLFW_KEY_HOME] = NvKey::K_HOME;
    sKeyCodeRemap[GLFW_KEY_END] = NvKey::K_END;
    sKeyCodeRemap[GLFW_KEY_CAPS_LOCK] = NvKey::K_CAPS_LOCK;
    sKeyCodeRemap[GLFW_KEY_SCROLL_LOCK] = NvKey::K_SCROLL_LOCK;
    sKeyCodeRemap[GLFW_KEY_NUM_LOCK] = NvKey::K_NUMLOCK;
    sKeyCodeRemap[GLFW_KEY_PRINT_SCREEN] = NvKey::K_PRINT_SCREEN;
    sKeyCodeRemap[GLFW_KEY_PAUSE] = NvKey::K_PAUSE;
    for (int32_t i=0; i<12; i++) // we only support through F12 atm. !!!TBD
        sKeyCodeRemap[GLFW_KEY_F1+i] = (NvKey::Enum)(NvKey::K_F1+(uint32_t)i); // these happen to be same values...
    for (int32_t i=0; i<=9; i++) 
        sKeyCodeRemap[GLFW_KEY_KP_0+i] = (NvKey::Enum)(NvKey::K_KP_0+(uint32_t)i); // these happen to be same values...
    sKeyCodeRemap[GLFW_KEY_KP_DECIMAL] = NvKey::K_KP_DECIMAL;
    sKeyCodeRemap[GLFW_KEY_KP_DIVIDE] = NvKey::K_KP_DIVIDE;
    sKeyCodeRemap[GLFW_KEY_KP_MULTIPLY] = NvKey::K_KP_MULTIPLY;
    sKeyCodeRemap[GLFW_KEY_KP_SUBTRACT] = NvKey::K_KP_SUBTRACT;
    sKeyCodeRemap[GLFW_KEY_KP_ADD] = NvKey::K_KP_ADD;
    sKeyCodeRemap[GLFW_KEY_KP_ENTER] = NvKey::K_KP_ENTER;
    sKeyCodeRemap[GLFW_KEY_KP_EQUAL] = NvKey::K_KP_ENTER; // !!!!TBD
    sKeyCodeRemap[GLFW_KEY_LEFT_SHIFT] = NvKey::K_SHIFT_LEFT;
    sKeyCodeRemap[GLFW_KEY_LEFT_CONTROL] = NvKey::K_CTRL_LEFT;
    sKeyCodeRemap[GLFW_KEY_LEFT_ALT] = NvKey::K_ALT_LEFT;
    sKeyCodeRemap[GLFW_KEY_LEFT_SUPER] = NvKey::K_WINDOWS_LEFT;
    sKeyCodeRemap[GLFW_KEY_RIGHT_SHIFT] = NvKey::K_SHIFT_RIGHT;
    sKeyCodeRemap[GLFW_KEY_RIGHT_CONTROL] = NvKey::K_CTRL_RIGHT;
    sKeyCodeRemap[GLFW_KEY_RIGHT_ALT] = NvKey::K_ALT_RIGHT;
    sKeyCodeRemap[GLFW_KEY_RIGHT_SUPER] = NvKey::K_WINDOWS_RIGHT;
    sKeyCodeRemap[GLFW_KEY_MENU] = NvKey::K_CONTEXT; // !!!TBD proper match?
}


// key callback
static void glfw_key( GLFWwindow* window, int32_t k, int32_t s, int32_t action, int32_t mods )
{
    bool handled = false;

    NvKey::Enum nvkey = NvKey::K_UNKNOWN;
    std::map<int32_t,NvKey::Enum>::const_iterator remap = sKeyCodeRemap.find(k);
    if (remap != sKeyCodeRemap.end())
        nvkey = remap->second;

    if (nvkey == NvKey::K_UNKNOWN)
        return;

    if (sCallbacks)
    {
        NvKeyActionType::Enum nvact;
        if (action==GLFW_PRESS)
            nvact = NvKeyActionType::DOWN;
        else if (action==GLFW_REPEAT)
            nvact = NvKeyActionType::REPEAT;
        else
            nvact = NvKeyActionType::UP;
        handled = sCallbacks->keyInput(nvkey, nvact);
    }

    // Send as a key event; if that is rejected, then send it as a char
    if (!handled) {
        if ( action == GLFW_RELEASE ) return;

        switch (nvkey) {
        case NvKey::K_ESC:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        default:
            if (sCallbacks)
                sCallbacks->characterInput((uint8_t) nvkey);
            return;
        }
    }
}

static int32_t s_lastButton = 0;

// mouse button callback
static void glfw_mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
{
    if (sCallbacks) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        NvPointerEvent p;
        p.m_x = (float)x;
        p.m_y = (float)y;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            p.m_id = NvMouseButton::LEFT;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            p.m_id = NvMouseButton::MIDDLE;
        else
            p.m_id = NvMouseButton::RIGHT;
        s_lastButton = p.m_id;
        p.m_device = NvInputDeviceType::MOUSE;
        sCallbacks->pointerInput(p.m_device, action ? NvPointerActionType::DOWN : NvPointerActionType::UP, 0, 1, &p);
    }
}

// mouse/cursor motion callback
static void glfw_motion(GLFWwindow* window, double x, double y)
{
    if (sCallbacks) {
        NvPointerEvent p;
        p.m_x = (float)x;
        p.m_y = (float)y;
        p.m_id = s_lastButton;
        p.m_device = NvInputDeviceType::MOUSE;
        sCallbacks->pointerInput(p.m_device, NvPointerActionType::MOTION, 0, 1, &p);
    }
}

void setInputCallbacksGLFW(GLFWwindow *window)
{
    prepareKeyCodeRemap();
    glfwSetKeyCallback(window, glfw_key);
    glfwSetMouseButtonCallback(window, glfw_mouse);
    glfwSetCursorPosCallback(window, glfw_motion);
    //glfwSetScrollCallback(window, scroll);
}

