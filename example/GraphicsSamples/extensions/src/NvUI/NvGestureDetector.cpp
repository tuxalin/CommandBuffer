//----------------------------------------------------------------------------------
// File:        NvUI/NvGestureDetector.cpp
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

/*
 * library for tracking standard user input, and converting it to more
 * easily-digested 'Gestures'.
 */

//#define VERBOSE_DEBUG
//#define VERBOSE_LOGGING

#include <NvSimpleTypes.h>
#include "NvFoundationMath.h"

#include "NvAssert.h"
#include "NV/NvTime.h"
#include "NvUI/NvGestureDetector.h"

#define VERBOSE_DEBUG
//#define OVERLY_VERBOSE_DEBUG

#ifdef VERBOSE_DEBUG
#include "NV/NvLogs.h"
#define LOGI_D(...)
//#define LOGI_D LOGI
#ifdef OVERLY_VERBOSE_DEBUG
#define LOGI_OD LOGI
#else
#define LOGI_OD(...)
#endif
#else
#define LOGI_D(...)
#endif

//============================================================================
//fwd decls
static void EnqueueGesture(NvUST time);

//============================================================================
//============================================================================

// TODO any of these master values need to be config/tweak values?

#define GESTURE_DP_ADJUST_FACTOR        (1.f/160.f)
#define GESTURE_MODERN_DPI              (320.f)

// comment this out if you don't want flicks to have extrapolated 'next' velocity value.
//#define GESTURE_FLICK_EXTRAP_VELOCITY

// smallest time window of history needed to calc flick (ALONG with minimum samples!)
#define GESTURE_MIN_FLICK_SAMPLE_TIME   0.015f
// max time window to look at for flick calc
#define GESTURE_MAX_FLICK_SAMPLE_TIME   0.110f // 110ms, approx 6 frames @ 60hz...

// how much distance needed to activate flick?
#define GESTURE_MIN_FLICK_DIST          15.0f
// how much velocity do we need to move to activate velocity flick
#define GESTURE_MIN_FLICK_VELOCITY      50.0f

// velocity over which we determine something freaked...
#define GESTURE_BADVEL                  160000.0f
// velocity over which we clamp
#define GESTURE_MAX_FLICK_VELOCITY      8000.0f
// multiplier over which we 'drop' one axis versus the other, for slow ramps
#define GESTURE_AXIS_FACTOR             10

// TODO make these distances config/tweak values?
// Input values in array order: unset, mouse, touch, stylus.
static int s_gDistanceTap[NvInputEventClass::MAX] =       {16, 8, 20, 15};
static int s_gDistanceDoubletap[NvInputEventClass::MAX] = {32, 12, 100, 50};
static NvInputEventClass::Enum s_gInputType = NvInputEventClass::MOUSE;
// mouse by default, need to call NvGestureSetInputType to change

// TODO make these times config/tweak values?
//#define G_TIME_TAP              0.2f // !!!!TBD no longer used even to filter/debounce??? TODO
#define G_TIME_HOLD             1.0f // !!!!TBD
#define G_TIME_DOUBLETAP        0.5f // !!!!TBD


// these don't likely need to be tweaked.
#define G_MAX_TRAIL_TRACKED     512 // enough for many, many seconds of input...
#define G_MIN_TRAIL_SHIFT       (G_MAX_TRAIL_TRACKED>>2)  // this must be LESS than trail_tracked!
#define G_MAX_TRAIL_EVAL        20  // this must be LESS than trail_tracked, and is max samples to ever look-back


//============================================================================
//============================================================================

//============================================================================
typedef struct {
    uint8_t down;
    uint8_t wasDown;
    NvGestureKind::Enum currG;
    NvGestureKind::Enum lastG;
    NvUST startTime;
    NvUST endTime;    
    NvGestureInputData startInput;
} NvGestureState;
static NvGestureState  s_gestate = {0}, s_prigestate = {0};
static NvGestureTrackedEvent *        s_startEvent;

static NvGestureTrackedEvent s_historyTrail[G_MAX_TRAIL_TRACKED];
static int32_t history_count = 0;

static float s_gestureXVel = 0, s_gestureYVel = 0;

// !!!!TBD TODO have a setter function to expose setting DPI factor
static float s_dpiAdjustFactor = GESTURE_MODERN_DPI * GESTURE_DP_ADJUST_FACTOR;

//============================================================================
//============================================================================
void CheckValidGesture()
{
    float absX = fabsf(s_gestureXVel);
    float absY = fabsf(s_gestureYVel);
    if (absX>GESTURE_BADVEL || absY>GESTURE_BADVEL)
    {
        s_gestureXVel = 0;
        s_gestureYVel = 0;
    }
    else
    {
        if (absX > GESTURE_MAX_FLICK_VELOCITY * s_dpiAdjustFactor)
        {
            int sign = 1;
            if (s_gestureXVel<0) sign = -1;
            s_gestureXVel = GESTURE_MAX_FLICK_VELOCITY * s_dpiAdjustFactor * sign;
        }

        if (absY > GESTURE_MAX_FLICK_VELOCITY * s_dpiAdjustFactor)
        {
            int sign = 1;
            if (s_gestureYVel<0) sign = -1;
            s_gestureYVel = GESTURE_MAX_FLICK_VELOCITY * s_dpiAdjustFactor * sign;
        }

/* disable the axis-factor zeroing for now
        if (absX > GESTURE_AXIS_FACTOR*absY)
            s_gestureYVel = 0;
        else
        if (absY > GESTURE_AXIS_FACTOR*absX)
        {
            s_gestureXVel = 0;
            LOGI_D(">> zeroed X due to Y (%0.2f vs %0.2f)", absX, absY);
        }
*/

/* disable the minimum flick velocity zeroing for now
        if (absX < GESTURE_MIN_FLICK_VELOCITY * s_dpiAdjustFactor)
        {
            s_gestureXVel = 0;
            LOGI_D(">> zeroed X due to velocity");
        }
        if (absY < GESTURE_MIN_FLICK_VELOCITY * s_dpiAdjustFactor)
        {
            s_gestureYVel = 0;
            LOGI_D(">> zeroed Y due to velocity");
        }
*/
    }

    LOGI_D(">> vel clamped [%0.2f, %0.2f]", s_gestureXVel, s_gestureYVel);
}


