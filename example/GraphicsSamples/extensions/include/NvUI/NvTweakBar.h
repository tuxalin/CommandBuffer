//----------------------------------------------------------------------------------
// File:        NvUI/NvTweakBar.h
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

#ifndef NV_TWEAKBAR_H
#define NV_TWEAKBAR_H

#include "NvUI/NvUI.h"
#include "NvUI/NvTweakVar.h"
#include <vector>

/** @file NvTweakBar.h
    Implements a UI system for sample applications to display visual widgets
    for interacting with the C variables that control the application, using
    NvTweakVarBase to abstract access to those variables.
*/

/** An object that will help link an NvTweakVar with a proxied NvUIElement.
    This class acts as the basis for NvTweakVarUI, connecting up a tweakable
    app variable up with a particular NvUI widget class appropriate for
    interacting with that variable -- such as buttons and sliders.
*/
class NvTweakVarUIProxyBase : public NvUIProxy
{
protected:
    /** @cond */
    INHERIT_FROM(NvUIProxy);
    /** @endcond */
    bool m_readonly;

public:
    /** Default constructor, takes the UI element we proxy to. */
    NvTweakVarUIProxyBase(NvUIElement *el)
        : INHERITED(el)
    {
        SetReadOnly(false);
    }

    void SetReadOnly(bool ro)
    {
        m_readonly = ro;
        m_canFocus = !ro;
    }

    // !!!TODO may want to override drawstate to only draw inactive state, etc....

    /** We override HandleEvent to short-circuit any tweaks of read-only variables. */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
    {
        if (m_readonly)
            return nvuiEventNotHandled;
        return INHERITED::HandleEvent(ev, timeUST, hasInteract);
    }

    /** We override HandleEvent to short-circuit any tweaks of read-only variables. */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react)
    {
        //if (m_readonly)
        //    return nvuiEventNotHandled;
        return INHERITED::HandleReaction(react);
    }
};


/** A templated object that links NvTweakVar of some datatype with an appropriate UI widget.
    This class connects a tweakable app variable, a specific NvTweakVar template instance,
    with a UI widget appropriate for changing the value of that variable, linked by an
    action code shared between the systems.
*/
template<class T>
class NvTweakVarUI : public NvTweakVarUIProxyBase
{
protected:
    /** @cond */
    INHERIT_FROM(NvTweakVarUIProxyBase);

    //copying doesn't make sense
    NvTweakVarUI() {}
    NvTweakVarUI& operator=( const NvTweakVarUI& v) {}
    NvTweakVarUI( const NvTweakVarUI& v){}
    /** @endcond */

    NvTweakVar<T> &m_tvar; /**< The variable we hold a reference to in order to adjust its value based on the input/reaction from the user interacting with our proxied UI widget. */

public:
    /** Base constructor for establishing UI for editing an NvTweakVar variable.
        This instantiates the base proxy link between this stand-in object and
        the real NvUIElement widget, and sets the action code if needed.
        */
    NvTweakVarUI(NvTweakVar<T> &tvar, NvUIElement *el, uint32_t actionCode=0)
    : INHERITED(el)
    , m_tvar(tvar)
    {
        m_tvar.setActionCode(actionCode);
    };

    /** We override HandleReaction so that when there is an NvUIReaction passing through
        the system containing a value change for us or from our proxied UI, we can intercept
        and set the value of our NvTweakVar appropriately. */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);
};

/** A templated subclass of NvTweakVarUI specifically for handling one entry of an NvTweakEnum
    list/array passed into the tweakbar -- i.e., one button in a radio button group, or one
    menu item in a popup menu.  Each NvTweakVarUI element is responsible for setting its stored
    enum value on the linked NvTweakVar when the UI element is selected.
*/
template<class T>
class NvTweakEnumUI : public NvTweakVarUI<T>
{
protected:
    /** @cond */
    INHERIT_FROM(NvTweakVarUI<T>);

    //copying doesn't make sense
    NvTweakEnumUI() {}
    NvTweakEnumUI& operator=( const NvTweakEnumUI& v) {}
    NvTweakEnumUI( const NvTweakEnumUI& v){}
    /** @endcond */

