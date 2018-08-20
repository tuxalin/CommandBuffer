//----------------------------------------------------------------------------------
// File:        NvAppBase/NvPlatformContext.h
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

#ifndef NV_PLATFORM_CONTEXT_H
#define NV_PLATFORM_CONTEXT_H

#include <NvSimpleTypes.h>
#include <vector>
#include "NvKeyboard.h"
#include <string>

/// \file
/// Platform-independent input and system context

class NvGamepad;

/// Pointer input action values.
struct NvPointerActionType {
    enum Enum {
        UP = 0, ///< touch or button release
        DOWN = 1, ///< touch or button press
        MOTION = 2, ///< touch or mouse pointer movement
        EXTRA_DOWN = 4, ///< multitouch additional touch press
        EXTRA_UP = 8 ///< multitouch additional touch release
    };
};

/// Key input action values.
struct NvKeyActionType {
    enum Enum {
        UP = 0, ///< key release
        DOWN = 1, ///< key initial press
        REPEAT = 2 ///< key auto-repeat (held)
    };
};

/// Type of pointer input devices.
struct NvInputDeviceType {
    enum Enum {
        MOUSE, ///< Mouse (possibly multi-button)
        TOUCH, ///< Touch (possibly multi-touch)
        STYLUS ///< Stylus (possibly multiple "point" types)
    };
};

/// Pointer event key modifiers (generally mouse-only).
struct NvPointerModifierType {
    enum Enum {
    SHIFT = 0x00000001, ///< Shift key down
    CTRL = 0x00000002, ///< Control key down
    ALT = 0x00000004, ///< Alt key down
    };
};

/// Mouse button masks.
/// The button indices are bitfields.  i.e., button-3 == middle == 1<<3.
struct NvMouseButton {
    const static uint32_t LEFT = 0x00000001; ///< Left button
    const static uint32_t RIGHT = 0x00000002; ///< Right button
    const static uint32_t MIDDLE = 0x00000004; ///< Middle button
};

/// Basic single-point pointer event.
class NvPointerEvent {
public:
    float m_x; ///< x value in pixel-space
    float m_y; ///< y value in pixel-space
    uint32_t m_id; ///< Unique ID for tracking multiple touches
    NvInputDeviceType::Enum m_device; ///< Device type for this pointer event
};

/// Input handling callback interfaces.
class NvInputCallbacks 
{
public:
    /// Pointer input event.
    /// Called when any pointer device has changed
    /// \param[in] device the device generating the event
    /// \param[in] action the action represented by the event
    /// \param[in] modifiers any modifiers to the event (normally only for mice)
    /// \param[in] count the number of points in the #points array parameter
    /// \param[in] points an array of the points in the event (normally a single point 
    /// unless the device is a multi-touch screen
    /// \return true if the recipient handled the event, false if the recipient wishes
    /// the caller to handle the event
    virtual bool pointerInput(NvInputDeviceType::Enum device,
                                NvPointerActionType::Enum action, 
                                uint32_t modifiers, 
                                int32_t count, NvPointerEvent* points,
                                int64_t timestamp=0) = 0;

    /// Key input event
    /// Called when a key is pressed, released or held
    /// param[in] code the keycode of the event.  This is an #NvKey mask
    /// param[in] action the action for the given key
    /// \return true if the recipient handled the event, false if the recipient wishes
    /// the caller to handle the event.  Returning true from a key event will generally
    /// preclude any #characterInput events coming from the action
    virtual bool keyInput(uint32_t code, NvKeyActionType::Enum action) = 0;

    /// Character input event
    /// Called when a keypressed, release or hold is unhandled and maps to a character
    /// param[in] c the ASCII character of the event
    /// \return true if the recipient handled the event, false if the recipient wishes
    /// the caller to handle the event
    virtual bool characterInput(uint8_t c) = 0;

    /// Gamepad input event
    /// Called when any button or axis value on any active gamepad changes
    /// \param[in] changedPadFlags a mask of the changed pad indices.  For each gamepad
    /// i that has changed, bit (1<<i) will be set.
    /// \return true if the recipient handled the event, false if the recipient wishes
    /// the caller to handle the event
    virtual bool gamepadChanged(uint32_t changedPadFlags) = 0;
};