//============================================================================
// helper function.  would be an initializer method if in C++...
//============================================================================
static void TouchEvent(NvGestureTrackedEvent *ev, NvUST t, const NvGestureInputData* userinput)
{
    ev->time = t;
    ev->elapsed = 0;
    ev->count = 1; // one sample when filled in.
    memcpy(&(ev->input), userinput, sizeof(NvGestureInputData));

    //printf("TE: %d,%d -> %d,%d\n", userinput->x, userinput->y, userinput->x2, userinput->y2);
    //printf("TE (ev): %d,%d\n", ev->input.x, ev->input.y );
}

//============================================================================
//============================================================================
int LooksLikeFlick()
{
//    CheckValidGesture(); // this is really 'check flick'...
    if (s_gestureXVel != 0 || s_gestureYVel != 0) // then something passed...
        return 1;
    return 0;
}

void SetVelocities(float ddx, float ddy)
{
    s_gestureXVel = ddx;
    s_gestureYVel = ddy;
}

#define NVG_ABS(x) (((x)<0) ? -(x) : (x))
#define NVG_SIGN(x) (((x)>0) ? +1 : (((x)<0) ? -1 : 0))

// this computes the 'y' value given an 'x' value, using the
// passed in vec3 of coefficients for the equation y = v[0] + v[1]x + v[2]x^2
float ComputeNewLeastSquaresValue(const nvidia::NvVec3 &vC, float xnext)
{
    return vC[0] + vC[1]*xnext + vC[2]*xnext*xnext;
}

// this function tries to approximate the next y value for an input set of x and y.
// in our case:
// vCoeff = LeastSquaresFit(maxs, elapsed, dist, avgdt);
// x = elapsed time in ms, y = current distance/position on given axis.
nvidia::NvVec3 LeastSquaresFit(int n, float x[], float y[], const float avgdx)
{
    //if (1)
    { // moved this adjustment into the fn so it's shared code for both axes.
        // adjust the last sample to be half weight assuming prior sample was instant vel.
        // first, get the actual delta x
        float dx = x[n-1] - x[n-2];
        // get delta y for that same period.
        float dy = y[n-1] - y[n-2];
        // extrapolate linearly to avgdx delta
        dy *= (avgdx/dx); 
        // find midpoint between y deltas and add in 50/50
        dy += ((y[n-2] - y[n-3]) - dy) * 0.5f;
        // extrapolate back to original offset
        dy *= (dx/avgdx);
        // assign back
        y[n-1] = y[n-2] + dy;
    }

    double sx=0, sx2=0, sx3=0, sx4=0, sxy=0, sx2y=0, sy=0;
    double ix; // temp to simplify
    int i;

    // first build up the sums for the 3x3 matrix for curve calc
    for (i=0; i<n; i++)
    {
        ix = x[i]; // x1
        sx += ix;
        sxy += ix * y[i];

        ix *= x[i]; // so now x2
        sx2 += ix;
        sx2y += ix * y[i];

        ix *= x[i]; // so now x3
        sx3 += ix;

        ix *= x[i]; // so now x4
        sx4 += ix;

        sy += y[i];
    }

    // next, solve for the three variables by calculating |vC| = inv[mA]*|vB|
    nvidia::NvMat33 mA, mInvA;
    nvidia::NvVec3 vB, vC;

    // fill in the matrix and vector
    mA[0][0] = (float)n;
    mA[0][1] = (float)sx;
    mA[0][2] = (float)sx2;
    mA[1][0] = (float)sx;
    mA[1][1] = (float)sx2;
    mA[1][2] = (float)sx3;
    mA[2][0] = (float)sx2;
    mA[2][1] = (float)sx3;
    mA[2][2] = (float)sx4;

    vB[0] = (float)sy;
    vB[1] = (float)sxy;
    vB[2] = (float)sx2y;

    // invert the matrix.
    mInvA = mA.getInverse();

    // multiply matrix times vector
    vC = mInvA * vB;

    // return the vector of coefficients to the equation
    return vC;
}


