//----------------------------------------------------------------------------------
// File:        NvUI/NvUI.h
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

#ifndef _NV_UI_H
#define _NV_UI_H

#include <NvSimpleTypes.h>

#include <NvUI/NvGestureEvent.h>
#include "NvUI/NvPackedColor.h"
#include "NV/NvVector.h"
#include "NV/NvTime.h"

#include <string.h>
#include <string>
#include <map>

// fwd decl of BFText class so we don't need to include header at all.
class NvBFText;

/** @file NvUI.h
    @brief A cross-platform, GL/GLES-based, simple user interface widget framework.
    
    An basic user interface framework built up from a starting primitive widget,
    the NvUIElement base class.  In order to keep to the basics, it uses no
    multiple-inheritance, no STL or templates, no RTT, no operator overloads.
        
    In terms of accessible functionality, the UI framework exposes:
    <ul>
        <li> positioning and sizing of elements </li>
        <li> overall visibility, as well as more precision alpha fading </li>
        <li> drawing a given element (or contained hierarchy) </li>
        <li> giving an element a chance to handle user input events </li>
        <li> giving an element a chance to react to other elements' event handling </li>
    </ul>

    Some of the specific UI elements covered so far include:
    <ul>
        <li> graphics (icons, frames, backgrounds -- including texture loading) </li>
        <li> frames/boxes (resizable border+background) </li>
        <li> text (single or multi-line, aligned, wrapped, font/size/style) </li>
        <li> buttons (push, radio, and checkbox style interactions) </li>
        <li> progress indicators & sliders </li>
        <li> generic containers (hold other elements and pass along drawing/handling) </li>
    </ul>
   
   Combined with custom code, the system allows for great flexibility.  For
   example, you could easily create an 'animated icon' by moving a graphic's
   origin around each frame based on some custom animation code, or even
   swap the texture a graphic is rendering each frame to produce 'flipbook'
   style animations.
   
   You can also use the classes as 'helper objects', instantiating even just a
   single graphic object to help quickly render textures/bitmaps to screen without
   the need to write a line of GL code, or custom shaders.
 */

/** This macro offers "INHERITED::" access to superclass in each NvUI class. */
#define INHERIT_FROM(c) typedef c INHERITED;

 
/** Event handling response flags.

    All UI objects have the ability/opportunity to handle input events, and must
    return an NvUIResponse flag for what occurred as a result.
    
    In general, the response notes that an object has handled the event, wants
    to keep 'interacting' as a result of it, and/or has a NvUIReaction (a side effect)
    that is triggered based on it.
 */
typedef enum {
    nvuiEventNotHandled     = 0x000, /**< Flag/mask that we didn't handle the event. */
    nvuiEventHandled        = 0x001, /**< Flag/mask that we handled the event. */

    nvuiEventWantsHover    = 0x010, /**< Flag/mask that we want hover highlight as a result of the event. */
    nvuiEventHandledHover   = nvuiEventHandled | nvuiEventWantsHover,  /**< We handled the event AND want hover highlight. */

    nvuiEventWantsInteract     = 0x020, /**< Flag/mask that we want to keep interacting as a result of the event. */
    nvuiEventHandledInteract   = nvuiEventHandled | nvuiEventWantsInteract,  /**< We handled the event AND want to keep interacting. */
    
    nvuiEventHadReaction    = 0x100, /**< Flag/mask that we have a NvUIReaction as a side effect. */
    nvuiEventHandledReaction = nvuiEventHandled | nvuiEventHadReaction,  /**< We handled the event AND 'posted' an NvUIReaction. */
    nvuiEventHandledInteractReaction = nvuiEventHandled | nvuiEventWantsInteract | nvuiEventHadReaction, /**< We handled the event AND want to keep interacting AND had an NvUIReaction. */
    nvuiEventNoReaction    = ~nvuiEventHadReaction, /**< Mask to clear a prior Reaction flag from the response. */
} NvUIEventResponse;


/** A big bold constant to make pure virtual methods more visible in code. */
#define NV_PURE_VIRTUAL     0

/** This enum defines bit flags for extra/other info about a particular NvUIReaction. */
struct NvReactFlag {
    enum Enum { 
        NONE =           0x00, /**< No additional flag data. */
        FORCE_UPDATE =   0x01, /**< Flag to notify any UI elements linked to an outside data
                        source (NvTweakVar or otherwise) that they should update themselves.*/
        CLEAR_STATE =    0x02, /**< Flag that UI elements that match this reaction should
                        clear their drawing state (to 'inactive'). */
    };
};

/** This enum defines values for moving focus around with keyboard/DPAD when allowed and supported. */
struct NvFocusEvent {
    enum Enum { 
        FOCUS_CLEAR     =   0x0000, /**< Clear current focus chain. */
        
        FLAG_MOVE       =   0x10, /**< Movement events. */
        FLAG_ACT        =   0x20, /**< Action events. */

        MOVE_FIRST      =   0x01+FLAG_MOVE, /**< Move focus to first element on screen. */
        MOVE_UP         =   0x02+FLAG_MOVE, /**< Move focus up. */
        MOVE_DOWN       =   0x03+FLAG_MOVE, /**< Move focus up. */
        MOVE_LEFT       =   0x04+FLAG_MOVE, /**< Move focus up. */
        MOVE_RIGHT      =   0x05+FLAG_MOVE, /**< Move focus up. */
        
        ACT_PRESS       =   0x01+FLAG_ACT, /**< Press or toggle current value/button where appropriate. */
        ACT_INC         =   0x02+FLAG_ACT, /**< Increase current value/button where appropriate. */
        ACT_DEC         =   0x03+FLAG_ACT, /**< Decrease current value/button where appropriate. */
    };
};

/** A structure holding some 'reaction' to a given user input event.

    When a UI element handles an input event, it might have a side-effect it
    wants to occur (an NvUIReaction) in the UI hierarchy/system.  This simple
    structure allows for noting who kicked off the reaction, the 'type' of
    Reaction (that is usually a unique identifier code), and any secondary
    state that might be useful to the future-handling element.
*/
typedef struct
{
    uint32_t uid;   /**< This is the NvUIElement.uiuid member of the element 'raising' the reaction. */
    uint32_t code;  /**< This is the app/element specific 'reaction code'. */
    uint32_t state; /**< This is other reaction state, generally used for visual widgets like buttons to track or pass current draw state. */

    NvGestureKind::Enum causeKind; /**< Copying the 'cause' of the reaction.  If kind is none, it's not input/gesture related. */
    uint8_t causeIndex; /**< For input-related reactions, this could be mouse button, gamepad button, or key identifier. */
    NvReactFlag::Enum flags; /**< Any particular extra flags about this reaction. */

    float fval;   /**< This is a floating point value from data-related widgets. */
    uint32_t ival;   /**< This is an integer value from data-related widgets. */

} NvUIReaction;


//=============================================================================
//=============================================================================
// !!!!TBD should have m_ prefixed this stuff. 
/** A class that manages all draw-related state.

    When calling a UI element to Draw itself, an NvUIDrawState object is passed
    in to give 'annotation' to the drawing process.  This allows us to also add
    further information to the DrawState, without changing the signature of the
    Draw method.
*/
class NvUIDrawState
{
public:
    NvUST   time; /**< A UST timestamp for the "time" of a given Draw call.  Currently unused. */
    int32_t   width; /**< The render-buffer view width. */
    int32_t   height; /**< The render-buffer view height. */
    int32_t   designWidth; /**< [optional] The current UI's designed width.  Important for auto-scaling and orientation of UI widgets if there is an explicit 'design space' vs current viewport space. */
    int32_t   designHeight; /**< [optional] The current UI's designed height.  Important for auto-scaling and orientation of UI widgets if there is an explicit 'design space' vs current viewport space. */
    float alpha; /**< [optional] An alpha-fade override for this draw call, to help fading elements over time.  Defaults to 1.0 (opaque). */
    float rotation; /**< [optional] A rotation (in degrees) to use for 'aligning' content between the design and the render-buffer.  Defaults to 0.0 (unrotated). */

    /** Default constructor.  Design sizes are optional params, all other members must be set after. */
    NvUIDrawState(NvUST inTime, int32_t inWidth, int32_t inHeight, int32_t inDesignWidth=0, int32_t inDesignHeight=0)
        : time(inTime), width(inWidth), height(inHeight),
            designWidth(inDesignWidth), designHeight(inDesignHeight),
            alpha(1.0f), rotation(0.0f)
    {
    }    
};
        

//=============================================================================
//=============================================================================
/** A class that abstracts on onscreen rectangular region.

    All UI elements use the NvUIRect to a greater or lesser degree.  It is the
    underpinning of onscreen positioning and scaling (and thus animation too),
    and heavily used in input "hit testing" routines as well.
*/
class NvUIRect
{
public:    
    float left; /**< Left/x origin in UI-space */
    float top; /**< Top/y origin in UI-space */
    float width; /**< Width in pixels */
    float height; /**< Height in pixels */
    float zdepth; /**< FUTURE: z-depth of rect */
    // NvUIGravity gravity; // todo.

    /** Default constructor, zeroes all elements. */
    NvUIRect()
        : left(0), top(0), width(0), height(0), zdepth(0)
        { /* empty */ };
        
    /** Normal use constructor.
        @param l left edge position
        @param t top edge position
        @param w width of rect
        @param h height of rect
        @param z [optional] z-depth of rect
    */
    NvUIRect(float l, float t, float w, float h, float z=0.0f)
        : left(l), top(t), width(w), height(h), zdepth(z)
        { /* empty */ };

