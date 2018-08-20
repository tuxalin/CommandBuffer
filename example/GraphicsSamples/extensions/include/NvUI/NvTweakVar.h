//----------------------------------------------------------------------------------
// File:        NvUI/NvTweakVar.h
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

#ifndef NV_TWEAKVAR_H
#define NV_TWEAKVAR_H

#include <NvSimpleTypes.h>

/** @file NvTweakVar.h
    Implements a system for indirect referencing of physical C variables from
    an abstracted object, by keeping a reference to a variable along with
    an 'action code' that can be used to connect up with NvTweakBar widgets,
    NvSampleApp's input mapping, or other custom code.  This allows the developer
    to 'tweak' the value of a given variable across these different helper systems
    without any of them knowing directly of the bound variable.
*/

/** This is an abstract base class for indirectly referencing app variables.
*/
struct NvTweakVarBase {
// while annoying, this is the best way to protect access to setActionCode.
friend class NvTweakBar;
template<class T> friend class NvTweakVarUI;

protected:
    const char* mName; /**< A human-readable name/title of the variable. */
    const char* mDesc; /**< An informative 'help' string for the variable. */
    uint32_t mActionCode; /**< A unique value for signalling changes across systems. */

    /** Base constructor.
        Note that the base constructor defaults mActionCode to 0, expecting
        subclass constructors to set a meaningful, unique value afterward.
        @param name A title for this variable.
        @param description An OPTIONAL help string.
    */
    NvTweakVarBase(const char* name, const char *description=NULL)
    : mName(name)
    , mDesc(description)
    , mActionCode(0)
    {
        /*no-op*/
    }

public:
    /** @name Virtual methods for quick value tweaks to a variable.
        @{
    */
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void reset() = 0;
    /** @} */

    /** @name Virtual methods for testing equality against values without knowing our type.
        @{
    */
    virtual bool equals(bool val) = 0;
    virtual bool equals(float val) = 0;
    virtual bool equals(uint32_t val) = 0;
    /** @} */

    /** Accessor to retrieve pointer to the name string. */
    const char *getName() { return mName; }
    /** Accessor to retrieve pointer to the description string. */
    const char *getDesc() { return mDesc; }

    /** Accessor to retrieve the action code value. */
    uint32_t getActionCode() { return mActionCode; }

protected:
    // Devs shouldn't touch action code.  This is for other systems like TweakBar.
    /** Set the action code value for this variable. */
    void setActionCode(uint32_t code) { mActionCode = code; }
};


/** Templated class for holding a reference to a variable of a particular datatype. */
template<class T>
struct NvTweakVar : public NvTweakVarBase {
private:
    // these don't make sense, let's no-op them.
    NvTweakVar() {}
    NvTweakVar& operator=( const NvTweakVar& v) {}
    NvTweakVar( const NvTweakVar& v){}

protected:   
    T& mValRef; /**< Reference to variable being tracked/tweaked. */
    T mValInitial; /**< Initial value, useful for 'reset' to starting point. */

    T mValSelf; /**< A member of our datatype, for self-referencing NvTweakVar to point into itself. */

    T mValMin; /**< Minimum value for a variable with clamped range. */
    T mValMax; /**< Maximum value for a variable with clamped range. */
    T mValStep; /**< Value step for the variable (increment/decrement). */

    bool mValClamped; /**< Whether value adjustments to this variable are clamped/bounded. */
    bool mValLoop; /**< When value is clamped, does value loop around when increment/decrement hits 'ends'. */

public:
    /** Accessor for getting at the current value when we hold onto a typed subclass. */
	T getValue() { return mValRef; }

	/** Set the max value of the variable dynamically */
	/** \param[in] maxVal the new maximum of the var */
	void SetMaxValue(T maxVal) { mValMax = maxVal; }

    /** Value-reference operators to access the internal variable we manage. */
    operator T&() { return mValRef; }
    /** Const value-reference operators to access the internal variable we manage. */
    operator const T&() const { return mValRef; }
    /** Assignment operator to set (once) the internal variable we will point to. */
    const NvTweakVar& operator=( T val) { mValRef = val; return *this; }

    /** Clamped constructor, typically used for scalar variables. */
    NvTweakVar( T& refVal, const char* name, T minVal, T maxVal, T step, char *description=NULL)
    : NvTweakVarBase(name, description)
    , mValRef(refVal)
    , mValInitial(refVal)
    , mValSelf(0)
    , mValMin(minVal)
    , mValMax(maxVal)
    , mValStep((step==0)?1:step)
    , mValClamped(true)
    , mValLoop(false)
    {
        /* no-op */
    }

    /** Clamped constructor, used for self-referential scalars. */
    NvTweakVar(const char* name, T minVal, T maxVal, T step, char *description=NULL)
    : NvTweakVarBase(name, description)
    , mValRef(mValSelf)
    , mValInitial(0)
    , mValSelf(0)
    , mValMin(minVal)
    , mValMax(maxVal)
    , mValStep((step==0)?1:step)
    , mValClamped(true)
    , mValLoop(false)
    {
        /* no-op */
    }