// try to 'look back' and find what we need.
// this function does the RIGHT THING to properly calculate 
// the velocity values for potential flicks.
// !!!!TBD
// it might need to understand that some elements could have
//  repeat count (hold in place), and that needs to be factored into
//  how we grab samples, deltas, etc.
void NvCalcVelocities()
{
    int i;
    const int startingSampleIdx = history_count - 1;
    const int maxSamplesBack = history_count; // can't go further back than this many history samples.
    float dt = 0, edt = 0;
    int dx=0, dy=0;
    float avgdt=0;
    float fdxdt=0, fdydt=0;
    int totaldx = 0, totaldy = 0;
    float ddt[G_MAX_TRAIL_EVAL] = {0};
    float dist[G_MAX_TRAIL_EVAL] = {0};
    float elapsed[G_MAX_TRAIL_EVAL] = {0}; // elapsed time
    int signx = 0, signy = 0, tmpsign;
    int samplesBack = 1; // the number of samples to review
    int samplesX = 0, samplesY = 0; // split up samples so we can track x/y separately
	//uint32_t fingerCounter = 0;

    // example
    // history_count = 5
    // array indices == 0..4
    // startingSample = 4
    // samplesBack best case = 4 (samples BACK from start) or == startingSample.

    LOGI_D("..........\nCalcVelocities: history_count = %d", history_count);

    // loop until one of the checks exits.
    // we already early-exited if not enough samples in history...
    // if increasing the sample count again would exceed our sampling max, we're done.
    // and, we now exit loop if we have enough samples compared to size of history_count...
    // or if the elapsed time exceeds our maximum window
    for ( samplesBack=1;
            samplesBack < maxSamplesBack
            && samplesBack < G_MAX_TRAIL_EVAL
            && edt < GESTURE_MAX_FLICK_SAMPLE_TIME;
          samplesBack++ )
    {
        const int currIndex = startingSampleIdx-samplesBack;
        // store the delta time and elapsed time
        dt = NvTimeDiffInSecs(s_historyTrail[currIndex+1].time, s_historyTrail[currIndex].time);
        dt -= s_historyTrail[currIndex].elapsed; // so an event where the finger was still for a period starts when the finger moved..
        ddt[samplesBack-1] = dt;
        edt += dt;
    }

    // check if still not enough data...
    if (edt < GESTURE_MIN_FLICK_SAMPLE_TIME || samplesBack < 2)
    {
        LOGI_D(">> no flick, dt [%0.2f], samples [%d]", edt, samplesBack);
        SetVelocities(0,0);
        return;
    }

    if (samplesBack==2) // just return guessed accel forward.
    {
        dx = s_historyTrail[startingSampleIdx].input.x - s_historyTrail[startingSampleIdx-1].input.x;
        dy = s_historyTrail[startingSampleIdx].input.x - s_historyTrail[startingSampleIdx-1].input.x;
        fdxdt = dx/ddt[0];
        fdydt = dy/ddt[0];
        LOGI_D(">> short flick, velx [%0.2f], vely [%0.2f]", fdxdt, fdydt);
        SetVelocities(dx/ddt[0], dy/ddt[0]);
        return;
    }

    // let's try calcing instant velocities going backwards.  if they increase or remain
    // within epsilon, it's a flick.  if they decrease, it's a stop.
          
	// tbd how to handle multi-finger flicks.  
    //for (fingerCounter = 0; fingerCounter < s_gestate.startInput.fingers; ++fingerCounter)
	{
        float dxdt[G_MAX_TRAIL_EVAL] = {0};
        float dydt[G_MAX_TRAIL_EVAL] = {0};
        int lowVelCountX = 0, lowVelCountY = 0;
                
		// TBD how to deal with multi-finger flick -- really need to track based on finger id, multiple histories?
        //for (i=fingerCounter; i<samplesBack;i+=s_gestate.startInput.fingers)
        for (i=0; i<samplesBack; i++)
        {
            //----------------------------------------------
            // first calculate X history
            dx = s_historyTrail[startingSampleIdx-i].input.x - s_historyTrail[startingSampleIdx-i-1].input.x;
            dxdt[i] = (float)dx / ddt[i];

            tmpsign = NVG_SIGN(dx);
            if (!signx && dx) // then calculate sign.
                signx = tmpsign;
            // small negatives and zeros should drop out.  large sign flips are a transition.
            if (!samplesX && tmpsign && tmpsign!=signx) // this wasn't 0, and sign changed
                samplesX = i+1; // as this one is invalid, we set this as 'max'.
            if (!samplesX)
                totaldx += dx;

            LOGI_OD("   [%02d]  vel %0.1f    (%d, %0.3f)", startingSampleIdx-i, dxdt[i], dx, ddt[i]);
            if (NVG_ABS(dxdt[i]) < GESTURE_MIN_FLICK_VELOCITY * s_dpiAdjustFactor)
            {
                lowVelCountX++;
                LOGI_OD(">> low velocity dxdt[%d] = %0.2f", i, dxdt[i]);
            }

            //----------------------------------------------
            // then calculate Y history...
            dy = s_historyTrail[startingSampleIdx-i].input.y - s_historyTrail[startingSampleIdx-i-1].input.y;
            dydt[i] = (float)dy / ddt[i];

            tmpsign = NVG_SIGN(dy);
            if (!signy && dy) // then calculate sign.
                signy = tmpsign;
            if (!samplesY && tmpsign && tmpsign!=signy) // this wasn't 0, and sign changed
                samplesY = i+1; // as this one is invalid, we set this as 'max'.
            if (!samplesY)
                totaldy += dy;

            if (NVG_ABS(dydt[i]) < GESTURE_MIN_FLICK_VELOCITY * s_dpiAdjustFactor)
            {
                lowVelCountY++;
                LOGI_OD(">> low velocity dydt[%d] = %0.2f", i, dydt[i]);
            }
        }

        if (!samplesX)
            samplesX = samplesBack;
        if (!samplesY)
            samplesY = samplesBack;

        // !!!!!TBD TODO update to use dpi factor...
        if ((NVG_ABS(totaldx) < GESTURE_MIN_FLICK_DIST * s_dpiAdjustFactor) &&
            (NVG_ABS(totaldy) < GESTURE_MIN_FLICK_DIST * s_dpiAdjustFactor) )
        {
            LOGI_D(">> no flick: didn't achieve min distance either direction");
            SetVelocities(0,0);
            return;
        }

        nvidia::NvVec3 vCoeff;
        float nextdist;
        float nexttime;
        float fdelta;
        int maxs = 0;

        if (samplesX<2)
        {
            LOGI_D(">> no X flick, %d samples post eval", samplesX);
            fdxdt = 0;
        }
        else
        {
            maxs = samplesX;
            //int maxs = (samplesX<5?samplesX:5);
            // swap order of vel array and calc elapsed time array
            for (i=0; i<maxs; i++)
            {  
                const int tmpi = maxs-1 - i;
                dist[i] = (float)(s_historyTrail[startingSampleIdx-tmpi].input.x);
                if (!i)
                    elapsed[i] = 0;
                else
                    elapsed[i] = elapsed[i-1] + (ddt[tmpi] ? ddt[tmpi] : 0.005f); // so never a nonzero advancement.
                LOGI_OD("   [%02d] xpos %0.0f  (%0.3f)", i, dist[i], elapsed[i]);
            }

            // !!!!TBD must look at final sample to see if it is SANE.  if it is ZERO motion for any reason, that'll
            // screw things up and add NANs into the system.
            if (ddt[0]<=0.002f || dist[maxs-1]==dist[maxs-2]) // if final release has a near-zero dt or near-zero dx, drop it.
            // try ALWAYS dropping last sample AGAIN...
                maxs--; // drop down one sample.

            if (maxs<3)
            {
                if (maxs<2)
                // !!!!TODO FIX THIS SO WE CALC WITH WHATEVER WE HAVE!!!
                    fdxdt = 0;
                else
                {
                    fdelta = dist[1] - dist[0];
                    fdxdt = fdelta/elapsed[1];
                }
                LOGI_D(">> quick X flick [%0.2f] (%d)", fdxdt, maxs);
            }
            else
            {
                avgdt = elapsed[maxs-2]/(maxs-2); // avg without second to last sample.
                if (avgdt > 1/60.0f)
                    avgdt = 1/60.0f;
                LOGI_D(">> avgdt = %0.3f", avgdt);

                vCoeff = LeastSquaresFit(maxs, elapsed, dist, avgdt);
                i = 2; // 1 would be looking AHEAD, 2 is 'revisiting' final time.
                nexttime = elapsed[maxs-i]+avgdt;
                nextdist = ComputeNewLeastSquaresValue(vCoeff, nexttime);
                fdxdt = (nextdist - dist[maxs-i])/avgdt;
                LOGI_D(">> leastsq X count %d, pos %0.2f, vel %0.3f", maxs, nextdist, fdxdt);            
            }
        }

        if (samplesY<2)
        {
            LOGI_D(">> no Y flick, %d samples post eval", samplesX);
            fdydt = 0;
        }
        else
        {
            maxs = samplesY;
            //int maxs = (samplesY<5?samplesY:5);
            // swap order of vel array and calc elapsed time array
            for (i=0; i<maxs; i++)
            {  
                const int tmpi = maxs-1 - i;
                dist[i] = (float)(s_historyTrail[startingSampleIdx-tmpi].input.y);
                if (!i)
                    elapsed[i] = 0;
                else
                    elapsed[i] = elapsed[i-1] + (ddt[tmpi] ? ddt[tmpi] : 0.001f); // so never a nonzero advancement.
                //LOGI_D("   [%02d] ypos %0.0f  (%0.3f)", i, dist[i], elapsed[i]);
            }

            // !!!!TBD must look at final sample to see if it is SANE.  if it is ZERO motion for any reason, that'll
            // screw things up and add NANs into the system.
            if (ddt[0]<=0.002f || dist[maxs-1]==dist[maxs-2]) // if final release has a zero dt or zero dx, drop it.
                maxs--; // drop down one sample.

            if (maxs<3)
            {
                if (maxs<2)
                    fdydt = 0;
                else
                {
                    fdelta = dist[1] - dist[0];
                    fdydt = fdelta/elapsed[1];
                }
                LOGI_D(">> quick Y flick [%0.2f] (%d)", fdydt, maxs);
            }
            else
            {
                avgdt = elapsed[maxs-2]/(maxs-2); // avg without second to last sample.
                if (avgdt > 1/60.0f)
                    avgdt = 1/60.0f;
                //LOGI_D(">> avgdt = %0.3f", avgdt);

                vCoeff = LeastSquaresFit(maxs, elapsed, dist, avgdt);
                i = 2; // 1 would be looking AHEAD, 2 is 'revisiting' final time.
                nexttime = elapsed[maxs-i]+avgdt;
                nextdist = ComputeNewLeastSquaresValue(vCoeff, nexttime);
                fdydt = (nextdist - dist[maxs-i])/avgdt;
                LOGI_D(">> leastsq Y count %d, pos %0.2f, vel %0.3f", maxs, nextdist, fdydt);            
            }
        }
    }
       
    SetVelocities(fdxdt, fdydt);

    // LooksLikeFlick used to do this, now we do for internal validity.
    CheckValidGesture(); 
}