    /** Default destructor */
    virtual ~NvUIRect();

    /** General value setter.
        @param l left edge position
        @param t top edge position
        @param w width of rect
        @param h height of rect
        @param z [optional] z-depth of rect
    */
    virtual void Set(float l, float t, float w, float h, float z=0.0f)
        {
            left = l;
            top = t;
            width = w;
            height = h;
            zdepth = z;
        };

    /** Helper to test point-in-rect.
        Tests whether a given point is inside our rect bounds, with optional
        margin for slop-factor with inaccurate tests (i.e., finger-hit-box).
        @param x X coordinate of point in UI space
        @param y Y coordinate of point in UI space
        @param mx Margin in pixels in x-axis
        @param my Margin in pixels in y-axis
        @return true if tests as inside rect, false otherwise.
        */
    virtual bool Inside(float x, float y, float mx=0, float my=0)
        {
            if (x >= left-mx && x <= left+width+mx)
                if (y >= top-my && y <= top+height+my)
                    return true;
            return false;
        }

    /** Helper to grow the rectangle by a given width and height, keeping existing 'center'.
        This is used to build 'outer' frames, borders, focus rectangles. */
    virtual void Grow(float w, float h)
        {   
            // when add gravity, need to adjust +/- here.. todo.
            left -= w*0.5f;
            top -= h*0.5f;
            width += w;
            height += h;
        }

    friend class NvUIElement;
};


// fwd decl container, as elements hold their parent whenever possible.
class NvUIContainer;

//=============================================================================
// >>> ABSTRACT BASE CLASS <<<
//=============================================================================
/** The abstract base class of the NvUI framework.

    All UI classes are derived from NvUIElement.  It provides all the needed
    member variables and methods for onscreen user interface elements.
*/
class NvUIElement
{
protected:
    uint32_t m_uiuid;           /**< Stores a unique 32-bit identifier (internal to a given app) for a given element.
                                Useful for differentiating or identifying elements without referencing pointers
                                directly, such as inside of a raised NvUIReaction. */

    NvUIRect m_rect;            /**< Stores the onscreen position and size. */

    bool m_isVisible;           /**< Whether the element is currently visible/drawn. */
    bool m_isInteracting;       /**< Whether this object got the 'down' action, thus interacting with the user. */
    bool m_canFocus;            /**< Whether the element can take focus. */
    bool m_hasFocus;            /**< Whether the element or a child has the focus. */
    bool m_showFocus;           /**< Whether the element wants to have focus shown when it has focus; some cotainers may not. */
    bool m_canMoveFocus;        /**< Whether this element can move focus to another element, usually a container. */

    float m_alpha;              /**< Stores the current forced 'alpha-fade' level. Uses normal GL [0.0-1.0] expected range. */

    uint32_t m_slideInteractGroup; /**< An identifier for items which can have focus/hit 'slid' between them.
                                The SlideInteractGroup is used to aid in cases where it is legal and necessary to 'slide'
                                the user's interaction between a group of elements while the pointer is still held down, 
                                e.g. dragging through the menu items of a popup menu, or across keys of a virtual keyboard. */

    NvUIContainer *m_parent;    /**< The container that holds this element. */

public:
    /** Default constructor */
    NvUIElement()
        : m_uiuid(ms_uiuid_next++)
        , m_rect()

        , m_isVisible(true)
        , m_isInteracting(false)
        , m_canFocus(false)
        , m_hasFocus(false)
        , m_showFocus(true)
        , m_canMoveFocus(false)

        , m_alpha(1.0f)
        , m_slideInteractGroup(0)
        , m_parent(NULL)

        , m_currDrawState(0)
        , m_prevDrawState(0)
        , m_maxDrawState(0)
        , m_llnext(NULL)
        , m_llprev(NULL)
        { /* empty */ };
    
    /** Default destructor */
    virtual ~NvUIElement();

    /** Pure virtual rendering method for widgets.
        Pure virtual as there is no base implementation, it must be implemented by each widget subclass. */
    virtual void Draw(const NvUIDrawState &drawState) = NV_PURE_VIRTUAL; 

    /** Virtual user interaction handling method.
        We implement a base version to return not-handled, so that non-interactive classes don't have to.
        @param ev The current NvGestureEvent to handle
        @param timeUST A timestamp for the current interaction
        @param[in,out] hasInteract The element that had interaction last, or has taken over current interaction.
    */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
        {
           return nvuiEventNotHandled;
        }

    /** Virtual method for handling NvUIReaction in response to user interaction.
        We implement a base version to return not-handled, so that non-reacting classes don't have to.
    */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react)
        {
           return nvuiEventNotHandled;
        }

    /** Virtual method for reacting to a change in viewport size.
        Not many UI elements will need to handle this direct, so base is a no-op method. */
    virtual void HandleReshape(float w, float h)
        {
            /* no-op */
        }

    /** Accessor for whether this element can be focused -- or has a child that can. */
    virtual bool CanFocus()
        {
            return m_isVisible && m_canFocus;
        }

    /** Accessor for whether this element can move the focus itself, generally only true for containers. */
    bool CanMoveFocus()
        {
            return m_canMoveFocus;
        }

    /** Accessor for whether this element HAS the input focus (or has a child that does). */
    virtual bool HasFocus()
        {
            return m_hasFocus;
        }

    /** Accessor for whether this element's focus state should be shown (containers might not always want it shown). */
    virtual bool ShowFocus()
        {
            return m_showFocus;
        }

    /** Virtual method for moving the highlight focus between UI elements or acting upon the selected one. 
        @return true if we were able to move the focus or act on it, false otherwise. */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt)
        {
            return nvuiEventNotHandled;
        }

    /** Virtual method for setting 2D origin point in UI/view space.
        Base implementation simply sets the NvUIElement's rectangle top-left to the passed in values.
        Virtual as some subclasses may override to reposition children or account for padding/margins. */
    virtual void SetOrigin(float x, float y)
        { // unless overridden, just drop into the m_rect top/left.
            m_rect.left = x;
            m_rect.top = y;
        }

    /** Virtual method for setting the dimensions of this element in pixels.
        Base implementation simply sets the NvUIElements rectangle width and height to passed in values. */
    virtual void SetDimensions(float w, float h)
        {
            m_rect.width = w;
            m_rect.height = h;
        }

    /** Virtual method for changing just the width of this element.
        Leverages SetDimensions so all subclasses will get desired results without further effort. */
    virtual void SetWidth(float w)
        {
            SetDimensions(w, m_rect.height);
        }

    /** Virtual method for changing just the height of this element.
        Leverages SetDimensions so all subclasses will get desired results without further effort. */
    virtual void SetHeight(float h)
        {
            SetDimensions(m_rect.width, h);
        }

    /** Accessor to set the Z/depth value of our UI rectangle. */
    virtual void SetDepth(float z)
        {
            m_rect.zdepth = z;
        }
    
    /** Method to test whether the element's bounding rect has a non-zero Z/depth value. */
    virtual bool HasDepth()
        {
            return (m_rect.zdepth > 0);
        }
    
    /** Accessor to retrieve the UI-space NvUIRect for this element into a passed-in reference. */
    virtual void GetScreenRect(NvUIRect& rect)
        {
            rect = m_rect;
        }

    /** Accessor to retrieve the UI-space NvUIRect for this element's focus rectangle. */
    virtual void GetFocusRect(NvUIRect& rect)
        {
            rect = m_rect;
        }

    /** Accessor to get the width of this element's bounding rect. */
    virtual float GetWidth()
        {
            return m_rect.width;
        }
    
    /** Accessor to get the height of this element's bounding rect. */
    virtual float GetHeight()
        {
            return m_rect.height;
        }
      
    /** Set whether or not this element is visible and thus to be drawn. */
    virtual void SetVisibility(bool show) // virtual for customization.
        {
            m_isVisible = show;
        }

    /** Get whether or not this element is visible and thus to be drawn. */
    virtual bool GetVisibility() // virtual for customization.
        {
            return(m_isVisible);
        }

    /** Set the alpha-blend amount for this element. */
    virtual void SetAlpha(float a) // virtual for customization.
        {
            m_alpha = a;
        }

    /** Get the current alpha-blend override level for this element. */
    virtual float GetAlpha()
        {
            return m_alpha;
        }

    /** Get a const reference to the current/active NvUIReaction object. */
    static const NvUIReaction& GetReaction()
        {
            return ms_reaction;
        }

    /** Get a NON-const reference to the current NvUIReaction object for editing.
        @param clear Defaults to true to wipe our prior reaction state.  Pass false to leave intact to make minor changes to existing reaction. */
    static NvUIReaction& GetReactionEdit(bool clear=true)
        {
            if (clear)
                memset(&ms_reaction, 0, sizeof(ms_reaction));
            return ms_reaction;
        }

    /** Virtual method for telling this element it has lost interaction. */
    virtual void LostInteract()
        {
        }

    /** Virtual method for telling this element its is no longer focus. */
    virtual void DropFocus()
        {
        }

    /** Get the parent NvUIContainer, if one was set. */    
    virtual NvUIContainer* GetParent() { return m_parent; };
    /** Set the parent NvUIContainer, so a child knows who currently 'owns' it. */    
    virtual void SetParent(NvUIContainer* p) { m_parent = p; };
    
    /** Get the SlideInteractGroup identifier for this element. */
    virtual uint32_t GetSlideInteractGroup() { return m_slideInteractGroup; };
    /** Set the SlideInteractGroup identifier for this element. */
    virtual void SetSlideInteractGroup(uint32_t group) { m_slideInteractGroup = group; };

    /** Get the SlideInteractGroup identifier that is currently active during user interaction. */
    static uint32_t GetActiveSlideInteractGroup() { return ms_activeSlideInteractGroup; };
    /** Set the SlideInteractGroup identifier that is currently active during user interaction. */
    static void SetActiveSlideInteractGroup(uint32_t group) { ms_activeSlideInteractGroup = group; };

    /** Get the construction-time unique identifier for this element. */
    uint32_t GetUID()
        {
            return m_uiuid;
        }

    /** Hit-test a given point against this element's UI rectangle, using no extra margin. */
    virtual bool Hit(float x, float y)
        {
            return (m_rect.Inside(x, y, 0, 0));
        }

    /** Notify the NvUI system of a system/window resolution change, so it can resize buffers and such. */
    static void SystemResChange(int32_t w, int32_t h);

    /** Get the current drawing 'state' or index.
        Primarily used for multi-state objects like Buttons to have active vs selected/highlighted, vs inactive states tracked, and those states can then be used to render different visuals. */
    inline uint32_t GetDrawState()
        {
            return m_currDrawState;
        }

    /** Set the current drawing 'state' or index.
        Primarily used for multi-state objects like Buttons to have active vs selected/highlighted, vs inactive states tracked, and those states can then be used to render different visuals. */
    virtual void SetDrawState(uint32_t n) // must be virtual so we can catch it
        {
            if (n<=m_maxDrawState)
                m_currDrawState = n;
        }
        
    /** Set the current drawing 'state' or index back to a stashed prior value.
        Primarily used for objects that want to temporarily, briefly change state, and then revert back -- such as a push-button widget going active->selected->active.  This restores to last stashed value. */
    inline void SetDrawStatePrev()
        {
            SetDrawState(m_prevDrawState);
        }
        
    /** Get the prior drawing 'state' or index.
        Primarily used for objects that want to temporarily, briefly change state, and then revert back -- such as a push-button widget going active->selected->active.  This gets the last stashed value. */
    inline uint32_t GetPrevDrawState()
        {
            return m_prevDrawState;
        }

    /** Set the prior drawing 'state' or index.
        Primarily used for objects to stash prior visual state while they temporarily display a different state.  This stashes a passed value (generally the current draw state). */
    inline void SetPrevDrawState(uint32_t n)
        {
            if (n<=m_maxDrawState)
                m_prevDrawState = n;
        }

    /** Get maximum draw state value supported by this object.
        Used for classes of objects which have the potential to have many different visual states, but may only be set up with physical visuals for a select set.  For example, many buttons may have active and pressed/selected visuals, but may not bother with inactive visual. */    
    inline uint32_t GetMaxDrawState()
        {
            return m_maxDrawState;
        }

    /** Get maximum draw state value supported by this object.
        Used for classes of objects which have the potential to have many different visual states, but are only set up a select set. */
     inline void SetMaxDrawState(uint32_t n)
        {
            m_maxDrawState = n;
        }