    /** Specialized unclamped constructor, generally used for bool variable. */
    NvTweakVar( T& refVal, const char* name, const char *description=NULL)
    : NvTweakVarBase(name, description)
    , mValRef(refVal)
    , mValInitial(refVal)
    , mValSelf(0)
    , mValMin((T)0)
    , mValMax((T)0)
    , mValStep((T)1)
    , mValClamped(false)
    , mValLoop(false)
    {
        /* no-op */
    }

    /** Specialized unclamped constructor, used for self-referential bool. */
    NvTweakVar(const char* name, const char *description=NULL)
    : NvTweakVarBase(name, description)
    , mValRef(mValSelf)
    , mValInitial(0)
    , mValSelf(0)
    , mValMin((T)0)
    , mValMax((T)0)
    , mValStep((T)1)
    , mValClamped(false)
    , mValLoop(false)
    {
        /* no-op */
    }
    
    /** Set whether or not to loop clamped value when increment/decrement reach ends of range. */
    void setValLoop(bool loop) { mValLoop = loop; }

    /** Specific implementation of increment for the templated datatype. */
    virtual void increment();
    /** Specific implementation of decrement for the templated datatype. */
    virtual void decrement();

    /** Reset the managed variable to its initial value. */
    virtual void reset() {
        mValRef = mValInitial;
    }

    /** Specific implementation of equals that each templated type must override appropriately. */
    virtual bool equals(bool val);
    /** Specific implementation of equals that each templated type must override appropriately. */
    virtual bool equals(float val);
    /** Specific implementation of equals that each templated type must override appropriately. */
    virtual bool equals(uint32_t val);
};


/** Template for objects that pair human-readable strings with their application-value counterparts.
    When placed in an array initializer, NvTweakEnum allows for quick abstract definition
    of name:value pairs like C enums, and the array can then be passed into other
    functions to define a set of assignable named values for a specific variable.
*/
template<class T>
struct NvTweakEnum
{
    /** This is a helper macro to calculate the number of array
        entries for enum arrays, so one can declare such arrays unsized.
    */
    #define TWEAKENUM_ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

    const char *m_name; /**< The human-readable name of this enumerator. */
    T m_value; /**< The specific value denoted by this enumerator. */

    operator T&() { return m_value;} /**< We override the type-reference operator to access the value. */
    operator const T&() const { return m_value;} /**< We override the const type-reference operator to access the value. */

    operator char *() { return m_name;} /**< We override char* operator to access the name field. */
    operator const char *() const { return m_name;} /**< We override const char* operator to access the name field. */
};


/** Template subclass for an NvTweakVar whose value is set based on entries of an NvTweakEnum of the same datatype.
*/
template<class T>
struct NvTweakEnumVar : public NvTweakVar<T> {
protected:
    NvTweakEnum<T> *m_enumVals;
    uint32_t m_enumValCount;
    uint32_t m_enumIndex;

public:
    /** Basic constructor for making an NvTweakEnumVar from a list of enum values */
    NvTweakEnumVar( const NvTweakEnum<T> enumVals[], uint32_t enumValCount, T& refVal, const char* name, T minVal, T maxVal, T step, char *description=NULL)
    : NvTweakVar<T>( refVal, name, minVal, maxVal, step, description )
    , m_enumValCount(enumValCount)
    , m_enumIndex(0) // until set later...
    {
        m_enumVals = new NvTweakEnum<T>[enumValCount];
        for (uint32_t i=0; i<enumValCount; i++) {
            m_enumVals[i] = enumVals[i]; // copy?
            if (enumVals[i] == refVal)
                m_enumIndex = i;
        }
    }

    /** Default destructor */
    virtual ~NvTweakEnumVar()
    {
        delete[] m_enumVals;
    }

    /** Array operator for accessing the values of the contained enum */
    T& operator[](const uint32_t index)
    {
        if (index>=m_enumValCount)
            return m_enumVals[0].m_value;  // OOB !!!!TBD TODO log error to cout/stderr?
        return m_enumVals[index].m_value;
    }

    /** Implementation of increment that incs internal index into enum value array, and sets based on enum value. */
    virtual void increment() {
        if (m_enumIndex==m_enumValCount-1)
            m_enumIndex = 0;
        else
            m_enumIndex++;
        this->mValRef = m_enumVals[m_enumIndex];
    }

    /** Implementation of decrement that decs internal index into enum value array, and sets based on enum value. */
    virtual void decrement() {
        if (m_enumIndex==0)
            m_enumIndex = m_enumValCount-1;
        else
            m_enumIndex--;
        this->mValRef = m_enumVals[m_enumIndex];
    }
};


/** Enum of 'tweak' commands we can apply to a given NvTweakVar. */
struct NvTweakCmd {
    enum Enum {
        NONE = 0, /**< No command specified */
        RESET, /**< Reset the variable to initial state */
        INCREMENT, /**< Increment the variable */
        DECREMENT, /**< Decrement the variable */
    };
};

/** Pair of NvTweakCmd and NvTweakVarBase that we can look up as a bound result for some user input. */
struct NvTweakBind {
    NvTweakCmd::Enum mCmd;
    NvTweakVarBase *mVar;

    NvTweakBind()
        : mCmd(NvTweakCmd::NONE), mVar(NULL) { /* no-op */ };
    NvTweakBind(NvTweakCmd::Enum cmd, NvTweakVarBase *var)
        : mCmd(cmd), mVar(var) { /* no-op */ };
};

#endif //NV_TWEAKVAR_H