//============================================================================
//============================================================================
static void SetGestateGesture(NvUST t, const NvGestureKind::Enum kind)
{
    s_gestate.currG = kind;
#if defined(VERBOSE_LOGGING)
    {
        float timeSinceWorldBegan = NvTimeDiffInSecs(t, 0);
        LOGI_D("!> SGG: g[0x%0x] t[%0.2f]", (uint32_t)kind, timeSinceWorldBegan);
    }
#endif
}


//============================================================================
//============================================================================
static void PrepGestate(NvUST t, const NvGestureInputData* userinput)
{
    // first, back up prior gesture, so we can do doubletap, tap-ZZZ.
    memcpy(&s_prigestate, &s_gestate, sizeof(s_gestate));
    // clear state.
    s_gestate.currG = NvGestureKind::NONE; // TODO should this one use SetGestateGesture(t, NvGestureKind::NONE)??
    s_gestate.lastG = NvGestureKind::NONE;
    s_gestate.startTime = t;
    s_gestate.endTime = 0;
    memcpy(&(s_gestate.startInput), userinput, sizeof(NvGestureInputData));

    history_count = 1; // since that's the basis for a new event.
}


static void TrackInput(NvUST t, const NvGestureInputData* userinput)
{
    // if our history greater than some cap, eliminate a chunk and reset count.
    if (history_count+1>=G_MAX_TRAIL_TRACKED)
    { // !!!!TBD THIS IS REALLY UNSAFE.  NEED TO ONLY PUSH BACK TO LAST AGGREGATABLE EVENT.
        memcpy( &s_historyTrail[history_count-G_MIN_TRAIL_SHIFT*2],
                &s_historyTrail[history_count-G_MIN_TRAIL_SHIFT],
                sizeof(NvGestureTrackedEvent)*G_MIN_TRAIL_SHIFT );
        history_count -= G_MIN_TRAIL_SHIFT;
    }
                    
/*
    [0]-[19], 20 samples.
    20-5*2 == [10] 11th sample overwritten?
    20-5 == [15]
    20-5 == 15 samples, [14] is last one.
*/
                    
    // fill in new event
    TouchEvent(&s_historyTrail[history_count], t, userinput);
    if (userinput->countDown==1 && (s_gestate.currG&NvGestureKind::MASK_MULTI)) // use REAL fingers count
    { // duplicate first finger data when WAS multi, now single.
        s_historyTrail[history_count].input.x2 = s_historyTrail[history_count].input.x;
        s_historyTrail[history_count].input.y2 = s_historyTrail[history_count].input.y;
    }
    history_count++;
}