/// Redraw frequency mode.
struct NvRedrawMode {
    enum Enum {
        UNBOUNDED, ///< Call the rendering callback as fast as looping will allow - might block in swaps, etc
        VSYNC, ///< Call the rendering callback at the rate of vsync or composition; avoids queuing frames
        ON_DEMAND, ///< Render only when the app requests
    };
};

/// Basic platform functionality interface.
/// Interface class representing basic platform functionalities.  This is merely the
/// interface; the application framework must implement the platform-dependent functionalities
/// along with a method of retrieving an instance
class NvPlatformContext {
public:
    /// Mainloop active query.
    /// Query to determine if the application should continue to parse events, or can return
    /// from its mainloop.
    /// \return true if the app must continue processing events via #pollEvents, false if the
    /// application can exit its mainloop and return from its "main" function
    virtual bool isAppRunning() = 0;

    /// Request exit.
    /// Function to allow the app to request to exit.
    /// Applications should NOT assume that they will immediately exit.  Applications cannot
    /// stop processing events in their mainloop until #isAppRunning returns false.
    virtual void requestExit() = 0;

    /// Update event system.
    /// Function allowing the application to pass its input callback implementation to the
    /// platform in order to process any new events.
    /// \param[in] callbacks an implementation of the input callbacks interface to be used to process
    /// any new events.
    /// \return true on success and false on failure.  This return value does NOT indicate the
    /// presence or absence of new events.
    virtual bool pollEvents(NvInputCallbacks* callbacks) = 0;

    /// Context lost query.
    /// Returns whether the application's graphics context has been "lost" (i.e. all existing
    /// graphics resources have been deleted and will need to be recreated before rendering again)
    /// \return true if the context has been lost, false if the context is intact.  A false return
    /// value does not in and of itself indicate that the system is entirely ready to render.  That
    /// requires the use of #shouldRender
    virtual bool isContextLost() = 0;

    /// Context bound query.
    /// Returns whether the application's graphics context is bound and thus the app can make GL calls
    /// \return true if the context is bound, false if the context is not bound.  A true return
    /// value does not in and of itself indicate that the app should be rendering actively.  That
    /// requires the use of #shouldRender
    virtual bool isContextBound() = 0;

    /// Rendering query.
    /// Returns whether the application in in a state such that it should be rendering at
    /// its normal, interactive rate (i.e. visible, focused, etc)
    /// \return true if the application is expected to be rendering, false if it should skip
    /// rendering at this time
    virtual bool shouldRender() = 0;

    /// Window resize query.
    /// Returns whether the application's window has resized since the last call to this query
    /// \return true if the window has been resized and false if not
    virtual bool hasWindowResized() = 0;

    /// Redraw mode query
    /// Returns the current redraw mode.  Depending on support, this may NOT match the requested mode
    /// \return the currently implemented mode
    virtual NvRedrawMode::Enum getRedrawMode() = 0;

    /// Set redraw mode query
    /// Attempts to set the current redraw mode.  Depending on support, the requested mode may NOT be
    /// matched.  Call getRedrawMode to confirm.
    /// \param[in] mode declares the desired mode
    virtual void setRedrawMode(NvRedrawMode::Enum mode) = 0;

    /// Request redraw
    /// When the system is in NvRedrawMode::ON_DEMAND, this function requests a redraw be queued.
    /// The redraw will not be done immediately in this call, so the call is safe to make inside of
    /// the application's rendering function.  In any other redraw mode, this function is a NO-OP.
    virtual void requestRedraw() = 0;

    /// Gamepad access.
    /// Returns the gamepad class instance if available
    /// \return a pointer to the platform's gamepad implementation, or NULL if there is no
    /// gamepad support
    virtual NvGamepad* getGamepad() = 0;

    /// Set app title.
    /// Allows an application to set the title to be used by the platform
    /// and framework as needed.
    /// param[in] title a null-terminated string containing a user-readable application name
    virtual void setAppTitle(const char* title) = 0;

    /// Platform-specific application context access.
    /// Most apps should remain cross-platform, but for developers wishing to create
    /// platform-specific applications, this function returns a platform-specific type
    /// that is likely to be the most useful for an application
    /// Currently, this is only supported on Android, and returns the NvAppBase source-internal
    /// object of type android_app as declared in
    /// extensions/src/NvAppBase/nvAndroidNativeAppGlue.h
    /// Currently, all other platforms return NULL. 
    /// \return a per-platform application object, or NULL if not supported on the platform
    virtual void* getPlatformApp() { return NULL; }
};

#endif