private:
    uint32_t m_currDrawState; /**< Stores the current drawing 'state' or index (useful for multi-state objects like Buttons). */
    uint32_t m_prevDrawState; /**< The previous drawing 'state' or index (useful for things like Button hit-tracking). */
    uint32_t m_maxDrawState; /**< The maximum drawing 'state' or index. */

    static NvUIReaction ms_reaction; /**< Static object storing the 'current running' Reaction.
                                        We keep a static member for easier access and to elimate any dynamic allocation.
                                        Also, there is should only ever be one reaction 'in processing' at any given time. */
    static uint32_t ms_uiuid_next; /**< Holds the UIUID index for the to-be-created-next NvUIElement. */
    static int32_t ms_designWidth; /**< Optional design width for the entire UI hierarchy of a given application -- can be 0. */
    static int32_t ms_designHeight; /**< Optional design height for the entire UI hierarchy of a given application -- can be 0. */
    static uint32_t ms_activeSlideInteractGroup;  /**< The current active SlideInteractGroup identifier set during most recent PRESS event. */
   
    // declare container as friend so it can access the internal LL variables,
    // until we otherwise reimplement container storage as a std::vector or map or the like.
    friend class NvUIContainer;
    class NvUIElement *m_llnext, *m_llprev; /**< built-in doubly-linked-list pointers. */
};


//=============================================================================
//=============================================================================
/** A UI element that 'proxies' calls to another element.
    This is used to create further specialized subclasses of NvUIElement that have
    an existing type of widget they want to use for their visual and iteractive methods,
    but can't subclass as it could be ANY class and isn't known up front which ones
    would be used.
    */
class NvUIProxy : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);
    NvUIElement *m_proxy;
    
public:
    NvUIProxy(NvUIElement *el)
        : m_proxy(el) // we own the proxy now.
        { /* empty */ };
    
    virtual ~NvUIProxy()
    {
        delete m_proxy; // since we owned it.
    }

    // --- OVERRIDE VIRTUALS TO PROXIED OBJECT ---
    virtual void Draw(const NvUIDrawState &drawState)
    { m_proxy->Draw(drawState); }
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
    { return m_proxy->HandleEvent(ev, timeUST, (hasInteract==this)?m_proxy:hasInteract); }
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react)
    { return m_proxy->HandleReaction(react); }
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt)
    { return m_proxy->HandleFocusEvent(evt); }

    virtual void SetOrigin(float x, float y)
    { m_proxy->SetOrigin(x,y); }
    virtual void SetDimensions(float w, float h)
    { m_proxy->SetDimensions(w,h); }
    virtual void SetDepth(float z)
    { m_proxy->SetDepth(z); }   
    virtual bool HasDepth()
    { return m_proxy->HasDepth(); }

    virtual void GetScreenRect(NvUIRect& rect)
    { m_proxy->GetScreenRect(rect); }
    virtual void GetFocusRect(NvUIRect& rect)
    { m_proxy->GetFocusRect(rect); }
    virtual float GetWidth()
    { return m_proxy->GetWidth(); }
    virtual float GetHeight()
    { return m_proxy->GetHeight(); }
      
    virtual void SetVisibility(bool show)
    { m_proxy->SetVisibility(show); }
    virtual bool GetVisibility()
    { return m_proxy->GetVisibility(); }

    virtual void SetAlpha(float a)
    { m_proxy->SetAlpha(a); }
    virtual float GetAlpha()
    { return m_proxy->GetAlpha(); }

    virtual void LostInteract()
    { m_proxy->LostInteract(); }

    virtual void DropFocus()
    { m_proxy->DropFocus(); }
    
    virtual uint32_t GetSlideInteractGroup()
    { return m_proxy->GetSlideInteractGroup(); }

    /** While we DO have a UID for ourselves, we always return the UID of the proxy instead. */
    uint32_t GetUID()
    { return m_proxy->GetUID(); }

    virtual bool Hit(float x, float y)
    { return m_proxy->Hit(x,y); }

    //=============================================================================
    // !!!!TBD
    //inline uint32_t GetDrawState()
    virtual void SetDrawState(uint32_t n)
    { m_proxy->SetDrawState(n); }
    //inline void SetDrawStatePrev();
    //inline uint32_t GetPrevDrawState();
    //inline void SetPrevDrawState(uint32_t n);
    //inline uint32_t GetMaxDrawState();
    //inline void SetMaxDrawState(uint32_t n);
};

class NvImage;
class NvUITextureRender {
public:
    virtual ~NvUITextureRender() { /* */ }
    virtual bool IsTexValid() = 0;
};

//=============================================================================
//=============================================================================
/** Abstraction of a texture object, allowing for texture caching, lookup, and reuse/refcounting.
    In addition, it stores a set of 'knowledge' about the texture data loaded in the object,
    including things like the source filename and dimensions.
*/
class NvUITexture
{
private:
    // !!!!TBD TODO change this over to a std::map or hash table.
    NvUITexture *m_llnext; /**< Internally linked list. */
    static const int32_t NV_UITEX_HASHMAX = 19; /**< Some random prime num picked for hashing. !!!!TBD */
    static NvUITexture *ms_texTable[NV_UITEX_HASHMAX]; /**< The texture hash table.  Using chaining for filled slots. */

protected:
    std::string m_filename; /**< The requested bitmap filename, used in hashing and debugging. */
    NvUITextureRender* m_render; /**< The GL texture ID allocated for our texture upload. */
    int32_t m_width; /**< The loaded texture width. */
    int32_t m_height; /**< The loaded texture height. */
    bool m_hasAlpha; /**< Flag for if this texture has data with an alpha channel. */
    uint32_t m_refcount; /**< Trivial internal refcount of a given texture object. */ // !!!!TBD TODO use a real ref system?
    bool m_cached; /**< Whether or not the texture is cached in our master NvUITexture table. */

private:
    /** Static method to help calculate a hash-table value based on a texture name string. */
    static uint32_t CalculateNameHash(const std::string& texname);
    /** Handles internal dereferencing of texture objects, may include removal of entry from cache. */
    bool DerefTexture(); 

public:
    /** Constructor for texture loaded from filename; may load from/to the texture cache. */
    NvUITexture(const std::string& texname, bool noMips=true);
    /** Default destructor, helps with refcounting and cleanup of owned textures. */
    virtual ~NvUITexture();
    