//============================================================================
// at end of func, if s_gestate.entTime!=0 we reached end of processing of one
// gesture, and s_gestate.lastG should be the gesture code.
// !!!!TBD TO ADD MULTITOUCH GESTURE DETECTION!!!
//============================================================================
void NvGestureAddInput(NvUST t, const NvGestureInputData* userinput)
{
    int exceededDoubleTapRanges = 0;
    
    // in case timestamp is zero, let's get a system time value.
    if (t==0) t = NvTimeGetTime();

    // ANY code checking finger count should check s_gestate.down instead
    // so that we can override the count in special cases and make other
    // blocks think the count is different than 'actual'...
    s_gestate.down = 0;
    if (userinput)
        s_gestate.down = userinput->countDown; // so anything querying this frame knows immediate state.

    // cursor wasn't down previously
    if (!s_gestate.wasDown)
    {        
        if (s_gestate.lastG==NvGestureKind::TAP
#ifdef HANDLE_MAYBE_GESTURES
            || s_gestate.lastG==NvGestureKind::MAYBE_TAP
#endif
            )
        {
            int addx,addy;
            addx = abs(userinput->x - s_gestate.startInput.x);
            addy = abs(userinput->y - s_gestate.startInput.y);
            // if exceeded double-tap distance or time
            if (addx > s_gDistanceDoubletap[s_gInputType] ||
                addy > s_gDistanceDoubletap[s_gInputType]
            ||  NvTimeDiffInSecs(t, s_gestate.startTime) > G_TIME_DOUBLETAP)
                exceededDoubleTapRanges = 1;
        }
        else
#ifdef HANDLE_MAYBE_GESTURES
        if (!(s_gestate.lastG==NvGestureKind::MAYBE_TAP))
#endif
        { // if last state was a release state, AND we're now back in here, copy and clear.
            if (s_gestate.lastG & NvGestureKind::MASK_RELEASED) // last state was a released state
                if (s_gestate.lastG != NvGestureKind::NONE || s_gestate.currG != NvGestureKind::NONE)
                    PrepGestate(t, userinput);
        }

        if (s_gestate.down) // down now, start of new gesture boundary
        {
            NvGestureKind::Enum startG = NvGestureKind::PRESS;
#ifdef HANDLE_PLUS_GESTURES
            if (!exceededDoubleTapRanges // then we're within doubletap ability, in tap-plus land..
            &&  (s_gestate.lastG==NvGestureKind::TAP
#ifdef HANDLE_MAYBE_GESTURES    
                || s_gestate.lastG==NvGestureKind::MAYBE_TAP
#endif
                ))
            {
                startG = NvGestureKind::TAP_PLUS_PRESS;
                // s_gestate.wasDown = 1; // don't muck with this right now.
            }
#endif

#ifdef HANDLE_MAYBE_GESTURES
            if (s_gestate.lastG==NvGestureKind::MAYBE_TAP)
            {
                // if exceeded double-tap time, or mouse came up and we got inputs
                // beyond tap range, queue up the tap gesture for the prior event.
                if (exceededDoubleTapRanges) // check of doubletap ranges above.
                {
                    SetGestateGesture(t, NvGestureKind::TAP);
                    s_gestate.lastG = s_gestate.currG; // since we're 'ending' with this change...
                    s_gestate.endTime = t; // we're done with this.
                    EnqueueGesture(t); // everything else is retrieved from gesture system state.
                }
            }
#endif

            if (s_gestate.lastG != NvGestureKind::NONE || s_gestate.currG != NvGestureKind::NONE)
                PrepGestate(t, userinput);
            
            // fill in start event.
            s_startEvent = &s_historyTrail[0];
            TouchEvent(s_startEvent, t, userinput);

            SetGestateGesture(t, startG);
            s_gestate.lastG = NvGestureKind::NONE;
            s_gestate.startTime = t;
            s_gestate.endTime = 0;
            memcpy(&(s_gestate.startInput), userinput, sizeof(NvGestureInputData));
            
            history_count = 1; // since it's in the s_historyTrail...

            // !!!!TBD anything else?
        }
        else // still not down
        {
#ifdef HANDLE_MAYBE_GESTURES
            // extension of prior event.  if event was MAYBE_TAP, wait until dbltime passes to mark it tap instead.
            // and will THEN generate an event! ;)
            if (s_gestate.lastG==NvGestureKind::MAYBE_TAP)
            {
                // if exceeded double-tap time, or mouse came up and we got inputs
                // beyond tap range, generate the tap gesture.
                if (exceededDoubleTapRanges) // check of doubletap ranges above.
                    SetGestateGesture(t, NvGestureKind::TAP);
                else
                { // check for exceeding single-tap distance.
                    int adx, ady;
                    adx = abs(userinput->x - s_startEvent->input.x);
                    ady = abs(userinput->y - s_startEvent->input.y);
                    if (adx > s_gDistanceTap[s_gInputType] ||
                        ady > s_gDistanceTap[s_gInputType] )
                        SetGestateGesture(t, NvGestureKind::TAP);
                }
            }
            else // !!!!TBD do we switch gesture state here to _NONE?
 #endif
            if (s_gestate.lastG & NvGestureKind::MASK_RELEASED) // last state was a released state
            {
                PrepGestate(t, userinput);
                SetGestateGesture(t, NvGestureKind::NONE); // this is in dup, but explicit.                

                s_startEvent = &s_historyTrail[0];
                TouchEvent(s_startEvent, t, userinput);
            }
            else
            if (s_gestate.currG == NvGestureKind::NONE)
            {
                // for the moment, when currG is none, we want to ensure:
                // 1) we have no history trail, one element always.
                // 2) current input values are passed through to BOTH input storage locs.
                // this is just for sanity, in theory should already be this...
                s_startEvent = &s_historyTrail[0];
                if (s_gestate.lastG == NvGestureKind::NONE)
                    s_startEvent->count++; // just for bookkeeping.
                    
                // we know not down now.
                // if not down prior, if last gesture none, if input same, return w/o enqueu.
                if (!s_gestate.wasDown 
                &&  s_gestate.lastG == NvGestureKind::NONE
                &&  s_gestate.startInput.x == userinput->x
                &&  s_gestate.startInput.y == userinput->y
                    ) // then no change, return.
                {
                    return; // so we don't enqueue NONE @ same x/y repeatedly.
                }
                else
                {
                    // just copy new locations.
                    TouchEvent(s_startEvent, t, userinput);
                    s_gestate.startInput.x = userinput->x;
                    s_gestate.startInput.y = userinput->y;
                }
            }
        }
    }

    // separate if here as we MIGHT want to change wasDown internally above for tricky transitions...
    if (s_gestate.wasDown) // cursor was down
    { // s_startEvent is valid... precompute some deltas.
        //float dt = NvTimeDiffInSecs(t, s_startEvent->time);
        int adx, ady;
        adx = abs(userinput->x - s_startEvent->input.x);
        ady = abs(userinput->y - s_startEvent->input.y);
        
        // if cursor still down AND finger count didn't change on us! !!!!!TBD best thought I have!
        if (s_gestate.down) // && s_gestate.startInput.countDown==userinput->countDown)
        {
#if HANDLE_MULTI_GESTURES // !!!!TBD TODO trying to disable multi for the moment.
            // first, a HACK!!!!TBD
            // if countDown started as 1, but is now >really< 2, reset history to now!
            if (s_gestate.startInput.countDown==1 && userinput->countDown==2)
            {
                NvMemcpy(&(s_gestate.startInput), userinput, sizeof(NvGestureInputData)); // reset
                history_count = 0; // reset/
                // override to think it was zooming already...
				// !!!!TBD maybe we need to look at whether we should be dragging or zooming?
                s_gestate.lastG = NvGestureKind::MULTI_ZOOM;                
            }
            
            // !!!TBD not sure a better way to do this.
            if (s_gestate.startInput.countDown==2
            &&  s_gestate.down != 2) // need to keep it multi.  or release/ignore when second finger is up?? !!!!TBD
                s_gestate.down = 2;
 #endif            
            // if still in PRESS mode, and not outside some delta region, aggregate with earliest event we can
            if (s_gestate.down == 1 // NOT ptr->countDown here!
            && (s_gestate.lastG==NvGestureKind::PRESS || s_gestate.lastG==NvGestureKind::HOLD) // !!!TBD TAP_PRESS stuff?
            && adx < s_gDistanceTap[s_gInputType]
            && ady < s_gDistanceTap[s_gInputType])
            { // !!!!TBD which event to aggregate to?  will it always be the start of the gesture?
                NvGestureTrackedEvent *evToAgg = s_startEvent;
                const float et = NvTimeDiffInSecs(t, evToAgg->time);
                evToAgg->elapsed = et;
                evToAgg->count++; // increment count
                
                if (et > G_TIME_HOLD && s_gestate.lastG!=NvGestureKind::HOLD)
                    SetGestateGesture(t, NvGestureKind::HOLD);
            }
            else // outside region.  start of drag, maybe flick.
            {  // !!!TBD TAP_PRESS stuff?
#ifdef HANDLE_PLUS_GESTURES
                if (s_gestate.currG==NvGestureKind::TAP_PLUS_PRESS)
                {
                    // !!!!TBD
/*
                    // fill in new event
                    TouchEvent(&s_historyTrail[history_count], t, userinput);
                    if (userinput->countDown==1 && (s_gestate.currG&NvGestureKind::MASK_MULTI)) // use REAL countDown count
                    { // duplicate first finger data when WAS multi, now single.
                        s_historyTrail[history_count].input.x2 = s_historyTrail[history_count].input.x;
                        s_historyTrail[history_count].input.y2 = s_historyTrail[history_count].input.y;
                    }
                    history_count++;
*/
                }
                else
#endif
#if HANDLE_MULTI_GESTURES // !!!!TBD TODO trying to disable multi for the moment.
                if (s_gestate.down==2 // NOT ptr->countDown here!
                ||  s_gestate.lastG==NvGestureKind::MULTI_ZOOM)
                    SetGestateGesture(t, NvGestureKind::MULTI_ZOOM);
                else
#endif
                    SetGestateGesture(t, NvGestureKind::DRAG);
                
#if HANDLE_MULTI_GESTURES // !!!!TBD TODO trying to disable multi for the moment.
                if (s_gestate.currG == NvGestureKind::MULTI_ZOOM
                &&  userinput->countDown != 2) // REAL finger count
                {
                    // noop -- we don't add event in this case.
                }
                else // add the event...
#endif
                {
                    TrackInput(t, userinput);
                }
            }
        }
        else //!cursor.down == not cursor still down == lifted up.  capture the transition, and generate any gesture based on the lift-up.
        {
            // not sure why it wasn't, and was working,
            // but this should be set here so that any processing
            // knows the end time of the current gesture.
            s_gestate.endTime = t; // we're done with this.
#ifdef DEBUG_PRINTFS            
            fprintf(stderr, "\nPENUP: Captured %d samples in %0.2f secs (%f/s)...\n", history_count, dt, history_count/dt);
            fprintf(stderr, "PENUP: total dx/dy = (%0.2f,%0.2f) == (%0.2f,%0.2f)/s...\n", dx, dy, s_gestureXVel, s_gestureYVel);
#endif
           
// then we didn't change to hold or drag, if time small it's a tap.
// TODO !!!!TBD
// actually... some of the logic is lost here.
// IF we didn't already change to a hold (1s) or drag (moved out of tap range)
// THEN aren't we by default a tap?  If we didn't hold long enough, and didn't
// move, triggering anything OTHER than tap/maybetap seems WRONG!!
            if (s_gestate.lastG == NvGestureKind::PRESS
#ifdef HANDLE_PLUS_GESTURES
            || s_gestate.lastG == NvGestureKind::TAP_PLUS_PRESS
#endif
                )
            {
                
                // look for potential double-tap.
#if latertodo
                float dt = NvTimeDiffInSecs(s_gestate.endTime, s_gestate.startTime);
                if (dt <= G_TIME_TAP)
#endif
                {
                    if (
                         ( s_prigestate.lastG==NvGestureKind::TAP
#ifdef HANDLE_MAYBE_GESTURES
                        || s_prigestate.lastG==NvGestureKind::MAYBE_TAP
#endif
#ifdef HANDLE_PLUS_GESTURES
                        || s_gestate.lastG == NvGestureKind::TAP_PLUS_PRESS // no need to look at prior gesture
#endif
                         )
                        &&
                         (NvTimeDiffInSecs(s_gestate.startTime, s_prigestate.endTime) < G_TIME_DOUBLETAP)
                       )
                    {
                        // we matched state and time, how about distance?
                        int addx,addy;
                        addx = abs(s_gestate.startInput.x - s_prigestate.startInput.x);
                        addy = abs(s_gestate.startInput.y - s_prigestate.startInput.y);
                        if (addx < s_gDistanceDoubletap[s_gInputType] &&
                            addy < s_gDistanceDoubletap[s_gInputType])
                            SetGestateGesture(t, NvGestureKind::DOUBLETAP); // implies release
                    }
                }
                
                // if we didn't declare this a doubletap, it's a regular tap.
                // NEVER can be just a release, right?  TODO!  need eyes to review.
                if (s_gestate.currG != NvGestureKind::DOUBLETAP) // didn't set yet...
                {
#ifdef HANDLE_MAYBE_GESTURES
                    SetGestateGesture(t, NvGestureKind::MAYBE_TAP); // implies release
#else
                    SetGestateGesture(t, NvGestureKind::TAP); // implies release
#endif
                }

#if latertodo
    // don't think we need this any more...
                // if we STILL haven't flagged a change from PRESS, we just signal RELEASE.
                if (s_gestate.currG == NvGestureKind::PRESS)
                {
                    SetGestateGesture(t, NvGestureKind::RELEASE);
                    // !!!!TBD temp hack to integrate old and new systems.
                    s_gestureXVel = s_gestureYVel = 0;
                }
#endif
            }
            else // could be HOLD or DRAG
            {
                bool handled = false;

                if (!(s_gestate.currG & NvGestureKind::MASK_MULTI)) // !!!!TBD for the moment, no multi-flick!!
                {
                    // don't add release as it seems to be same as prior event.
                    //TrackInput(t, userinput);
                    // run velocity calc
                    NvCalcVelocities();
                    // check for flick velocities
                    if (LooksLikeFlick())
                    {
                        SetGestateGesture(t, NvGestureKind::FLICK); // implies release
                        handled = true;
                        //printf("Flick: (%0.2f,%0.2f)\n", s_gestureXVel, s_gestureYVel);
                    }
                }

                if (!handled) // hold and drag and multizoom need to be released...
                {
                    SetGestateGesture(t, NvGestureKind::RELEASE);
                    // !!!!TBD temp hack to integrate old and new systems.
                    s_gestureXVel = s_gestureYVel = 0;
                }
            }

			// NVDHC: I think UIC added this.  need to ensure it's right...
			//Update history so it is accurate to the mouse position.
			if ( history_count )
			{
				s_historyTrail[0].input.x = userinput->x;
				s_historyTrail[0].input.y = userinput->y;
			}

			// !!!!!TBD TODO this was in UIC version, but not clear we should do this here.
            //s_gestate.endTime = t; // we're done with this.
	    } // end !cursor.down
    } // end cursor.wasDown   	        

    // update state for next time through...
    s_gestate.lastG = s_gestate.currG;
    s_gestate.wasDown = s_gestate.down; 
    EnqueueGesture(t); // everything else is retrieved from gesture system state.
}