    NvTweakEnumVar<T>& m_tevar;     /*< A reference to the enum var that we represent one entry from. */
    uint32_t m_teindex;             /*< The array index of the enum entry that our UI item represents. */
    T m_enumval;                    /*< A cached value passed in via an NvTweakEnum entry. 
                                        Will be used to set our linked NvTweakVar to a specific value
                                        during an appropriate HandleReaction. */

public:
    /** Normal constructor.  Takes the referenced NvTweakEnumVar, the enumerant index we represent,
        the individual UI element that acts as our proxy for user interaction in a radio group
        or popup menu, and an optional override of the action code for the variable and UI.
    */
    NvTweakEnumUI(NvTweakEnumVar<T> &tevar, uint32_t index, NvUIElement *el, uint32_t actionCode=0)
    : INHERITED(tevar, el, actionCode)
    , m_tevar(tevar)
    , m_teindex(index)
    , m_enumval(tevar[index]) // cache the value at the given index for easier access later.
    {
    };

    /** We override HandleEvent so that if there is a reaction by the proxied UI widget,
        we can replace the value inside the NvUIReaction with our cached value, and
        allow the rest of the reaction process to occur as normal.
    */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract);

    /** We override HandleReaction so that when there is an NvUIReaction passing through
        the system containing a real value, we can set our state to be 0 or 1 (active or
        pressed) based on whether our cached value matches the value of the NvTweakVar
        we're bound to.  This is so radio buttons and menu items update their visual
        state to match outside changes to the NvTweakVar's value (such as from key input).
    */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);

    /** We override HandleFocusEvent so that if there is a reaction by the proxied UI widget,
        we can replace the value inside the NvUIReaction with our cached value, and
        allow the rest of the reaction process to occur as normal.
    */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);
};

/** This is a predefined base action code value for any codes
    defined/allocated inside of the NvTweakBar system itself.
*/
#define TWEAKBAR_ACTIONCODE_BASE    0x43210000

/** Class representing a visual 'bar' of widgets (a 'tweakbar').
    This class shows a contained group of user-interface widgets
    specific to the given application, for directly interacting with
    core variables controlling the application's functionality.
*/
class NvTweakBar : public NvUIContainer
{
private:
    INHERIT_FROM(NvUIContainer);

    float m_lastElX;
    float m_lastElY;
    float m_xInset;
    float m_widthPadFactor;
    float m_padHeight;
    uint32_t m_lastActionCode;
    uint32_t m_defaultRows;
    bool m_textShadows;
    bool m_compactLayout;

    float m_subgroupLastY;
    float m_subgroupMaxY;
    NvUIContainer *m_subgroupContainer;
    NvTweakVarBase *m_subgroupSwitchVar;

    NvPackedColor m_labelColor;
    NvPackedColor m_valueColor;

    std::vector<NvTweakVarBase *> m_tweakVars;
    
    NvUIGraphic *MakeFocusFrame();
    NvUIPopup *MakeStdPopup(const char* name, NvTweakEnumVar<uint32_t> &refvar, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode=0);
    NvUIButton *MakeStdButton(const char* name, bool val, NvUIButtonType::Enum type=NvUIButtonType::CHECK, uint32_t code=0, uint32_t subcode=0);
    NvUISlider *MakeStdSlider(const char* name, float val, float min, float max, float step=0, bool integral=false, uint32_t code=0);

    void AddElement(NvUIElement *te, bool autoSpace=true);

public:
    /** Static method to create an NvTweakBar attached to a window.
        This method helps you to creates an instance of NvTweakBar
        that is properly connected up with the provided NvUIWindow.
        @param window The NvUIWindow to attach to.
        */
    static NvTweakBar *CreateTweakBar(NvUIWindow *window);

    /** Normal constructor. 
        Note the constructor should not need to be manually called unless
        you are creating a very customized user interface which is not
        relying on NvSampleApp, nor CreateTweakBar to link up with an
        NvUIWindow and auto-size the tweakbar to fit reasonably.
        @param width The width of the bar in pixels
        @param height The height of the bar in pixels
    */
    NvTweakBar(float width, float height);

    /** Normal virtual destructor. 
        Note the destructor should not need to be manually called unless
        you are creating a custom user interface not relying on NvSampleApp.
    */
    virtual ~NvTweakBar();

    /** Adjusts the height of rows and padding by setting the number of rows
        desired in the tweakbar 
        @param numRows Number of rows to fit on the TweakBar by adjusting the
                       height of rows and associated padding relative to the
                       current size of the tweakbar.
        @note This will only adjust sizes for UI elements added after the call.
              any previously added UI Elements will retain their previous 
              positions and dimensions.
    */
    void SetNumRows(uint32_t numRows) 
    { 
        m_defaultRows = numRows; 
        m_padHeight = GetDefaultLineHeight()*0.35f;
    }