    /** Accessor for texture width. */
    inline int32_t GetWidth() { return m_width; };
    /** Accessor for texture height. */
    inline int32_t GetHeight() { return m_height; };
    /** Accessor for GL texture object ID. */
    inline uint32_t IsTexValid() { return m_render && m_render->IsTexValid(); };
    /** Accessor for whether texture was flagged as having alpha channel. */
    inline bool GetHasAlpha() { return m_hasAlpha; };

    inline NvUITextureRender* GetRender() { return m_render; }
    
    /** Add reference to this texture. */
    void AddRef() { m_refcount++; }; // that one is easy.
    /** Subtract a reference from this texture/ */
    void DelRef(); // this one is trickier...

    /** Static method for loading textures via internal cache.
        First tries to find existing object in the cache.  If not found in the cache, tries to
        load from disk, and if successful then store in the cache for later load attempts.
    */
    static NvUITexture *CacheTexture(const std::string& texname, bool noMips=true);
};

class NvUIGraphic;

class NvUIGraphicRender {
public:
    virtual ~NvUIGraphicRender() { /* */ }
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4]) = 0;
};

//=============================================================================
// The base, static visual element class.
//=============================================================================
/** A graphical element, with no interactivity.

    This class renders a texture (or portion of one) to the screen, but has 
    no interactivity with the user, just the visual.  Other UI classes derive
    from this functionality, or otherwise utilize or proxy to it.
    
    NvUIGraphic can be initialized from a texture on disk, an existing NvUITexture
    object, or an existing GL texture ID.
*/
class NvUIGraphic : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);

protected:
    NvUITexture *m_tex; /**< The texture we render from. */
    bool m_scale; /**< Are we scaling the texture (pixel size other than original width/height texel size)? */
    NvPackedColor m_color; /**< An RGB color to modulate with the texels of the graphic (alpha is ignored for the moment) */ // !!!!TBD
    
private:
    NvUIGraphicRender *m_render;

public:
    /** Basic constructor, taking texture filename as source.
        Note it uses the texture size for target w/h unless default params overridden,
        in which case it will scale the texture appropriately.
        @param texname Texture filename (and path if any needed) in a std::string.
        @param dstw Target width dimension, defaults to texture width if not overridden.
        @param dsth Target height dimension, defaults to texture height if not overridden.
         */
    NvUIGraphic(const std::string& texname, float dstw=0, float dsth=0);
    /** Basic constructor, taking NvUITexture object as source.
        Note it uses the texture size for target w/h unless default params overridden,
        in which case it will scale the texture appropriately.
        @param tex Source NvUITexture object pointer.
        @param dstw Target width dimension, defaults to texture width if not overridden.
        @param dsth Target height dimension, defaults to texture height if not overridden.
         */
    NvUIGraphic(NvUITexture* tex, float dstw=0, float dsth=0);
    virtual ~NvUIGraphic();
    /** Loads a texture from cache or file.
        This actually ends up loading data using the @ref NvUITexture::CacheTexture method,
        to take advantage of the texture cache, and loaded from disk into the cache if not
        already there.
        @param texname Filename of texture to load
        @param resetDimensions Whether to set our dimensions to match those of the texture.  Defaults true.
    */
    virtual bool LoadTexture(const std::string& texname, bool resetDimensions=true);
    /** Change this graphic to use a specific NvUITexture object. */
    void SetTexture(NvUITexture *tex);

    //virtual void SetDimensions(float w, float h); // no need to override atm.

    /** Does the heavy lifting to render our texture at target position/dimensions. */
    virtual void Draw(const NvUIDrawState &drawState); // leaf, needs to implement!

    /** Sets a color value to multiply with during fragment processing.
        Setting to white (1,1,1,x) color effectively disables colorization.
    */
    virtual void SetColor(NvPackedColor color);
    
    /** If we have a texture object, deletes our reference, and nulls out our member. */
    void FlushTexture(); // made public in case needed.

    NvUITexture* GetTex() { return m_tex; }
    
private:
    bool StaticInit();
    void StaticCleanup();
    void PrivateInit(void);

    static float s_pixelScaleFactorX;
    static float s_pixelScaleFactorY;
    static int32_t s_pixelXLast;
    static int32_t s_pixelYLast;
    static float s_graphicWidth, s_graphicHeight;
};

class NvUIGraphicFrameRender {
public:
    virtual ~NvUIGraphicFrameRender() { /* */ }
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4], 
                            const nv::vec2<float>& thickness, const nv::vec2<float>& texBorder,
                            bool drawCenter) = 0;
};


//=============================================================================
// STATIC visual element
//=============================================================================
/** A graphical frame element, representing a background and border combined.

    This class implements a flexible/dynamic frame system based off the general
    NvUIGraphic base class, letting the base class manage texture loading, etc.

    Then, this subclass adds the concept of drawing a properly-formed frame
    texture which has designed in a non-stretched border area which is used in
    calculating mapping from texel space (source) to pixel space (destination).
    The frame object can then be resized with the frame border remaining a fixed
    'thickness', but the edges and center stretching and contracting to match the
    given size/rect.
*/
class NvUIGraphicFrame : public NvUIGraphic
{
private:
    INHERIT_FROM(NvUIGraphic);

protected:
    nv::vec2<float> m_texBorder; /**< How many pixels in the texture is considered to be a border */
    nv::vec2<float> m_borderThickness; /**< Thickness of the border when drawing */
    bool m_drawCenter; /**< Whether or not to draw the center piece or just the border */

private:
    NvUIGraphicFrameRender* m_render;

public:
    /** Constructor for a texture file that has a single x/y border thickness. */
    NvUIGraphicFrame(const std::string& texname, float texBorder);
    /** Constructor for a texture file with independent x and y border thicknesses. */
    NvUIGraphicFrame(const std::string& texname, float texBorderX, float texBorderY);
    /** Constructor for an existing NvUITexture that has a single x/y border thickness. */
    NvUIGraphicFrame(NvUITexture *uiTex, float border);
    /** Default destructor, tries to ensure memory is released. */
    virtual ~NvUIGraphicFrame();

    /** Loads a texture from cache or file.
        We override the default handling to force @p resetDimensions to false, as frames
        inherently stretch to render/fill the specified destination rectangle, and thus
        we don't want to take on the dimensions of the texture by default.
    */
    virtual bool LoadTexture(const std::string& texname, bool resetDimensions=true);

    /** Set a single x/y border thickness for this frame. */
    void SetBorderThickness(float thickness);
    /** Set independent x and y border thicknesses for this frame. */
    void SetBorderThickness(float width, float height);
    /** Get the x and y border thicknesses for this frame. */
    void GetBorderThickness(float *x, float *y);
    /** Flag for whether to draw the center piece of the 3x3 frame grid, or ignore that quad. */
    void SetDrawCenter(bool m_drawCenter);

    /** Renders the frame texture appropriately stretched to fit the target position/dimensions. */
    virtual void Draw(const NvUIDrawState &drawState); // Override parent class drawing

private:
};

//=============================================================================
//=============================================================================
/** Default font families you may reference.

    This enum represents the families/typefaces for use in NvUI-based applications.

    We currently use a default font that is sans-serif, generated from Roboto Condensed,
    and a monospaced font generated from Courier New.  Both fonts have normal and bold
    styles in their respective bitmap files, so have bolding supported.
    @see NvBitFont.h
    @see NvUIText
*/
struct NvUIFontFamily {
    enum Enum
    {
        DEFAULT = 0, /**< Default font as initialized */
        SANS = DEFAULT, /**< Sans serif style font */
        MONO = 1, /**< Monospaced style font */
        COUNT // should get set to right value...
    };
};

/** Basic text alignment.

    This enum is a clone of the alignments from BitFont's NvBftAlign enum, in order
    to separate the two and not create a hard dependency between the system (as we
    might decide to change the text rasterizer under the hood of NvUI, for example).
    @see NvBftAlign
*/
struct NvUITextAlign {
    enum Enum {
        LEFT = 0, /**< Align to left */
        RIGHT = 1, /**< Align to right */
        CENTER = 2, /**< Align to center */
        TOP  = 0, /**< Align to top */
        BOTTOM = 1 /**< Align to bottom */
    };
};

//=============================================================================
// STATIC visual element
//=============================================================================
/** A UI element that renders text strings to the view.

    This class wrappers rendering of a string to the screen using @p NvBitFont.
    
    It exposes all the matching functionality, including font, size, color,
    drop-shadow, alignment 'text box', and multi-line output support.
    @see NvBitFont.h
*/
class NvUIText : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);

protected:
    NvBFText *m_bftext; /**< The NvBitFont BFText object that does actual text rendering. */
    float m_size; /**< Local cache of the original font size. */
    NvPackedColor m_color; /**< Modulation of the text with an RGB color */
    bool m_wrap; /**< Whether we wrap or truncate if exceed drawable width. */
    static const char DEFAULT_SHADOW_OFFSET = 3; /**< The default/canonical shadow offset value. */
