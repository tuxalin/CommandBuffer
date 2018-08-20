//----------------------------------------------------------------------------------
// File:        NvUI/NvGestureDetector.h
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

#ifndef _NV_GESTURE_H
#define _NV_GESTURE_H

#include <NvSimpleTypes.h>

#include <NvUI/NvGestureEvent.h>
#include <NV/NvTime.h>

#if later
class NvGestureDetector
{
    /** The main function for adding finger/mouse/stylus input into the gesture system. */
    virtual void NvGestureAddInput(NvUST t, const NvGestureInputData* userinput) = 0L;

    /** Helper function for 'ticking' the gesture system periodically when no events have occurred for a period. */
    virtual bool NvGestureTick(NvUST t);
};

class NvSimpleGestureDetector
{
};

class NvComplexGestureDetector
{
};
#endif

/** The main structure for passing input events into the gesture system.

    This structure encapsulates single or multi touch inputs in one
    structure, with the current limitation being two simultaneous inputs.
    Next, adding support for mouse buttons and extra status flags.
 */
typedef struct _NvGestureInputData
{
    uint8_t        countDown; /**< Number of fingers/inputs down for this sample: 0, 1, or 2. */
    int8_t         width; /**< A device-specific 'grading' of the relative size of the 'hit area'. */
    uint8_t        index; /**< Indicator of non-primary button/hand.  So for mouse, left/right/center is 0/1/2. */
    uint8_t        flags; /**< Any alternate state flags, such as chording keys/buttons while dragging. */
    int32_t        x; /**< Input 1 x position */
    int32_t        y; /**< Input 1 y position */
    int32_t        x2; /**< Input 2 x position or input 1 z position*/
    int32_t        y2; /**< Input 2 y position */
} NvGestureInputData;


/** The main function for adding finger/mouse/stylus input into the gesture system. */
void NvGestureAddInput(NvUST t, const NvGestureInputData* userinput);

/** Helper function for 'ticking' the gesture system periodically when no events have occurred for a period. */
bool NvGestureTick(NvUST t);

/** Helper function to externally set the existing gesture to be ignored */
void NvIgnoreGesture(const NvGestureKind gesture);

/** Structure for keeping a timestamped-queue of gesture event entries. */
class NvGestureQueueEvent
{
public:
    NvGestureQueueEvent()
    {
        time = 0;
        //gesture = ???
        next = NULL;
    };
    NvUST time; /**< the timestamp this event was queued up at... */
    NvGestureEvent gesture; /**< the gesture at this time */
    class NvGestureQueueEvent *next;
};

/** Helper function for retrieving any queued events from the system. */
NvGestureQueueEvent *NvGestureGetNextEvent(void);

/** Helper function for freeing returned GQEvents. */
void NvFreeGQEvent(NvGestureQueueEvent *gq);

/** Helper function for cleaning up the gesture system at shutdown. */
void NvGestureCleanup();

void NvGestureSetInputType(NvInputEventClass::Enum type);

/* ======================================================================== */
/* ======================================================================== */

/** The universal structure for tracking touchscreen/input data for gestures. */
typedef struct _NvGestureTrackedEvent
{
    NvUST    time; /**< The timestamp of this sample, in native UST format. */
    float    elapsed; /**< Elapsed seconds since start of a gesture this event is part of. */
    uint32_t count; /**< Number of samples this event represents -- default is one, but can grow when repeated/duplicate input. */
    NvGestureInputData input; /**< A given 'frame' of user input to the system. */
} NvGestureTrackedEvent;

#if later
/** Get "history list" sample count, for debug display. */
int NvGestureGetTouchCount();
/** Get a specific input event from the "history list", for debug display. */
uint32_t NvGestureGetHistoryEvent(uint32_t n, NvGestureTrackedEvent *ev);

NvUST NvGestureGetStartTime();
NvUST NvGestureGetLastTime();
float NvGestureGetElapsedTime();

/* retrieving live 'drag' data... */
float NvGestureGetXDist();
float NvGestureGetYDist();
#endif

/* retrieving quick inferred velocities in pix/sec */
float NvGestureGetXVel();
float NvGestureGetYVel();

#endif /*_NV_GESTURE_H_*/