    /** Accessor to retrieve the default height of one row in the Tweakbar. */
       inline float GetDefaultLineHeight() { return GetHeight()/m_defaultRows; }
    /** Accessor to retrieve the default width of a row in the Tweakbar. */
    inline float GetDefaultLineWidth() { return (GetWidth() - (2 * m_widthPadFactor * GetWidth()) - m_xInset); }
    /** Accessor to retrieve the starting vertical offset of elements in the Tweakbar.
        Primary use of this method is so external controls can be aligned with the
        first line of the Tweakbar, for visual continuity. */
    inline float GetStartOffY() { return GetDefaultLineHeight()*0.5f; }

    /** Method to handle results of user interaction with widgets.
        @param react The reaction structure to process/handle.
    */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);

    /** Method to handle the effect of window/screen resize.
        @param w The new view width.
        @param h The new view height.
    */
    virtual void HandleReshape(float w, float h);

    /** Method to handle drawing us on screen.
        @param drawState The NvUIDrawState that encapsulates needed state.
    */
    virtual void Draw(const NvUIDrawState &drawState);

    /** Method to notify Tweakbar widgets of possible changes to tracked variables.
        This method is called to let the Tweakbar know that some outside system
        has (potentially) changed the underlying values of variables that Tweakbar
        widgets are also watching/controlling via NvTweakVars, and thus all widgets
        should go and refresh themselves.
    */
    void syncValues();

    /** Method to notify Tweakbar widgets of changes to a specific NvTweakVarBase.
        This method is called to let the Tweakbar know that some outside system
        has (potentially) changed the value of a specific variable that Tweakbar
        widgets are watching, and thus matching widgets should refresh themselves.
    */
    void syncValue(NvTweakVarBase* var);

    /** Method to reset all TweakVar values and associated UI to what the
        TweakVars have as the starting value/state.
    */
    void resetValues();

    /** Method to request the Tweakbar try to use more compact versions
        of widgets, and spacing between widgets, when possible.
        @param compact Pass in true to enable compact layout logic.
    */
    void setCompactLayout(bool compact) { m_compactLayout = compact; }

    /** Method to request automatic dropshadows for all Tweakbar text.
        @param b Pass true to enable automatic dropshadows.  Default is false.
    */
    void textShadows(bool b) { m_textShadows = b; }

    /** @name Methods for grouping together a block of related variables.
        @{
        */

    /** Method to request the start of a set of widgets be grouped together.
    */
    void subgroupStart();
    /** Method to note the end of a grouped set of widgets.
    */
    void subgroupEnd();

    /** Method to request the start of a group of widgets, whose visibility
        is controlled by the value of a given NvTweakVarBase being tracked.
        This method starts a very special grouping of widgets, working like
        a 'case' statement, where each case/subgroup is only visible when
        the tracked variable matches the value of the given subgroup's case.

        An example use might be something like the following:
@code
// A temporary variable to hold the current NvTweakVarBase we're referencing.
NvTweakVarBase *var;

// Add a variable to be managed by the tweakbar, and get the returned pointer
var = mTweakBar->addValue("Auto LOD", mLod);

// Start a 'switch' based on the returned variable
mTweakBar->subgroupSwitchStart(var);

    // Handle the switch case for 'true'
    mTweakBar->subgroupSwitchCase(true);

        // Add a variable to the current switch case.
        var = mTweakBar->addValue("Triangle size", mParams.triSize, 1.0f, 50.0f);

    // Handle the switch case for 'false'
    mTweakBar->subgroupSwitchCase(false);

        // Add a variable to the current switch case.
        var = mTweakBar->addValue("Inner tessellation factor", mParams.innerTessFactor, 1.0f, 64.0f);
        // Add another variable to the current case.
        var = mTweakBar->addValue("Outer tessellation factor", mParams.outerTessFactor, 1.0f, 64.0f);

// Close/end the switch group.
mTweakBar->subgroupSwitchEnd();
@endcode
        @param var The NvTweakVarBase object to track and switch visibility upon.
    */
    void subgroupSwitchStart(NvTweakVarBase *var);
    /** Method for setting a case for a boolean tracked variable, with the
        value for the specific case.
        @param val The boolean case value for this subgroup/block.
    */
    void subgroupSwitchCase(bool val);
    /** Method for setting a case for a floating-point tracked variable, with the
        value for the specific case to match upon.
        @param val The floating-point case value for this subgroup/block.
    */
    void subgroupSwitchCase(float val);
    /** Method for setting a case for an integral tracked variable, with the
        value for the specific case to match upon.
        @param val The integral case value for this subgroup/block.
    */
    void subgroupSwitchCase(uint32_t val);
    /** Method to note the end of the current subgroup 'switch' block.
    */
    void subgroupSwitchEnd();

    /**  @}  */