public:
    /** Default constructor for onscreen text element.
        @param str Text string to display.
        @param font Font family to use for this text.
        @param size Font size to use for this text.
        @param halign Alignment to use for this text.
    */
    NvUIText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum halign);
    virtual ~NvUIText();
    
    /** Static helper method to get the specific font ID for a given font family enum. */
    static uint8_t GetFontID(NvUIFontFamily::Enum font);
    /** Static helper method for initializing underlying text system with current view size. */
    static bool StaticInit(float width=1280, float height=720);
    /** Static helper method for cleaning up any static held items in the text system. */
    static void StaticCleanup();
    
    /** Set the horizontal alignment of the text. */
    virtual void SetAlignment(NvUITextAlign::Enum halign);
    /** Set the alpha transparency of the text. */
    virtual void SetAlpha(float alpha);
    /** Set the overall color of the text. */
    virtual void SetColor(const NvPackedColor& color);
    /** Enable a drop-shadow visual under the text string.
        @param offset A positive (down+right) or negative (up+left) pixel offset for shadow.  Default is +2 pixels.  Set 0 to disable.
        @param color An NvPackedColor for what color the drop-shadow should be.  Default is black.
    */
    virtual void SetShadow(char offset=DEFAULT_SHADOW_OFFSET, NvPackedColor color=NV_PC_PREDEF_BLACK);
    /** Assign a box the text will render within.
        The box will be used to align, wrap, and crop the text.
        <ul>
        <li>If @p lines is 1, only a single line will be drawn, cropped at the box
        width.</li>
        <li>If @p lines is 0, we will wrap and never crop.</li>
        <li>If @p lines is >1, we will wrap for that many lines, and crop on the final line.</li>
        <li>If @p dots is a valid character, when we crop we won't simply truncate, will
        will back up enough to fit in three of the @p dots character as ellipses.</li>
        <li>When a box is active, all alignment settings on the text become relative to the
        box rather than the view.</li>
        <li>If width or height is zero, the box will be cleared/disabled.</li>
        </ul>
        @param width Set the width of the box
        @param height Set the height of the box
        @param lines Set the number of vertical lines allowed for the text
        @param dots Set the character to use as ellipses
    */
    virtual void SetTextBox(float width, float height, uint32_t lines, uint32_t dots);

    /** Override so if a box has been set, we update the width and height with these values. */
    virtual void SetDimensions(float w, float h);

    /** Make proper calls to the text rendering system to draw our text to the viewport. */
    virtual void Draw(const NvUIDrawState &drawState); // leaf, needs to implement!

    /** Set the string to be drawn. */
    void SetString(const char* in);
    /** Set the font size to use for our text. */
    virtual void SetFontSize(float size);
    /** Get our font size value. */
    float GetFontSize() { return m_size; }

//    virtual void GetString(char *string, uint32_t buflen); // copies out to a buffer.

    /** Get the approximate output pixel width calculated for our text. */
    float GetStringPixelWidth();

    /** Set an explicit character count truncation for drawing our text.
        This is handy helper function for doing simple 'type on' animated visuals. */
    void SetDrawnChars(int32_t count);
};


//=============================================================================
// DYNAMIC visual element
//=============================================================================
/** A UI element that converts numeric values to text and draws title and value.

    This class wrappers rendering of a title string and a value string to the screen
    leveraging NvUIText.

    Valid combinations of alignments of the two items and the resulting output are:
    - title: left, value: left
        ==> [TITLE VALUE         ]
    - title: left, value: right
        ==> [TITLE          VALUE]
    - title: right, value: right
        ==> [         VALUE TITLE]
    
    @see NvUIText
*/
class NvUIValueText : public NvUIText
{
private:
    INHERIT_FROM(NvUIText);

protected:
    float m_value;                    /**< The current value. */
    bool m_integral;                  /**< Whether or not the value is actually an integer. */
    uint32_t m_precision;             /**< Number of digits after decimal point to display, zero for default string formatting. */
    NvUIText *m_valueText;            /**< The NvUIText defining the output value text. */
    NvUITextAlign::Enum m_titleAlign; /**< The alignment of the title output, so we can position value relative to title. */
    NvUITextAlign::Enum m_valueAlign; /**< The alignment of the value output, so we can position relative to title. */
    uint32_t m_action;                /**< If nonzero, the action code we respond to for changing our value. */

public:
    /** Constructor for onscreen text element displaying a floating-point value.
        @param str Title string to display.
        @param font Font family to use for this text.
        @param size Font size to use for this text.
        @param titleAlign Alignment to use for the title text.
        @param value Starting value to show.
        @param precision Number of digits after decimal point to display.
        @param valueAlign Alignment to use for the value text.
        @param actionCode [optional] Reaction code to match to set our value.
    */
    NvUIValueText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum titleAlign,
                float value, uint32_t precision, NvUITextAlign::Enum valueAlign, uint32_t actionCode=0);

    /** Constructor for onscreen text element displaying an integer value.
        @param str Title string to display.
        @param font Font family to use for this text.
        @param size Font size to use for this text.
        @param titleAlign Alignment to use for the title text.
        @param value Starting value to show.
        @param valueAlign Alignment to use for the value text.
        @param actionCode [optional] Reaction code to match to set our value.
    */
    NvUIValueText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum titleAlign,
                uint32_t value, NvUITextAlign::Enum valueAlign, uint32_t actionCode=0);
    virtual ~NvUIValueText();
    
    /** Set the horizontal alignment of the value text. */
    void SetValueAlignment(NvUITextAlign::Enum halign);
    /** Set the alpha transparency of both text strings. */
    virtual void SetAlpha(float alpha);
    /** Set the overall color of both text strings. */
    virtual void SetColor(const NvPackedColor& color);
    /** Set the color of just the value text string. */
    void SetValueColor(const NvPackedColor& color);
    /** Set drop-shadow features on the text strings. */
    virtual void SetShadow(char offset=DEFAULT_SHADOW_OFFSET, NvPackedColor color=NV_PC_PREDEF_BLACK);

    /** Set a box the two strings will render within.
        @see NvUIText
    */
    virtual void SetTextBox(float width, float height, uint32_t lines, uint32_t dots);

    /** Override to set dimensions of both text strings. */
    virtual void SetDimensions(float w, float h);

    /** Override to set origin of both text strings. */
    virtual void SetOrigin(float x, float y);

    /** Override to draw both title and value strings to the viewport. */
    virtual void Draw(const NvUIDrawState &drawState);

    /** Set the font size to use for both text strings. */
    virtual void SetFontSize(float size);

    /** Set the value to be drawn as a string. */
    void SetValue(float value);
    /** Set an integer value to be drawn as a string. */
    void SetValue(uint32_t value);

    /** Override to set our value if an appropriate reaction comes through. */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);

private:
    void PositionOutput();
};


//=============================================================================
//=============================================================================
/** Basic button states.

    This enum encapsulates the base three states of a button visual: inactive,
    active, and selected/pressed.
*/
struct NvUIButtonState {
    enum Enum {
        ACTIVE=0, /**< Button is active, can be interacted with. */
        SELECTED, /**< Button is selected, user is actively interacting with it. */
        INACTIVE, /**< Button is inactive, not able to interact with it. */
        MAX
    };
};

/** Basic button types.

    This enum defines the types of buttons we understand in the system, 
    including basic 'push' button, radio button, and checkbox style interactions.
*/
struct NvUIButtonType {
    enum Enum {
        PUSH,    /**< Button where state comes up automatically after click. */
        RADIO,   /**< Button where state sticks 'in' when clicked, others in same group release. */
        CHECK,   /**< Button where state toggles on/off on each click. */
    };
};

//=============================================================================
//=============================================================================
/** A class that implements a variety of standard interactive button widgets.

    This class uses abstract visual representations of the three button states,
    combined with the logic behind the types of interactions of the three types of
    buttons, and the concept in the system of a 'reaction' to interacting with a
    button, and wraps it all up with end-to-end event handling.
    
    This can represent a single graphical or textual (or other) element on the
    screen that is clickable.
    
    While tracking a press within the button, the @p HandleEvent method will attempt
    to keep the focus on the button, so that it will get 'first shot' at further
    event input in the system.

    @see NvUIElement
    @see NvUIReaction
    @see NvUIEventResponse
*/
class NvUIButton : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);

protected:
    NvUIElement *m_visrep[NvUIButtonState::MAX]; /**< Abstract UI element array of visual representations we proxy as our button visual.  Must have at least the 'active' state defined.  Can be text, graphics, complex container, etc. */
    NvUIText *m_title; /**< UIText-drawn title for a button, if one isn't baked into the graphic rep. */
    NvUIButtonType::Enum m_type; /**< The 'class' of button (which defines its 'style' of interaction): push, radio, or checkbox. */

    uint32_t m_action; /**< The reaction code 'posted' by the event handler when the button is pressed and released such that state changed.  Radio buttons use this to both notify change up the tree AND to notify 'linked' alternate state buttons to turn themselves back 'off'.*/
    uint32_t m_subcode; /**< A specialized reaction code used by some kinds of buttons (radio in particular) to denote which sub-item was picked by index. */

    NvGestureUID m_reactGestureUID;  /**< A gesture event UID of the last gesture that we triggered/reacted to, so we can quickly ignore further @p HandleEvent calls until the gesture UID increments. */
    NvGestureUID m_failedHitUID; /**< A gesture event UID of the last gesture that failed to hit the button, so we can quickly ignore further @p HandleEvent calls until the gesture UID increments. */
    bool m_wasHit; /**< Used by @p HandleEvent and internal hit-tracking.  Set when we had a successful press/click in the thumb, so we are then tracking for drag/flick/release. */
    bool m_reactOnPress; /**< When the button should be triggered on the initial press and not wait for release. */

    float m_hitMarginWide; /**< An extra 'padding' or margin added to the button's normal hit-test width, in order to allow for a larger test region than the button's visual.  Important for touchscreen+finger interactivity. */
    float m_hitMarginTall; /**< An extra 'padding' or margin added to the button's normal hit-test height, in order to allow for a larger test region than the button's visual.  Important for touchscreen+finger interactivity. */

    bool m_stickyClick; /**< Whether to retain the pressed-in state even if we drag outside bounds. */
   
