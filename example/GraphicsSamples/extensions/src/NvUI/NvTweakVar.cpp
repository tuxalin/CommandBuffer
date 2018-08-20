//----------------------------------------------------------------------------------
// File:        NvUI/NvTweakVar.cpp
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

#include "NvUI/NvTweakVar.h"

template<class T>
T NvMax(T a, T b)                            { return a<b ? b : a;    }

/**
\brief The return value is the lesser of the two specified values. 
*/
template<class T>
T NvMin(T a, T b)                            { return a<b ? a : b;    }

template <>
void NvTweakVar<uint32_t>::increment()
{
    if (mValClamped) // check to see if we'll exceed max.
        if (mValRef == mValMax || mValRef+mValStep > mValMax)
        {// if already at max, or would exceed max, loop or clamp.
            if (mValLoop)
                mValRef = mValMin;
            else
                mValRef = mValMax;
            return;
        }

    mValRef += mValStep;
}

template <>
void NvTweakVar<uint32_t>::decrement()
{
    if (mValClamped) // check to see if we'll drop below min.
        if (mValRef == mValMin || mValRef-mValStep < mValMin)
        {// if already at min, or would drop under min, loop or clamp.
            if (mValLoop)
                mValRef = mValMax;
            else
                mValRef = mValMin;
            return;
        }

    mValRef -= mValStep;
}


template <>
void NvTweakVar<float>::increment()
{
    if (mValClamped) // check to see if we'll exceed max.
        if (mValRef == mValMax || mValRef+mValStep > mValMax)
        {// if already at max, or would exceed max, loop or clamp.
            if (mValLoop)
                mValRef = mValMin;
            else
                mValRef = mValMax;
            return;
        }

    mValRef += mValStep;
}

template <>
void NvTweakVar<float>::decrement()
{
    if (mValClamped) // check to see if we'll drop below min.
        if (mValRef == mValMin || mValRef-mValStep < mValMin)
        {// if already at min, or would drop under min, loop or clamp.
            if (mValLoop)
                mValRef = mValMax;
            else
                mValRef = mValMin;
            return;
        }

    mValRef -= mValStep;
}

// partial specialization for bool, because +/- make no sense
template <>
void NvTweakVar<bool>::increment()
{
    mValRef = !mValRef;
}

template <>
void NvTweakVar<bool>::decrement()
{
    mValRef = !mValRef;
}


template <>
bool NvTweakVar<bool>::equals(bool val)
{
    return (mValRef == val);
}
template <>
bool NvTweakVar<bool>::equals(float val)
{
    return false;
}
template <>
bool NvTweakVar<bool>::equals(uint32_t val)
{
    return false;
}


template <>
bool NvTweakVar<float>::equals(bool val)
{
    return false;
}
template <>
bool NvTweakVar<float>::equals(float val)
{
    return (mValRef == val);
}
template <>
bool NvTweakVar<float>::equals(uint32_t val)
{
    return false;
}


template <>
bool NvTweakVar<uint32_t>::equals(bool val)
{
    return false;
}
template <>
bool NvTweakVar<uint32_t>::equals(float val)
{
    return false;
}
template <>
bool NvTweakVar<uint32_t>::equals(uint32_t val)
{
    return (mValRef == val);
}
