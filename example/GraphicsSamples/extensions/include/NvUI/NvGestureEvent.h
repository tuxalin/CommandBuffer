//----------------------------------------------------------------------------------
// File:        NvUI/NvGestureEvent.h
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

#ifndef __EVENT_H_
#define __EVENT_H_

#include <NvSimpleTypes.h>

/** @file NvGestureEvent.h
    Types and values for abstracting pointer/touch inputs into higher level
    'gesture' motions and results.
*/

/* a predefined value for an invalid gesture UID. */
#define NV_GESTURE_UID_INVALID              0xFFFFFFFF
#define NV_GESTURE_UID_MAX                  0xFFFFEEEE
typedef uint32_t NvGestureUID;

// mouse/stylus (accurate) vs finger (inaccurate).
// these are used as array indices + max size!
/** These are types of input devices generating pointer events.
    We might use this information to adjust hit margins and such
    based on the average size of the pointer.
    */
struct NvInputEventClass {
    enum Enum {
        NONE = 0, /**< No input type specified. */
        MOUSE, /**< Mouse input */
        TOUCH, /**< Touch input */
        STYLUS,  /**< Stylus input */
        MAX
    };
};

/** These are specific types of input tracked for pointing devices. */
struct NvInputEventType {
    enum Enum {
        HOVER = 0, /**< Input pointer is 'hovering' over app window, not yet 'pressing'. */
        DOWN, /**< Input has been 'pressed' down */
        MOVE, /**< Input is moving while down */
        UP, /**< Input has been 'released' up */
    };
};


/* this says we're building the system with "MAYBE" gesture support */
#define HANDLE_MAYBE_GESTURES
/* this says we're building the system with 'plus' support
    which means that events from maybe->press==press_plus */
//#define HANDLE_PLUS_GESTURES


/** These are the kinds of resulting gestures that we recognize. */
struct NvGestureKind {
    enum Enum
    {
    /** Special bit masks for classification of types of gestures.
        @{ */
        MASK_BASE =           0x00FF, /**< For masking off low byte for raw event */
        MASK_HELD =           0x0100, /**< Event involves device being held 'down' for some period */
        MASK_RELEASED =       0x0200, /**< Event involves a 'release' up of input */
        MASK_MOVED =          0x0400, /**< Event involves some movement input device */
        MASK_MULTI =          0x0800, /**< Event involves multiple inputs, i.e. multi-finger touch */
#ifdef HANDLE_MAYBE_GESTURES
        MASK_MAYBE =          0x1000, /**< Hint that event is conditional on a later timeout */
#endif
        MASK_REPEAT_DOUBLE =  0x2000, /**< Base event occurred twice within short period */
        MASK_REPEAT_TRIPLE =  0x4000, /**< Base event occurred three times in short period */
     /** @} */

    /** The set of recognized gesture values. 
        @{ */
        /** Denotes an invalid value */
        INVALID          = -1,

        /** Base value, no gesture has occurred yet */
        NONE             = 0,
        /** Denotes a pointer hovering on screen but no press/click yet. */
        HOVER            = 1,
        /** Pointer has been pressed down, clicked */
        PRESS            = 3,
        /** Pointer has been kept down past a particular timeout */
        HOLD             = 4 | MASK_HELD, 
        // !!!!TBD TODO: possible addition, the pointer being kept down even longer */
        // /** Pointer has been kept down past a longer timeout */
        // LONG_HOLD      = 5, 
        /** Pointer has been released, let up */
        RELEASE          = 6 | MASK_RELEASED,

    #ifdef HANDLE_MAYBE_GESTURES
        /** A short tap occurred, but we haven't confirmed user intent yet due to timeout or new event */
        MAYBE_TAP        = 8 | MASK_RELEASED | MASK_MAYBE,
    #endif
        /** A brief tap was confirmed (either because double-tap delay passed, or other event began) */
        TAP              = 9 | MASK_RELEASED, 
        /** Pointer is down and has moved from its starting point enough to intend to 'drag' */
        DRAG             = 10 | MASK_MOVED,
        /** Pointer was released during accelerating motion vector */
        FLICK            = 11 | MASK_MOVED | MASK_RELEASED,

    #ifdef HANDLE_PLUS_GESTURES
        /** An extended gesture after a tap, another quick gesture right after. */
        TAP_PLUS_PRESS   = 12, /* this is an 'extended' gesture, off of prior one. */
    #endif
     /*
        //!!!!TBD TODO:
        //these "tap-plus" gestures are all TBD.
        TAP_HOLD         = 16 | MASK_HELD,
        TAP_DRAG         = 17 | MASK_MOVED,
     */
        /** Pointer has come back down again within a small delta distance and time of a prior tap. */
        DOUBLETAP        = TAP | MASK_REPEAT_DOUBLE,
    /*
        //!!!!TBD TODO:
        TRIPLETAP        = TAP | MASK_REPEAT_TRIPLE,
    */
    #if HANDLE_MULTI_GESTURES
        /* !!!!TBD TODO: a tap with multiple fingers at once */
        MULTI_TAP        = TAP | MASK_MULTI,
        /* !!!!TBD TODO: a pinch/zoom of two fingers coming together, or spreading apart
         * (note I declared it drag+multi for convenience, it will likely
         * want/need to change it before locking in the specification) */
        MULTI_ZOOM       = DRAG | MASK_MULTI,
    #endif
        /** @} */
    };
};

//typedef uint16_t NvGestureKindSlot;

/** Object packaging up all the data related to an input event by the user. */
class NvGestureEvent
{
public:
    NvGestureUID uid; /**< A unique ID for this event -- generally an auto-incrementing counter. */
    NvGestureKind::Enum kind; /**< The kind of event that occurred. */
    NvInputEventClass::Enum type; /**< The input device that generated the event: mouse, finger, stylus, etc... */

    /* new fields, matching the user input struct, for button index and other flags */
    uint8_t    index; /**< Storing mouse button, gamepad button, or key identifier for event. */
    //uint8_t    flags;
    
    float   x; /**< x position at the START of the gesture. */
    float   y; /**< y position at the START of the gesture. */

    /* these are overloaded, different gestures will interpret as different values.
        could be things like:
         * DRAG: delta position
         * FLICK: velocities
         * ZOOM: second finger */
    float   dx; /**< delta x value for the gesture. DRAG == delta pos. FLICK == velocity. ZOOM == second finger. */
    float   dy; /**< delta y value for the gesture. DRAG == delta pos. FLICK == velocity. ZOOM == second finger. */   

    /* no timestamp embedded at the moment.  does anyone need it?  could do a
     *  8.8 fixedpoint elapsed seconds dt or something like that. */

    /** Default constructor, for use as simple struct. */
    NvGestureEvent() { /* no-op */ };

    /** General constructor.
        @param intype The input device generating the event
        @param inkind The kind of event
        @param inx The starting x position
        @param inx The starting y position
    */
    NvGestureEvent(NvInputEventClass::Enum intype, NvGestureKind::Enum inkind, float inx, float iny)
    {
        if (inkind == NvGestureKind::PRESS) // change UID only at press
            uid = InternalGetNextUID();
        else // keep existing ID
            uid = lastID;
        kind = inkind;
        type = intype;
        index = 0;
        //flags = 0;
        x = inx;
        y = iny;
        dx = 0;
        dy = 0;
    };

private:
    static NvGestureUID lastID;

    static NvGestureUID InternalGetNextUID()
    {
        lastID++;
        if (lastID > NV_GESTURE_UID_MAX) // some arbitrary cap...
            lastID = 1; // do NOT reset to 0, it's special...
        return lastID;
    };
};

#endif 