//============================================================================
//============================================================================
bool NvGestureTick(NvUST t)
{
    // extensions of prior events.
    
    // if event was press, and holdtime passed, generate hold.
    
    if (t!=0)
    {
        float et = NvTimeDiffInSecs(t, s_gestate.startTime);
        if (s_gestate.lastG==NvGestureKind::PRESS) // || s_gestate.lastG==NvGestureKind::HOLD)
        {
            if (et > G_TIME_HOLD)
            {
                SetGestateGesture(t, NvGestureKind::HOLD);
                s_gestate.lastG = s_gestate.currG; // since we're enqueuing
                EnqueueGesture(t); // everything else is retrieved from gesture system state.
                return true;
            }
        }
#ifdef HANDLE_MAYBE_GESTURES
        else
        if (s_gestate.lastG==NvGestureKind::MAYBE_TAP)
        {
            if (et > G_TIME_DOUBLETAP)
            {
                SetGestateGesture(t, NvGestureKind::TAP);
                s_gestate.lastG = s_gestate.currG; // since we're enqueuing
                EnqueueGesture(t); // everything else is retrieved from gesture system state.
                return true; // state changed!!
            }
        }
#endif
        else
        if (s_gestate.lastG & NvGestureKind::MASK_RELEASED)
        { // !!!!TBD
        }
    }
	else
	{
	}
    
    return false; // state didn't change
}