public:
    /** Most common constructor for a graphical, multi-state visual button.
        This is the generally 'best' method for building a visual button, as you give it the
        specific visual elements built for each state (active, pressed, inactive), and thus
        have very precise control over the visual results of interaction.  Usually the visuals
        are an NvUIGraphic or NvUIGraphicFrame, but they can be any UI element (including
        containers with complex overlaid visuals, or custom visual subclasses).
        @param btntype The type/style of button
        @param actionCode The action code we raise in an NvUIReaction when our state changes
        @param visrep Array of UI elements that are our visuals for each interaction state, could have text embedded.
        @param title [optional] String to display for our title
        @param pt [optional] Font size to use if we supply title string
        @param shadowed [optional] Whether to drop-shadow our title text
        @see NvUIGraphic
    */
    NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIElement *visrep[NvUIButtonState::MAX],
        const char *title=NULL, float pt=0, bool shadowed=false); // visrep has w/h set already.
    /** Button constructor for a quick, single-visual-state button.
        This button will not show different visuals while interacting, but otherwise has the
        ability to display any graphics/text to display as a button.
        @param btntype The type/style of button
        @param actionCode The action code we raise in an NvUIReaction when our state changes
        @param visrep The UI element that is our visual representation, could have text embedded.
        @param title [optional] String to display for our title
        @param pt [optional] Font size to use if we supply title string
        @param shadowed [optional] Whether to drop-shadow our title text
    */
    NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIElement *visrep,
        const char *title=NULL, float pt=0, bool shadowed=false); // visrep has w/h set already.
    /** Button constructor for a quick, text-only button.
        @param btntype The type/style of button
        @param actionCode The action code we raise in an NvUIReaction when our state changes
        @param size The UI/view space rectangle to position the button
        @param title The string to display for our title
        @param pt The font size to use for our title
        @param shadowed [optional] Whether to drop-shadow our title text
    */
    NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIRect &size,
        const char *title, float pt, bool shadowed=false);
    /** Default destructor, cleans up visrep elements and title, if any. */
    virtual ~NvUIButton();

    /** Set the title string of the button.
        Is used by constructors to set up our text string, but can be used to
        re-set the title later on.
        @param title The string to display for our title
        @param ptsize The font size to use for our title.  Note if point size is passed in as 0, a
                    size will be selected based on the height of the element.
        @param shadowed [optional] Whether to drop-shadow our title text
    */
    virtual void SetTitle(const char *title, float ptsize, bool shadowed=false);
    /** Set the color of the title text. */
    virtual void SetTitleColor(const NvPackedColor& c);

    /** Add extra horizontal/vertical margin to thumb hit rect to make it easier to grab. */
    virtual void SetHitMargin(float hitwide, float hittall);

    /** Draw the right UI element for current state, as well as optional title text. */
    virtual void Draw(const NvUIDrawState &drawState); // visual, must implement
    /** Position sub-elements and optional title based on the button type. */
    virtual void SetOrigin(float x, float y);

    /** Handles tracking from press to release on the button object, and if needed posts appropriate NvUIReaction. */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract); // interactive, must override.

    /** Handles any reaction matching our action code.

        If the action matches our code, but the uid is zero, we assume it's a 'system message'
        telling us to override our state to what's in the reaction.  Otherwise, if we're a
        radio button and receive our code, but not our uid, we assume another radio button got
        pressed, and we clear our state back to active.
    */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react); // interactive, must override.

    /** Virtual method for moving the highlight focus between UI elements or acting upon the selected one. 
        @return true if we were able to move the focus or act on it, false otherwise. */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);

//    bool WasHit() { return m_wasHit; };

    /** We override to clear hit tracking state, and reset draw state for push buttons. */
    virtual void LostInteract();
    
    /** Accessor to get the reaction action code for this button. */
    uint32_t GetActionCode() { return m_action; };

    /** Accessor to set the reaction sub-code for this button. */
    void SetSubCode(uint32_t c) { m_subcode = c; };
    /** Accessor to get the reaction sub-code for this button. */
    uint32_t GetSubCode() { return m_subcode; };

    /** Set whether to retain pressed-in state even if user drags outside the hit rect. */
    void SetStickyClick(bool b) { m_stickyClick = b; };

    /** Set whether to react on the press rather than the release. */
    void SetReactOnPress(bool b) { m_reactOnPress = b; };

    /** Allow us to flag that we reacted to this gesture, so we don't handle again until new gesture UID. */
    void ConsumeGesture(const NvGestureEvent& ev) { m_reactGestureUID = ev.uid; }

private:
    void PrivateInit(NvUIButtonType::Enum btntype, uint32_t actionCode);

#ifdef BTN_SUPPORTS_HOVER
    bool m_wantsHover;
    bool m_wasHover;
#endif
};


//=============================================================================
//=============================================================================
#if later
/** The possible flags for operational modes of container objects. */
struct NvUIContainerFlags {
    enum Enum {
        NONE =          0x00,
        CLIPCHILDREN =  0x01, /**< Enable scissoring to clip children to our bounds. */
    };
};
#endif


//=============================================================================
//=============================================================================
/** An object that holds and manages a group of UI elements and interactions with them.

    This class implements a 'container' or 'holder' object, that has no particular
    visual of its own, rather 'proxying' its visual to the children held within it.
    It does do custom handling for having a specific 'background' visual attached
    to it (such as a graphic or a frame) to draw beneath the children.
    
    The main interface is the @p Add method, which allows you to generally add any
    kind of UI element into the container, at a specific position in space.  The
    container, if moved, will properly adjust the position of all contained child
    UI elements to keep proper relative positioning between them.  This allows for
    a container to be 'shifted' around (jumped, or slowly slid, for buttonbars or
    or 'submenus' or the like), as well as alpha-faded (as alpha in the drawstate is
    passed through to child @p Draw calls), and hidden/shown (visibility control).
    
    The container also has quite detailed handling of NvGestureEvents and NvUIReactions,
    actually implementing the main 'translation' between the two systems (that is,
    NvGestureEvents come in that a child might handle and issue an NvUIReaction to,
    and the container code passes the reaction up.  The NvUIWindow class is a specialized
    container that automatically calls down into its children after event handling to
    allow them a chance at processing any Reaction, without further explicit code by
    the developer.
    
    The container also specially manages the focus state, special-case handling
    any incoming events, and passing them to the last-focused child for a 'first
    shot' at those events before passing them to all children in general.
    
    @see NvUIWindow
    @see NvUIElement
    @see NvUIEventResponse
    @see NvUIReaction
 */
class NvUIContainer : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);

protected:
    /** A background visual to draw behind all children, stretched to fill our dimensions. */
    NvUIGraphic *m_background; 
    /** A flag for whether to report events inside the container as
        handled even if no child wants them -- thus 'consuming' them
        and preventing others from trying to use them. */
    bool m_consumeClicks;
    /** Head of a doubly-linked, non-circular list of child elements.

        The use of internal linked-list fields of NvUIElement means child can
        only exist in one container at any one time.

        We walk the list forward to draw back-to-front visuals. */
    NvUIElement *m_childrenHead; 
    /** Tail of a doubly-linked list of children.

        We need double-link so we can walk the list tail-to-head in order to
        process pointer events topmost-first. */
    NvUIElement *m_childrenTail;
    /** Number of child element on our linked-list. */
    uint32_t m_numChildren; 
    /** The child that the user has most recently interacted with. Cleared if
        the child no longer responds that it wants focus, or if the container's
        @p LostFocus method is explicitly called. */
    NvUIElement *m_childInteracting; 
    /** A temporary (short-lived), popped-up element that we want drawn over everything else. */
    NvUIElement *m_popup; 
    //NvUIContainerFlags::Enum m_flags; /**< Flags as defined by NvUIContainerFlags. */

    /** The child that is currently showing focus. */
    NvUIElement *m_childFocused;
    /** An overlaid graphic used to display which child currently has focus. */
    NvUIGraphic *m_focusHilite;