private:
    // hide these two for now, as really they are internal-use.
    /** Method to set up a container for switching.
    */
    void subgroupSwitchCase(NvUIContainer *c);
    /** Method to close the current subgroup/case.
    */
    void subgroupClose();

    /** Method to add a TweakVar to our tracking list.
    */
    void trackTweakVar(NvTweakVarBase *var);

public:

    /** @name Methods for adding variables to the tweakbar.
        @{
        */

    /** Requests vertical padding be added prior to the next widget or subgroup.
        @param multiplier [optional] A factor to apply to the default padding amount. Defaults to 1.0. */
    void addPadding(float multiplier=1);

    // strings
    /** Add a static text label to the tweakbar. */
    void addLabel(const char *title, bool bold=false);
    // !!!!TBD TODO addValue that takes a string ref so
    // we can do dynamic labels controlled by the app.

    /** Add a textual readout for a float variable. */
    NvTweakVarBase* addValueReadout(const char* name, float &var, float max=0, uint32_t code=0);

    /** Add a textual readout for a uint variable. */
    NvTweakVarBase* addValueReadout(const char* name, uint32_t &var, uint32_t code=0);

    /** Add a textual readout for a pre-created floating-point NvTweakVar variable. */
    NvTweakVarBase* addValueReadout(NvTweakVar<float>* tvar, float max);

    /** Add a textual readout for a pre-created integral NvTweakVar variable. */
    NvTweakVarBase* addValueReadout(NvTweakVar<uint32_t>* tvar);

    // buttons/bools
    /** Add a bool variable to be controlled by the user.
        This method adds the given bool variable to be tracked by the
        system and controlled by the user.  The variable is visualized
        as a checkbox interface (so users can toggle between true and
        false).
    */
	NvTweakVarBase* addValue(const char *name, bool &var, uint32_t actionCode = 0, NvUIButton** buttonOut = NULL);
    /** Add an action-only push-button to the Tweakbar. 
        This method adds a push-button to the Tweakbar that is not tied to
        an application variable.  Instead, it simply generates the given
        action code for a normal NvUIReaction handling by the application --
        and as such, the @p actionCode parameter is required unlike other
        'add' methods, since that is how the app knows the button was pressed.
    */
    NvTweakVarBase* addButton(const char* name, uint32_t actionCode); // action code NOT optional.

    // scalars/sliders
    /** Add a floating-point variable to the Tweakbar as a slider.
        This method adds a slider to the Tweakbar, tracking the supplied
        floating-point variable.
    */
    NvTweakVarBase* addValue(const char *name, float &var, float min, float max, float step=0, uint32_t actionCode=0);
    /** Add an integral variable to the Tweakbar as a slider.
        This method adds a slider to the Tweakbar, tracking the supplied
        floating-point variable.
    */
	NvTweakVarBase* addValue(const char *name, uint32_t &var, uint32_t min, uint32_t max, uint32_t step = 0, uint32_t actionCode = 0, 
		NvUISlider** pSliderOut = NULL, NvTweakVar<uint32_t>** pVarOut = NULL);

    // enums/radios/menus
    /** Add an integral variable to the Tweakbar as an radio group or dropdown menu
        with an enumerated set of values.
        This method tracks and modifies the supplied variable using the passed in
        array of NvTweakEnum string-value pairs as individual items.  It will create
        a radio-button group for two or three enum values, and a dropdown menu for
        four or more values.
    */
    NvTweakVarBase* addEnum(const char *name, uint32_t &var, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode=0);

    /** Add an integral variable to the Tweakbar as a dropdown menu 
        with an enumerated set of values.
        This method tracks and modifies the supplied variable using the passed in
        array of NvTweakEnum string-value pairs as the menu items in a dropdown menu.
    */
    NvTweakVarBase* addMenu(const char *name, uint32_t &var, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode=0);
    /**  @}  */
};

#endif //NV_TWEAKBAR_H