//============================================================================
//============================================================================
int NvGestureGetTouchCount()
{
    return history_count;
}


//============================================================================
//============================================================================
/*
NvGestureKind::Enum NvGestureGetGestureEvent(NvGestureTrackedEvent *ev)
{
    NvGestureGetHistoryEvent(0, ev);
    return(s_gestate.lastG);
}
*/

//============================================================================
//============================================================================
int32_t NvGestureGetHistoryEvent(int32_t n, NvGestureTrackedEvent *ev)
{
    if (history_count == 0)
        return 0;
    if (n == -1) // last event...
        n = history_count-1;
    else
    if (n>=history_count)
        return 0;

    memcpy(ev, &(s_historyTrail[n]), sizeof(NvGestureTrackedEvent));
	return 1;
}

//============================================================================
//============================================================================
NvUST NvGestureGetStartTime()
{
    if (history_count==0)
        return 0;
    return s_historyTrail[0].time;
}

//============================================================================
//============================================================================
NvUST NvGestureGetLastTime()
{
    if (history_count==0)
        return 0;
    return s_historyTrail[history_count-1].time;
}

//============================================================================
//============================================================================
// TODO !!!!TBD this is potentially slow,
// but just trying to get things on-par with old code.
float NvGestureGetElapsedTime()
{   
    if (history_count<=1) // if only one, no delta time...
        return 0;
    return NvTimeDiffInSecs(NvGestureGetLastTime(), NvGestureGetStartTime());
}


//============================================================================
//============================================================================
float NvGestureGetXDist()
{
    if (history_count<=1) // no distance if only one history element
        return 0;
    else
#if HANDLE_MULTI_GESTURES
    if (s_gestate.currG == NvGestureKind::MULTI_ZOOM)
    {
        // dx at start
        int dx1 = NvAbs(s_historyTrail[0].input.x2 - s_historyTrail[0].input.x);
        // dx now
        int dx2 = NvAbs(s_historyTrail[history_count-1].input.x2 - s_historyTrail[history_count-1].input.x);
        return (float)(dx2 - dx1); // the delta change from start to now, can be negative.
    }
    else
#endif
    if (/*s_gestate.currG == NvGestureKind::TAP || */ s_gestate.currG == NvGestureKind::DOUBLETAP)
    {
        float dx = (float)(s_gestate.startInput.x - s_prigestate.startInput.x);
        //printf("dx = %d\n", dx);
        return dx;
    }
    else
    {
        float dx = (float)(s_historyTrail[history_count-1].input.x - s_historyTrail[0].input.x);
        return dx;
    }
}


//============================================================================
//============================================================================
float NvGestureGetYDist()
{        
    if (history_count<=1)
        return 0;
    else
#if HANDLE_MULTI_GESTURES
    if (s_gestate.currG == NvGestureKind::MULTI_ZOOM)
    {
        // dy at start
        int dy1 = NvAbs(s_historyTrail[0].input.y2 - s_historyTrail[0].input.y);
        // dy now
        int dy2 = NvAbs(s_historyTrail[history_count-1].input.y2 - s_historyTrail[history_count-1].input.y);
        return (float)(dy2 - dy1); // the delta change from start to now, can be negative.
    }
    else
#endif
    if (/*s_gestate.currG == NvGestureKind::TAP || */ s_gestate.currG == NvGestureKind::DOUBLETAP)
    {
        float dy = (float)(s_gestate.startInput.y - s_prigestate.startInput.y);
        //printf("dy = %d\n", dy);
        return dy;
    }
    else
    {
        float dy = (float)(s_historyTrail[history_count-1].input.y - s_historyTrail[0].input.y);
        return dy;
    }
}


//============================================================================
//============================================================================
float NvGestureGetXVel()
{
    return s_gestureXVel;
}

//============================================================================
//============================================================================
float NvGestureGetYVel()
{
    return s_gestureYVel;
}


//============================================================================
//============================================================================
#define MAX_GESTURE_QUEUE_LEN   256 // !!!!TBD TODO this was 16, UIC had 256, seems high?
static NvGestureQueueEvent s_gqPool[MAX_GESTURE_QUEUE_LEN];
static NvGestureQueueEvent *s_gqFreeList = NULL; // this is the free gq
static NvGestureQueueEvent *s_gestureQueue = NULL; // this is the active gq

//============================================================================
//============================================================================
static NvUST s_lastGestureTime = 0;
static NvGestureUID s_lastGestureID = 0;
static const NvGestureUID c_maxGestureID = 1<<10; // something less than max

static NvGestureUID s_ignoreGestureID = NV_GESTURE_UID_INVALID;
static NvGestureKind::Enum s_ignoreGesture = NvGestureKind::INVALID;