public:
    /** Normal constructor.
        Takes width and height dimensions, optional background graphic, and optional flags.
        Origin is set separately. */
    NvUIContainer(float width, float height, NvUIGraphic *bg=NULL);
    /** Normal virtual destructor. */
    virtual ~NvUIContainer();
    
    /** Set the background graphic element to draw before all children. */
    void SetBackground(NvUIGraphic *bg)
    {
        m_background = bg;
    };

    /** Set whether or not to consume all unhandled input within our bounding rect. */
    void SetConsumeClicks(bool b)
    {
        m_consumeClicks = b;
    };

    /** Set the visual highlight/framing shown around element in the container that has input focus. */
    void SetFocusHilite(NvUIGraphic *hilite) // we should refcount users...
    {
        m_focusHilite = hilite;
    };

    /** Add a child element to our list, at the desired top/left offset. */
    virtual void Add(NvUIElement *el, float desiredleft, float desiredtop);
    /** Remove a child from our list. */
    virtual bool Remove(NvUIElement *child);

    /** Hold onto a specific UI element to be drawn 'popped up' on top of all other elements. */
    virtual void AddPopup(NvUIElement *el);
    /** Clear prior popped-up element. */
    virtual void RemovePopup(NvUIElement *el);

    /** Set the top-left origin point of our bounding rect. */
    virtual void SetOrigin(float x, float y);
    /** Set width/height dimensions of our bounding rect.
        These values are especially applicable if the container is flagged to clip children to its bounds. */
    virtual void SetDimensions(float w, float h);

    /** Implements dispatching HandleEvent calls through to all contained children.
        - The focused child always gets an early shot at the event, in order to shortcut all
        further processing, since it was last to interact with the user and highly likely to
        handle the next event.
        - If there is a popup attached to us, and was not the focused child, it gets the next
        shot at the event, ahead of the normal loop over all children.
        - Lastly, we give the list of children a shot at the event. Note that we walk the child
        list tail-to-head, in order to process clicks frontmost-first.
        - After all child processing is complete, if the event still has not been handled but
        SetConsumesClicks(true) was called on us, if the event was inside us and the start of
        or continuation of a chain of events, we will consume the event by returning it as
        having been handled by us. */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract);
    /** Draws a backgound if we have one, followed by children in order of the linked-list. */
    virtual void Draw(const NvUIDrawState &drawState);

    /** Implements dispatching HandleReaction calls through to all contained children.
        As any contained NvUIElement might respond to the raised NvUIReaction, we walk the
        linked-list in normal head-to-tail order, since it is no more likely that any
        one element will handle the reaction. */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);

    /** Handle change in view size.
        Pass along the reshape to all child elements. */
    virtual void HandleReshape(float w, float h);

    /** Virtual method for moving the highlight focus between elements and acting on the selected element. 
        @return true if we were able to move the focus or act on it, false otherwise. */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);

    /** Virtual method for clearing focus from this container. */
    virtual void DropFocus();

    /** Move a specified child to be drawn in front of all others. */
    virtual bool MakeChildFrontmost(NvUIElement *child);

    /** Clear any state related to interaction. */
    virtual void LostInteract();
    
    /** Accessor for pointer to the child currently being interacted with. */
    NvUIElement *GetInteractingChild()
        {
            return m_childInteracting;
        };

    /** Accessor for the SlideInteractGroup identifier. */
    virtual uint32_t GetSlideInteractGroup()
        {
            if (m_childInteracting)
                return m_childInteracting->GetSlideInteractGroup();
            return NvUIElement::GetSlideInteractGroup(); // in case inherited does something...
        };

private:
    /* this internal/private method is what handles actually adding a
        child element to our linked list and setting back pointer. */
    virtual void Add(NvUIElement *el);
    /* Update the status of the focus, focushilite, etc., based on focused child pointer. */
    void UpdateFocusState();
};


//=============================================================================
//=============================================================================
/** A single, top-level container that can manage all NvUIElements in the view.

    Most importantly, the NvUIWindow class automatically takes care of calling
    @p HandleReaction on its children after processing the @p HandleEvent.

    @see NvUIContainer
*/
class NvUIWindow : public NvUIContainer
{
private:
    INHERIT_FROM(NvUIContainer);

public:
    /** Default constructor, takes starting window/viewport width/height.
        Also responsible for initializing the NvUIText system with the
        same dimensions.
    */
    NvUIWindow(float width, float height);

    /** Default destructor.
        Primarily exists to ensure NvUIText is cleaned up at UI shutdown.
    */
    virtual ~NvUIWindow();

    /** Overrides and calls normal @p HandleEvent, with pointer to @p this as element with focus.
    */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract);

    /** Responsible for ensuring things get appropriate resized, in as much as that
        is possible given current NvUI system design constraints.
     */
    virtual void HandleReshape(float w, float h);

    /** We override to ensure we save and restore outside drawing state around the UI calls. */
    virtual void Draw(const NvUIDrawState &drawState);
};


//=============================================================================
//=============================================================================
/** A generic "value bar" element, for progress, time/length, indicators.

    Using two NvUIGraphicFrame elements, one for the empty 'background',
    one for the filled area/value, this class implements a very flexible system
    for showing various types of bars, thermometers, and progress indicators.
    
    The empty frame can be NULL to have no background, no 'unfilled' visual --
    but the best combination is when the unfilled and filled 'bar' graphics
    are created and used as a pair.
    
    The NvUIValueBar class, once handed NvUIGraphicFrame objects representing
    the visual form of the bar, handles everything else.  It takes min/max values
    for the bar, the current value, and extracts the proper sizing that the
    filled bar needs to be set to in order to properly visualize the current
    value/state.
    
    @see NvUIElement
    @see NvUIGraphicFrame
    @see NvUISlider
 */
class NvUIValueBar : public NvUIElement
{
private:
    INHERIT_FROM(NvUIElement);

protected:
    float m_value;                  /**< The current value/progress of the bar. */
    float m_maxValue;               /**< The minimum value/progress of the bar. */
    float m_minValue;               /**< The maximum value/progress of the bar. */
    bool m_integral;                /**< Whether or not the bar should snap/round its value to an integer. */
    bool m_useRawBorders;           /**< leave frame bordering set as passed in, don't try to be smart. */
    bool m_flipDirection;           /**< Flip x or y axis fill 'direction'. */
    NvUIGraphicFrame *m_emptyFrame; /**< The NvUIGraphicFrame used for empty value/progress background. */
    NvUIGraphicFrame *m_fullFrame;  /**< The NvUIGraphicFrame used for the filled value/progress state indication. */

public:
    /** Default constructor which takes 'empty' and 'full' versions of a visual bar of some sort, overlaid to show it 'filling up'.
        @param emptybar The NvUIGraphicFrame visual for the empty/unfilled bar background
        @param fullbar The NvUIGraphicFrame visual for the filled bar overlay
        @param useRawBorders Whether to try to auto-set borders, or use the raw borders already set on the frame.  Defaults to false/auto.
        @param flipDir Whether to reverse the 'fill' direction (normally left->right or top->bottom).  Defaults false.
    */
    NvUIValueBar(NvUIGraphicFrame *emptybar, NvUIGraphicFrame *fullbar,
                    bool useRawBorders=false, bool flipDir=false);
    /** Default destructor */
    virtual ~NvUIValueBar();

    /** Override to proxy dimensions to inactive bar frame, including auto-border sizing if set. */
    virtual void SetDimensions(float w, float h);
    /** Override to proxy origin setting to our frames. */
    virtual void SetOrigin(float x, float y);

    /** Flag whether we represent integer or floating point values. */
    void SetIntegral(bool isint);
    /** Get whether we represent integer or floating point values. */
    bool GetIntegral();

    /** Override to proxy alpha setting to our frames. */
    virtual void SetAlpha(float a);

    /** Accessor to set current value for the bar. */
    virtual void SetValue(float value);
    /** Accessor to set maximum value visualized by the bar. */
    void SetMaxValue(float value);
    /** Accessor to set minimum value visualized by the bar. */
    void SetMinValue(float value);

    /** Accessor to get current value for the bar. */
    float GetValue();
    /** Accessor to get maximum value visualized by the bar. */
    float GetMaxValue();
    /** Accessor to get minimum value visualized by the bar. */
    float GetMinValue();
    
    //NvUIGraphicFrame *GetActiveFrame() { return m_active; };

    /** Must override to proxy drawing to our two frames. */
    virtual void Draw(const NvUIDrawState &drawState);

protected:
    /** Update the filled bar sizing based on current/min/max values, and rect of the empty bar. */
    void UpdateBar();
};


//=============================================================================
//=============================================================================
/** An interactive ValueBar with draggable thumb, for volume, play position, etc.

    This is a natural extension of the NvUIValueBar class, adding a graphical
    'thumb' to the bar for both visual feedback and user interactive control
    over the value (such as page, zoom level, audio volume, playback position,
    or other similar input needs).
    
    The NvUISlider is set up much like the NvUIValueBar, with the addition
    of passing in a NvUIGraphic for the thumb, and a unique NvUIReaction code
    to 'post' when the thumb has been dragged and released in a valid position.
    That Reaction can then be handled by any other UI element within the same
    'tree' as the slider, perfect for notifying an upper-level 'container' object
    that something under it has changed and needs to be handled.  This allows for
    a NvUISlider to be instantiated directly without subclassing if so desired.
    
    @see NvUIElement
    @see NvUIValueBar
    @see NvUIContainer
    @see NvUIReaction
 */
class NvUISlider : public NvUIValueBar
{
private:
    INHERIT_FROM(NvUIValueBar);

protected:
    NvUIGraphic *m_thumb; /**< The graphic drawn over the NvUIValueBar as the draggable 'thumb' piece. */
    bool m_wasHit; /**< Used by @p HandleEvent and internal hit-tracking. 
                        Set when we had a successful press/click in the thumb,
                        so we are then tracking for drag/flick/release. */

    float m_startValue; /**< The thumb 'value' cached at the start of a drag, in order to reset back on a 'failed' drag interaction. */
    float m_stepValue; /**< The amount to step the value at a time while dragging the thumb -- 'tick marks'. */

    uint32_t m_action; /**< The NvUIReaction code 'posted' by the event handler when the thumb is released in a valid position. */
    bool m_smooth; /**< If set true, this causes constant reactions during live dragging of the thumb.  Defaults false. */

public:
    /** Default constructor specializing on NvUIValueBar to make an interactive slider.
        Adds an interactive 'thumb' on top of a value bar to allow the user to manually adjust its value.
        @param emptybar The NvUIGraphicFrame visual for the empty/unfilled bar background
        @param fullbar The NvUIGraphicFrame visual for the filled bar overlay
        @param thumb An NvUIGraphic for the draggable 'thumb' button that sits over the bar.
        @param actionCode The action code sent in a reaction when there has been a value change.
    */
    NvUISlider(NvUIGraphicFrame *emptybar, NvUIGraphicFrame *fullbar, NvUIGraphic *thumb, uint32_t actionCode);
    /** Default destructor. */
    virtual ~NvUISlider();

    /** Override for specialized input handling of dragging the thumb on the bar and generating reactions. */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract); // interactive, must override
    /** Override for handling someone other than use send a reaction with our code as a way to pass along value changes. */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react); // interactive, must override
    /** Override for handling INC/DEC action events when bar has focus. */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);

    /** Override to handle proxying to thumb element. */
    virtual void SetDimensions(float w, float h);
    /** Override to handle proxying to thumb element. */
    virtual void SetOrigin(float x, float y);
    /** Override to handle proxying to thumb element. */
    virtual void SetAlpha(float a);

    /** Override to have better control over when value really changes, when visual elements update. */
    virtual void SetValue(float value);

    /** Add extra horizontal/vertical margin to thumb hit rect to make it easier to grab. */
    virtual void SetHitMargin(float hitwide, float hittall);

    /** Accessor to retrieve the UI-space NvUIRect for this element's focus rectangle. */
    virtual void GetFocusRect(NvUIRect& rect);

    /** Override to handle drawing thumb element over base valuebar. */
    virtual void Draw(const NvUIDrawState &drawState);

    /** Set true to enable posting of reaction during movement of the thumb.

        When disabled/false, we only post reaction when the thumb is released
        in its final position. When true, we update constantly while the thumb
        is being dragged.  The default is false/disabled.
    */
    void SetSmoothScrolling(bool smooth) { m_smooth = smooth; }

    /** Set incremental 'steps' for value changes, like 'tick marks' on a ruler.
        This enforces a given stepping of values along the min/max range.  Most
        helpful when the range is large, but you want specific 'stop' points.
    */
    void SetStepValue(float step) { m_stepValue = step; }

    /** Returns the action code we use when raising reaction to user interaction. */
    uint32_t GetActionCode() { return m_action; }

private:
    /** Call inherited @p SetValue method, update our thumb, and optionally update linked text string. */
    void SetValueReal(float value); // This function does the real work. This is because
                                    // when we are sliding the slider manually, we don't
                                    // want "external" SetValues to influence the position
                                    // of the thumb.
    /** Update the position of the thumb based on current/min/max values. */
    void PositionThumb();

    float m_hitMarginWide; /**< Extra horizontal hit margin for the thumb button. */
    float m_hitMarginTall; /**< Extra vertical hit margin for the thumb button. */
};


//=============================================================================
//=============================================================================
class NvUIPopup;
/** The NvUIPopupMenu is a specialized container class that simulates a popup menu.

    Note that this class itself does minimal work to make the 'menu magic' happen.
    It is really the contents of our container, the 'menu item' elements themselves,
    that control the real functionality and visuals of a given NvUIPopupMenu.
    
    There is no default implementation of menu items or other substructure, aside from
    currently we add items vertically in order (a subclass could implement different
    layout for a specialized menu with known visual structure).  The items should
    generally be NvUIButtons, but could also be static graphics for dividers, or
    static text for other status in the menu.

    @see NvUIPopup
    @see NvUIContainer
*/
class NvUIPopupMenu : public NvUIContainer
{
private:
    INHERIT_FROM(NvUIContainer);
    NvUIPopup *m_myButton; /**< The popup button that owns/shows/hides us. */

public:
    /** Default constructor.
        Basically just sets up our NvUIContainer superclass for us.
        @param btn The NvUIPopup that owns this popup menu.
        @param width The base width of the menu.
        @param height The base height of the menu (noting actual container height changes as items are added...).
        @param bg [optional] Background graphic to display under the menu items themselves.  For a menu, handy to have an NvUIGraphicFrame for the background element.
        @see NvUIPopup
        @see NvUIContainer
    */
    NvUIPopupMenu(NvUIPopup *btn, float width, float height, NvUIGraphic *bg=NULL);
    /** Default destructor */
    virtual ~NvUIPopupMenu();
    /** Override to ensure we take down the popup menu at the right times.

        We first allow the normal inherited container handling to occur.  If nothing in the container handles the event, and the event was a press or a release, we tell our NvUIPopup button to finish the popup.

        If something in the container DID handle the event, and the event was a release, and there was NO reaction to the event, we presume there was a release inside the container but not inside an item, and thus also tell out button to finish the popup.
        @see NvUIContainer
        @see NvUIPopup
        @see PopupFinish
    */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract);

    /** Override to handle events so we stay up until otherwise put away... */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);
};


//======================================================================
//======================================================================
/** The NvUIPopup class encapsulates the text/button that causes the NvUIPopupMenu to appear.
    This is the button object that when pressed causes a menu to pop up in the NvUIWindow in
    which it is contained.
    
    The menu contents are constructed by adding UI elements representing the 'menu items' to the
    popup, one at a time, also passing in a text string/name and a value for each item.  The individual
    NvUIElements each handle what occurs in the system when one is chosen, allowing for extreme
    flexibility in construction and handling of menus. The string:value combinations are used after
    the popup detects a menu item has been selected to appropriately change the popup button's text
    to match the selection.

    @see NvUIPopupMenu
    @see NvUIWindow
    @see NvUIButton
*/
class NvUIPopup : public NvUIButton
{
private:
    INHERIT_FROM(NvUIButton);
    NvUIPopupMenu *m_menu; /**< The popup menu we build and will display when user presses on us. */
    NvUIButton *m_popper; /**< A small indicator button that there's a popup/dropdown menu. */ 
    uint32_t m_itemCount; /**< The total number of menu items in our menu. */
    float m_lineHeight; /**< Our canonical height. */
    float m_nextItemTop; /**< The starting Y offset for the next item added to our menu. */
    uint32_t m_padTop; /**< Top padding for positioning items in the menu's container. */
    uint32_t m_padBottom; /**< Bottom padding for sizing the height of our menu's container. */
    std::string m_titlePre; /**< Prefix string for our button title, for recomposing after new menu item selected. */
    typedef std::map<uint32_t, std::string> ItemValueName; /** Map typedef for storing item value:name pairs. */
    ItemValueName m_itemNames; /** Map item value to a name, for setting title after pick. */

public:
    /** Default constructor for popup object.
        @param actionCode The action code generated when user presses on us, so we know to display/hide popup menu.
        @param visrep Standard array of visuals for button states
        @param popupFrame A graphic frame surrounding the popup menu contents
        @param popper A small button visual that denotes we have a menu for the user
        @param title [optional] Prefix for our button title, prepended to current menu item name.  Defaults NULL.
        @param pt [optional] Point size for font for our title.  Defaults to 0.
        @param shadowed [optional] Whether our title should have drop shadow.  Defaults false.
        */
    NvUIPopup(uint32_t actionCode, NvUIElement *visrep[NvUIButtonState::MAX],
                NvUIGraphicFrame *popupFrame, NvUIButton *popper,
                const char *title=NULL, float pt=0, bool shadowed=false);
    /** Default destructor */
    virtual ~NvUIPopup();

    /** Add an NvUIElement to our popup menu container.
        @param el NvUIElement to add next in order.
        @param name Title for the value this item represents, for showing to user when we're not popped up.
        @param value Value sent through our reaction when this item is clicked in the menu.
    */
    void AddItem(NvUIElement *el, const char *name, uint32_t value);

    /** Call to set which item is active based on value of item (not 'index'). */
    void SetActiveItemValue(uint32_t value);

    /** Override so we can special-case presses.

        We override and call inherited method first.  When it returns, if there was a reaction, and
        the gesture was a @p press, then depending on our state we either show or hide our popup menu.

        We also then clear the reaction flag, as we never want our base button to cause a reaction,
        only the menu items in the popup itself cause reactions.
        @see PopupStart
        @see PopupFinish
        @see NvUIButton
    */
    virtual NvUIEventResponse HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract);
    /** Override to handle successful selection of a menu item.

        When a menu item is selected, we reset our button title to use the selected item's name,
        we finish/hide the popup menu, and we proxy the reaction to our menu so that it can also
        react as needed (highlighting the right item, etc.). Finally, call inherited method when we're all done.
        @see NvUIReaction
        @see NvUIPopupMenu
        @see PopupFinish
        @see NvUIButton
    */
    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);

    /** Override to handle focus events causing the menu to pop up/down. */
    virtual NvUIEventResponse HandleFocusEvent(NvFocusEvent::Enum evt);

    /** Override as we manually Draw our popper visual if we have one. */
    virtual void Draw(const NvUIDrawState &ds);
    /** Override as we manually match our popper visual if we have one to our own draw state. */
    virtual void SetDrawState(uint32_t n);
    /** Override as we need to manually position sub-elements.
        Specifically, we position our popper visual if we have one, override our title position (as we know
        better than our button superclass), and set the popup menu position so it is ready to display.
    */
    virtual void SetOrigin(float x, float y);

    /** Override to handle adjustmetns to the focus rect for the popup. */
    virtual void GetFocusRect(NvUIRect& rect);

    /** Accessor to get at the popup menu itself. */
    NvUIPopupMenu* GetUIPopupMenu() { return m_menu; }

protected:
    /** Display the popup in our window and flag our button as 'pressed'. */
    void PopupStart();
    /** Hide the popup, remove from window, and clear our button back to 'active'. */
    void PopupFinish();

private:
    /** Update our title with the name matching the given value. */
    void UpdateTitleValue(uint32_t value);

    friend class NvUIPopupMenu;
};

#endif