//============================================================================
// private internal fn for allocating a new GQEvent.  because
// we pool GQEvents, we LEAK at shutdown for now.. !!!!TBD
//============================================================================
static NvGestureQueueEvent *AllocGQEvent()
{
    NvGestureQueueEvent *gq = NULL;
    
	// init the list until we have another method to do so...
	if (s_gqFreeList==NULL)
	{
		// link up the pool to the free list.
		s_gqFreeList = &(s_gqPool[0]);
		for (int i=0; i<MAX_GESTURE_QUEUE_LEN-1; i++)
			s_gqPool[i].next = &(s_gqPool[i+1]);
		s_gqPool[MAX_GESTURE_QUEUE_LEN-1].next = NULL; // for sanity.
	}

	// pop off the head of the list.
    gq = s_gqFreeList;
    s_gqFreeList = gq->next;
    gq->next = NULL;
    return gq;
}


//============================================================================
//============================================================================
void NvFreeGQEvent(NvGestureQueueEvent *gq)
{
    if (gq != NULL)
    { // add to the head of the free list.
        gq->next = s_gqFreeList;
        s_gqFreeList = gq;
    }
}


//============================================================================
//============================================================================
void NvGestureCleanup()
{
    // since it's internal pool, just null the ptr.
    s_gqFreeList = NULL;
    s_gestureQueue = NULL;
}


//============================================================================
//============================================================================
void NvIgnoreGesture(const NvGestureKind::Enum gesture)
{
    s_ignoreGestureID = s_lastGestureID;
    s_ignoreGesture = gesture;
}

//============================================================================
// helper that builds a NvGestureEvent, fills it, queues it.
//============================================================================
static void EnqueueGesture(NvUST time)
{
    const NvGestureTrackedEvent *currEvent = &(s_historyTrail[0]);
    const NvGestureKind::Enum gesture = s_gestate.lastG;
    float dx, dy;
    NvGestureQueueEvent *gq;

    if (s_lastGestureTime != currEvent->time
    &&  gesture == NvGestureKind::PRESS) // we only change ID at new PRESS
    {
        s_ignoreGestureID = s_lastGestureID; // guaranteed to be different in a sec!
        s_ignoreGesture = NvGestureKind::NONE; // just none to clear...
        
        s_lastGestureTime = currEvent->time;
        s_lastGestureID++;
        if (s_lastGestureID > c_maxGestureID)
            s_lastGestureID = 0;

#ifdef VERBOSE_LOGGING
        printf("gesture:newid\n");
#endif
    }
    
    // allow us to ignore THIS gesture ID until a change in gesture kind occurs...
    if (s_ignoreGestureID == s_lastGestureID &&
            (s_ignoreGesture == NvGestureKind::INVALID || s_ignoreGesture == gesture) )
    {
#ifdef VERBOSE_LOGGING
        printf("gesture:ignoringID\n");
#endif
        return;
    }
    
//    if (gesture == NvGestureKind::NONE) // nothing to do
//        return;

    // !!!!!TBD I'm not encoding/handling multi-touch gestures yet!
    // !!!!TBD should add in further sanity check here that gesture isn't generating
    // values greater than 16bit signed int.
    if (gesture == NvGestureKind::FLICK) // pack up velocities
    {
        dx = NvGestureGetXVel();
        dy = NvGestureGetYVel();
    }
    else // pack up dx/dy
    {
        dx = NvGestureGetXDist();
        dy = NvGestureGetYDist();
    }

    // handle situation where we somehow, some way, run out
    // of queue elements.  !!!!TBD TODO dynamic realloc would
    // 'fix' this problem.  but then we'd need a real cleanup fn...
	if (NULL == (gq = AllocGQEvent()))
	{
		// !!!!TBD TODO HACK HACK
		// we'll eat the event to not die!
#ifdef _DEBUG
    	NV_ASSERT(s_gqFreeList); // will tell us free list empty.
#endif
		return; // !!!!TBD
	}

    gq->next = NULL; // sanity.        
    gq->time = time;
    gq->gesture.uid = s_lastGestureID;
    gq->gesture.kind = gesture; // casting to/from the enumerants.
    gq->gesture.x = (float)(currEvent->input.x);
    gq->gesture.y = (float)(currEvent->input.y);
    gq->gesture.dx = dx;
    gq->gesture.dy = dy;
    // TODO !!!!TBD not sure if this is proper, but we need something for the moment.
    gq->gesture.index = currEvent->input.index;
    // !!!!TBD TODO
    //gq->gesture.flags = currEvent->input.flags;

#ifdef VERBOSE_LOGGING
    if (gesture!=NvGestureKind::NONE) // just for sanity in this logging...
        printf("gesture %d: [0x%x] %d,%d => %d,%d\n", gq->gesture.uid, gq->gesture.kind, gq->gesture.x, gq->gesture.y, gq->gesture.dx, gq->gesture.dy);
#endif

    // now we queue the gesture at the tail of the list...
    // !!!!TBD array was better for this O(n) insert.
    // !!!!TBD TODO add a gq_tail?
    if (s_gestureQueue == NULL)
        s_gestureQueue = gq;
    else
    { // add new element
        NvGestureQueueEvent *nq = s_gestureQueue;
        while (nq->next)
            nq = nq->next;
        nq->next = gq;
    }

    /* only post gestures ONCE, unless a type of drag... */
    if (
         (
           !(gesture & NvGestureKind::MASK_MOVED) // always post moved-class events repeatedly.
           &&
           !(gesture == NvGestureKind::NONE)  // always post move==none -- should tag as move-class??  !!!!TBD TODO
         )
         ||
         (
           (gesture & NvGestureKind::MASK_RELEASED) // only post release-class gestures ONCE.
         )
       )
    {
        NvIgnoreGesture(gesture);
#ifdef VERBOSE_LOGGING
        printf(">>> IGNORE SET for gesture %d kind 0x%x (maskbase 0x%x)\n", s_ignoreGestureID, s_ignoreGesture, (s_ignoreGesture & NvGestureKind::MASK_BASE));
#endif
    }
}


//============================================================================
//============================================================================
NvGestureQueueEvent *NvGestureGetNextEvent(void)
{
    NvGestureQueueEvent *gev = NULL;
    if (s_gestureQueue)
    {
        gev = s_gestureQueue;
        s_gestureQueue = gev->next;
    }   
    return gev;
}


//============================================================================
//============================================================================
void NvGestureSetInputType(NvInputEventClass::Enum type)
{
    if (type>=NvInputEventClass::MAX)
        return; // invalid.
    s_gInputType = type;
}

