//----------------------------------------------------------------------------------
// File:        NvUI/NvTweakBar.cpp
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

#include "NvUI/NvTweakBar.h"
#include "NvUI/NvBitFont.h" // for inline styling.
#include "NV/NvLogs.h"

#include <stdio.h>
#include <algorithm>

//======================================================================
#ifdef SHOW_PRESSED_BUTTON
#define NVB_FONT_TO_HEIGHT_FACTOR    2.0f
#define NVB_HEIGHT_TO_FONT_FACTOR    0.5f
#else
#define NVB_FONT_TO_HEIGHT_FACTOR    1.5f
#define NVB_HEIGHT_TO_FONT_FACTOR    0.66667f
#endif

// this is the max items in an enum where we use radio group, else we switch to a popup menu.
// normally 3, can reduce to 2 for testing menus easier, or increase higher to prevent auto-popups.
#define ENUM_COUNT_THRESH_FOR_COMPACT_MENU  3
// OR, use this to force menu to always replace enums for testing menu interaction.
#ifdef _DEBUG
static bool debugForceMenu = false;
#else
static bool debugForceMenu = false; // DON'T CHANGE THIS ONE! :)
#endif

// private/internal class decls.
class NvTweakContainer : public NvUIContainer
{
private:
    INHERIT_FROM(NvUIContainer);
public:
    NvTweakContainer(float width, float height, NvUIGraphic *bg=NULL)
        : INHERITED(width, height, bg)
    {
        m_canFocus = true; // !!!!TBD TODO should do custom CanFocus override, which iterates to see if any children can take focus.
        m_canMoveFocus = true;
        m_showFocus = false; // we don't want to show outer focus.
    };
};

template<class T>
class NvTweakSwitchContainer : public NvTweakContainer
{
private:
    INHERIT_FROM(NvTweakContainer);
    T m_baseVal;
	NvTweakVar<T>& m_tvar;
public:
    NvTweakSwitchContainer(float width, NvTweakVar<T> *tvar, T val, NvUIGraphic *bg=NULL)
        : NvTweakContainer(width, 0, bg)
        , m_baseVal(val)
        , m_tvar(*tvar)
    {
        SetVisibility(tvar->equals(val)); 
    };

    virtual NvUIEventResponse HandleReaction(const NvUIReaction& react);
};


template <>
NvUIEventResponse NvTweakSwitchContainer<bool>::HandleReaction(const NvUIReaction& react)
{
	if (react.code && (react.code != m_tvar.getActionCode()))
		return nvuiEventNotHandled; // not a message for us.

	if (react.flags & NvReactFlag::FORCE_UPDATE)
	{
		NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
		change.ival = m_tvar; // update to what's stored in the variable.
	}

	INHERITED::HandleReaction(react);

	NvUIEventResponse r = nvuiEventNotHandled;
	if ((react.uid == GetUID()) // we always listen to our own sent messages.
		|| (react.code && (react.code == m_tvar.getActionCode())) // we always listen to our action code
		|| (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE))) // we listen to force-update only if NO action code
	{
		// bool TweakVar stashed value in state in HandleReaction
		SetVisibility(m_baseVal == (react.state>0));
	}

	return r;
}

template <>
NvUIEventResponse NvTweakSwitchContainer<uint32_t>::HandleReaction(const NvUIReaction& react)
{
	if (react.code && (react.code != m_tvar.getActionCode()))
		return nvuiEventNotHandled; // not a message for us.

	if (react.flags & NvReactFlag::FORCE_UPDATE)
	{
		NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
		change.ival = m_tvar; // update to what's stored in the variable.
	}

	INHERITED::HandleReaction(react);

	NvUIEventResponse r = nvuiEventNotHandled;
	if ((react.uid == GetUID()) // we always listen to our own sent messages.
		|| (react.code && (react.code == m_tvar.getActionCode())) // we always listen to our action code
		|| (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE))) // we listen to force-update only if NO action code
	{
		// uint TweakVar stashed value in ival in HandleReaction
		SetVisibility((m_baseVal == react.ival));
	}

	return r;
}

template <>
NvUIEventResponse NvTweakSwitchContainer<float>::HandleReaction(const NvUIReaction& react)
{
	if (react.code && (react.code != m_tvar.getActionCode()))
		return nvuiEventNotHandled; // not a message for us.

	if (react.flags & NvReactFlag::FORCE_UPDATE)
	{
		NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
		change.ival = m_tvar; // update to what's stored in the variable.
	}

	INHERITED::HandleReaction(react);

	NvUIEventResponse r = nvuiEventNotHandled;
	if ((react.uid == GetUID()) // we always listen to our own sent messages.
		|| (react.code && (react.code == m_tvar.getActionCode())) // we always listen to our action code
		|| (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE))) // we listen to force-update only if NO action code
	{
		// float TweakVar stashed value in fval in HandleReaction
		SetVisibility((m_baseVal == react.fval));
	}

	return r;
}

//=====================================================================
// start of actual tweakbar code.
//=====================================================================

//=====================================================================
//=====================================================================
NvTweakBar *NvTweakBar::CreateTweakBar(NvUIWindow *window)
{
    // !!!!TODO how to set width properly?
    float bw = window->GetWidth() * 0.3f;
    float bh = window->GetHeight();

    NvTweakBar *twb = new NvTweakBar(bw, bh);

    // add it to the passed in window/container.
    window->Add(twb, 0, 0);

    return twb;
}

//=====================================================================
//=====================================================================
NvTweakBar::NvTweakBar(float width, float height)
: NvUIContainer(width, height, new NvUIGraphicFrame("info_text_box_thin.dds", 24, 24))
, m_defaultRows(20) //!!!!TBD TODO just picked a number that seems good atm.
, m_widthPadFactor(0.1f) // !!!!!TBD TODO picking padding that seems decent on L/R sides.
, m_padHeight(0)
, m_textShadows(true)
, m_compactLayout(false)
, m_lastElX(0)
, m_lastElY(0)
, m_xInset(0)
, m_lastActionCode(TWEAKBAR_ACTIONCODE_BASE) // so automatic codes are high up.
, m_subgroupLastY(0)
, m_subgroupMaxY(0)
, m_subgroupContainer(0L)
, m_subgroupSwitchVar(0L)
{
    m_canFocus = true;

    // trying a slightly mid-cyan color for labels.
	m_labelColor = NV_PACKED_COLOR(192, 192, 64, 255);
    // keep value white until we can pass through to buttons and others
    m_valueColor = NV_PACKED_COLOR(255,255,255,255); 

    m_lastElX = 0 + (GetWidth() - GetDefaultLineWidth())/2;
    m_lastElY = GetStartOffY();

    // can't do this in constructor, order of ops not guaranteed somehow...
    m_padHeight = GetDefaultLineHeight()*0.35f;

// !!!TBD Hack to make it a little more see-through for the moment...
//    if (m_background)
//        m_background->SetAlpha(0.75f);

    SetConsumeClicks(true);
    SetFocusHilite(MakeFocusFrame());
}


//======================================================================
//======================================================================
NvTweakBar::~NvTweakBar()
{
    // iterate through the tweak vars and delete them all, since we basically
    // own them internally.
    std::vector<NvTweakVarBase *>::iterator iter = m_tweakVars.begin();
    while (iter != m_tweakVars.end()) {
        NvTweakVarBase *var = (*iter);
        iter = m_tweakVars.erase(iter);
        if (var!=NULL) delete var;
    }
}


//======================================================================
//======================================================================
void NvTweakBar::HandleReshape(float w, float h)
{
    INHERITED::HandleReshape(w, h);

    // what should a tweakbar do to adjust its component layout???
    // !!!!!TBD TODO TODO TODO
    // for now, just resize the background HEIGHT...
    m_background->SetHeight(h);
}

//======================================================================
//======================================================================
NvUIEventResponse NvTweakBar::HandleReaction(const NvUIReaction &react)
{
    if (react.code==TWEAKBAR_ACTIONCODE_BASE)
    {
        SetVisibility((react.state>0));
        //return nvuiEventHandled;
    }
    return INHERITED::HandleReaction(react);
}


//======================================================================
//======================================================================
void NvTweakBar::Draw(const NvUIDrawState &drawState)
{
    INHERITED::Draw(drawState);
}


//======================================================================
//======================================================================
void NvTweakBar::syncValues()
{
    NvUIReaction &react = GetReactionEdit(true);
    react.code = 0; // match all.
    react.flags = NvReactFlag::FORCE_UPDATE;
    HandleReaction(react);
}


//======================================================================
//======================================================================
void NvTweakBar::syncValue(NvTweakVarBase* var)
{
    NvUIReaction &react = GetReactionEdit(true);
    react.code = var->getActionCode();
    react.flags = NvReactFlag::FORCE_UPDATE;
    HandleReaction(react);
}


//======================================================================
//======================================================================
NvUIGraphic *NvTweakBar::MakeFocusFrame()
{
    NvUIGraphicFrame *frame = new NvUIGraphicFrame("rounding.dds", 4);
    frame->SetDrawCenter(false);
//    frame->SetAlpha(0.5f);
    frame->SetColor(NV_PACKED_COLOR(0x60, 0xFF, 0xFF, 0xFF));
    return frame;
}


//======================================================================
//======================================================================
NvUIPopup *NvTweakBar::MakeStdPopup(const char* name, NvTweakEnumVar<uint32_t> &refvar, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode/*==0*/)
{
    uint32_t code = actionCode==0?++m_lastActionCode:actionCode; // need to stash code so all radios use the same
    float wide = GetDefaultLineWidth();
    float high = GetDefaultLineHeight();
    float fontFactor = NVB_HEIGHT_TO_FONT_FACTOR;    

    NvUIButton *btn = NULL;  
    NvUIGraphicFrame *frame;
    NvUIElement *els[3];

    els[2] = NULL;

    els[0] = new NvUIGraphic("arrow_blue_down.dds");
    els[0]->SetDimensions(high/2, high/2);
    els[1] = new NvUIGraphic("arrow_pressed_down.dds");
    els[1]->SetDimensions(high/2, high/2);
    btn = new NvUIButton(NvUIButtonType::CHECK, code, els);

    // first, the button styling
    frame = new NvUIGraphicFrame("frame_thin.dds", 10);
    frame->SetAlpha(1.5f);
    frame->SetDimensions(wide, high);
    els[0] = frame;
    frame = new NvUIGraphicFrame("frame_thin.dds", 10);
    frame->SetAlpha(1.8f);
    frame->SetDimensions(wide, high);
    frame->SetColor(NV_PACKED_COLOR(0xFF, 0xE0, 0x50, 0xFF)); // yellow
    els[1] = frame;

    // then, the popup menu framing
    frame = new NvUIGraphicFrame("frame_thin.dds", 10);
    frame->SetAlpha(1.8f);

    NvUIPopup *dd = new NvUIPopup(code, els, frame, btn, name, high*fontFactor, true);
    for (uint32_t i=0; i<valueCount; i++)
    {
        frame = new NvUIGraphicFrame("icon_button_highlight_small.dds", 8);
        frame->SetDimensions(wide, high);
        frame->SetAlpha(0);
        els[0] = frame;
        frame = new NvUIGraphicFrame("icon_button_highlight_small.dds", 8);
        frame->SetDimensions(wide, high);
        frame->SetAlpha(0.5f);
        frame->SetColor(NV_PACKED_COLOR(0xFF, 0xE0, 0x50, 0xFF)); // yellow
        els[1] = frame;

        btn = new NvUIButton(NvUIButtonType::PUSH, code, els, values[i].m_name, (high*fontFactor), true);
        btn->SetSubCode(i); // !!!!TBD should this be set inside AddItem??
        btn->SetHitMargin(high*0.05f, high*0.05f);
        btn->SetSlideInteractGroup(code);
        //btn->SetDrawState(0);
        //btn->SetDimensions(wide, high); // reset for check/radio buttons...

        NvTweakEnumUI<uint32_t> *twui = new NvTweakEnumUI<uint32_t>(refvar, i, btn, code);
        dd->AddItem(twui, values[i].m_name, values[i].m_value);
    }

    // refvar will convert to uint32 value for passing to SetItemValueActive
    dd->SetActiveItemValue(refvar);

    // add focus frame to the popup menu
    NvUIPopupMenu* mnu = dd->GetUIPopupMenu();
#if later
    frame = new NvUIGraphicFrame("frame_thin.dds", 10);
    frame->SetAlpha(0.5f);
    frame->SetColor(NV_PACKED_COLOR(0xFF, 0xE0, 0x50, 0xFF)); // yellow
#endif
    mnu->SetFocusHilite(MakeFocusFrame());

    return dd;
}

//======================================================================
//======================================================================
NvUIButton *NvTweakBar::MakeStdButton(const char* name, bool val, NvUIButtonType::Enum btntype/*==CHECK*/, uint32_t code/*==0*/, uint32_t subcode/*==0*/)
{
    uint32_t actionCode = code==0?++m_lastActionCode:code;
    NvUIButton *btn = NULL;  
    float wide = GetDefaultLineWidth();
    float high = GetDefaultLineHeight();
    float fontFactor = NVB_HEIGHT_TO_FONT_FACTOR;    
    NvUIElement *els[3];

    if (btntype==NvUIButtonType::PUSH)
    {
        els[0] = new NvUIGraphicFrame("btn_round_blue.dds", 18, 18);
        els[0]->SetDimensions(wide, high);
        els[1] = new NvUIGraphicFrame("btn_round_pressed.dds", 18, 18);
        els[1]->SetDimensions(wide, high);
        els[2] = NULL;
    }
    else
    if (btntype==NvUIButtonType::CHECK)
    {
        els[0] = new NvUIGraphic("btn_box_blue.dds");
        els[0]->SetDimensions(high, high);
        els[1] = new NvUIGraphic("btn_box_pressed_x.dds");
        els[1]->SetDimensions(high, high);
        els[2] = NULL;
    }
    else // radio
    {
        els[0] = new NvUIGraphicFrame("button_top_row.dds", 24);
        els[0]->SetDimensions(high*0.8f, high);
        els[1] = new NvUIGraphicFrame("button_top_row_pressed.dds", 24);
        els[1]->SetDimensions(high*0.8f, high);
        els[2] = NULL;
    }

    btn = new NvUIButton(btntype, actionCode, els, name, (high*fontFactor), m_textShadows);
    btn->SetSubCode(subcode);
    btn->SetHitMargin(high*0.05f, high*0.05f);
    btn->SetDrawState(val);
    btn->SetDimensions(wide, high); // reset for check/radio buttons...
        
    return btn;
}

NvUISlider *NvTweakBar::MakeStdSlider(const char* name, float val, float min, float max, float step, bool integral, uint32_t code/*==0*/)
{
    uint32_t actionCode = code==0?++m_lastActionCode:code;
    float wide = GetDefaultLineWidth();
    float high = GetDefaultLineHeight();

    // !!!TBD TODO 
    // text and output value update should be embedded in the slider itself...
  
    // this logic should all move into valuebar.
/* !!!!TBD
    if (m_compactLayout)
    {
        out->SetDimensions(wide*0.2f, high);
        text->SetDimensions(wide*0.3f, high);
    }
*/

    NvUIGraphicFrame *emptybar = new NvUIGraphicFrame("slider_empty.dds", 8);
    NvUIGraphicFrame *fullbar = new NvUIGraphicFrame("slider_full.dds", 8);
    fullbar->SetAlpha(0.6f); // make the fill a bit see-through.
	fullbar->SetColor(NV_PACKED_COLOR(0xFF, 0xFF, 0xFF, 0xFF)); // white

    NvUIGraphic *thumb = new NvUIGraphic("slider_thumb.dds");
    thumb->SetDimensions(high*0.5f, high*1.25f);

    NvUISlider *sld = new NvUISlider(emptybar, fullbar, thumb, actionCode);
    sld->SetSmoothScrolling(true);
    sld->SetIntegral(integral);
    sld->SetMaxValue(max);
    sld->SetMinValue(min);
    sld->SetStepValue(step);
    sld->SetValue(val);
    sld->SetDimensions(wide, high/4);
    sld->SetHitMargin(high/4, high/4); // !!!!TBD TODO

    return sld;
}


void NvTweakBar::AddElement(NvUIElement *te, bool autoSpace/*=true*/)
{
    NvUIRect mr;
    te->GetScreenRect(mr);

    if (m_subgroupContainer)
    {
        m_subgroupContainer->Add(te, m_padHeight, m_subgroupLastY);
        if (autoSpace)
            m_subgroupLastY += mr.height;
    }
    else
    {
        INHERITED::Add(te, m_lastElX+m_xInset, m_lastElY);
        if (autoSpace)
            m_lastElY += mr.height;
    }
}

void NvTweakBar::subgroupStart()
{
    m_xInset += GetDefaultLineHeight();
}

void NvTweakBar::subgroupEnd()
{
    m_xInset -= GetDefaultLineHeight();
}

// maybe tweakVar should have a factory that creates the
// proper TweakSwitchContainer<T> based on its current type?
void NvTweakBar::subgroupSwitchStart(NvTweakVarBase *var)
{
    m_subgroupSwitchVar = var;
    m_subgroupMaxY = 0;
    // TBD background !!!!!TBD TODO

    // this tells us:
    // 1) what container to add to
    // 2) what the starting top-left is of the NEXT switch container
    // ... but still need to track the HEIGHT max of all switches
}

void NvTweakBar::subgroupSwitchCase(NvUIContainer *c)
{
    subgroupClose();
    m_subgroupContainer = c;
    NvUIGraphicFrame *bg = new NvUIGraphicFrame("frame_thin.dds", 4);
    bg->SetAlpha(0.2f);
    m_subgroupContainer->SetBackground(bg);
    m_subgroupContainer->SetFocusHilite(MakeFocusFrame());
    INHERITED::Add(m_subgroupContainer, m_lastElX+m_xInset-m_padHeight, m_lastElY);
}

void NvTweakBar::subgroupSwitchCase(bool val)
{
	// risky down-cast; up to this caller to ensure that <T> is correct
    subgroupSwitchCase(new NvTweakSwitchContainer<bool>(GetDefaultLineWidth()+(2*m_padHeight), (NvTweakVar<bool>*)m_subgroupSwitchVar, val));
}

void NvTweakBar::subgroupSwitchCase(float val)
{
	// risky down-cast; up to this caller to ensure that <T> is correct
	subgroupSwitchCase(new NvTweakSwitchContainer<float>(GetDefaultLineWidth()+(2*m_padHeight), (NvTweakVar<float>*)m_subgroupSwitchVar, val));
}

void NvTweakBar::subgroupSwitchCase(uint32_t val)
{
	// risky down-cast; up to this caller to ensure that <T> is correct
	subgroupSwitchCase(new NvTweakSwitchContainer<uint32_t>(GetDefaultLineWidth()+(2*m_padHeight), (NvTweakVar<uint32_t>*)m_subgroupSwitchVar, val));
}

void NvTweakBar::subgroupClose()
{
    if (m_subgroupContainer!=NULL) // do we have a case already?
    {
        m_subgroupLastY += m_padHeight;
        m_subgroupContainer->SetHeight(m_subgroupLastY);
        if (m_subgroupMaxY < m_subgroupLastY)
            m_subgroupMaxY = m_subgroupLastY;
    }
    m_subgroupLastY = m_padHeight;
    m_subgroupContainer = NULL;
}

void NvTweakBar::subgroupSwitchEnd()
{
    subgroupClose();

    // bump lastElY by max subgroup height...
    m_lastElY += m_subgroupMaxY;

    // null out any control variables.
    m_subgroupSwitchVar = NULL;
}

void NvTweakBar::addPadding(float multiplier/*=1*/)
{
    if (m_subgroupContainer)
        m_subgroupLastY += multiplier*m_padHeight;
    else
        m_lastElY += multiplier*m_padHeight;
}

void NvTweakBar::addLabel(const char *title, bool bold/*==false*/)
{
    float dpsize = GetDefaultLineHeight()*NVB_HEIGHT_TO_FONT_FACTOR;//*(small?0.85f:1.0f);
    std::string s = bold?NVBF_STYLESTR_BOLD:"";
    s.append(title);
    NvUIText *text = new NvUIText(s.c_str(), NvUIFontFamily::DEFAULT, dpsize, NvUITextAlign::LEFT);
    text->SetColor(m_labelColor);
    if (m_textShadows) text->SetShadow();
    AddElement(text);
    // bottom padding
//    m_lastElY += (GetDefaultLineHeight()-text->GetHeight()); //*(small?0.5f:1.0f); // !!!TBD padding difference
}

NvTweakVarBase* NvTweakBar::addValueReadout(const char* name, float &var, float max, uint32_t code/*=0*/)
{
    const float wide = GetDefaultLineWidth();
    const float high = GetDefaultLineHeight();
    uint32_t actionCode = (code==0) ? ++m_lastActionCode : code;
    NvTweakVar<float> *tvar = new NvTweakVar<float>(var, name);
    trackTweakVar(tvar);
    tvar->setActionCode(actionCode);
    NvUIValueText *vtxt = new NvUIValueText(name, NvUIFontFamily::DEFAULT, (high*NVB_HEIGHT_TO_FONT_FACTOR), NvUITextAlign::LEFT,
                                            var, (max>100 ? 1 : (max>10 ? 2 : 3)), NvUITextAlign::RIGHT, actionCode);
    vtxt->SetWidth(wide); // override default string width...
	vtxt->SetColor(NV_PACKED_COLOR(255, 255, 255, 255));
    if (m_textShadows) vtxt->SetShadow();
    NvTweakVarUIProxyBase *te = new NvTweakVarUI<float>(*tvar, vtxt, actionCode);
    te->SetReadOnly(true);
    AddElement(te);
//    AddElement(vtxt, !m_compactLayout);
    return tvar;
}

NvTweakVarBase* NvTweakBar::addValueReadout(const char* name, uint32_t &var, uint32_t code/*=0*/)
{
    const float wide = GetDefaultLineWidth();
    const float high = GetDefaultLineHeight();
    uint32_t actionCode = (code==0) ? ++m_lastActionCode : code;
    NvTweakVar<uint32_t> *tvar = new NvTweakVar<uint32_t>(var, name);
    trackTweakVar(tvar);
    tvar->setActionCode(actionCode);
    NvUIValueText *vtxt = new NvUIValueText(name, NvUIFontFamily::DEFAULT, (high*NVB_HEIGHT_TO_FONT_FACTOR), NvUITextAlign::LEFT,
                                            (int)var, NvUITextAlign::RIGHT, actionCode);
    vtxt->SetWidth(wide); // override default string width...
	vtxt->SetColor(NV_PACKED_COLOR(255, 255, 255, 255));
    if (m_textShadows) vtxt->SetShadow();
    NvTweakVarUIProxyBase *te = new NvTweakVarUI<uint32_t>(*tvar, vtxt, actionCode);
    te->SetReadOnly(true);
    AddElement(te);
    return tvar;
}


NvTweakVarBase* NvTweakBar::addValueReadout(NvTweakVar<float>* tvar, float max)
{
    const float wide = GetDefaultLineWidth();
    const float high = GetDefaultLineHeight();
    NvUIValueText *vtxt = new NvUIValueText(tvar->getName(), NvUIFontFamily::DEFAULT, (high*NVB_HEIGHT_TO_FONT_FACTOR), NvUITextAlign::LEFT,
                                            tvar->getValue(), ((max>100) ? 1 : ((max>10) ? 2 : 3)), NvUITextAlign::RIGHT, tvar->getActionCode());
    vtxt->SetWidth(wide); // override default string width...
	vtxt->SetColor(NV_PACKED_COLOR(255, 255, 255, 255));
    if (m_textShadows) vtxt->SetShadow();
    // put value color back to white if based on tvar as that's a slider normally.
    vtxt->SetValueColor(NV_PC_PREDEF_WHITE);
    NvTweakVarUIProxyBase *te = new NvTweakVarUI<float>(*tvar, vtxt, tvar->getActionCode());
    te->SetReadOnly(true);
    AddElement(te);
    return tvar;
}


NvTweakVarBase* NvTweakBar::addValueReadout(NvTweakVar<uint32_t>* tvar)
{
    const float wide = GetDefaultLineWidth();
    const float high = GetDefaultLineHeight();
    NvUIValueText *vtxt = new NvUIValueText(tvar->getName(), NvUIFontFamily::DEFAULT, (high*NVB_HEIGHT_TO_FONT_FACTOR), NvUITextAlign::LEFT,
                                            tvar->getValue(), NvUITextAlign::RIGHT, tvar->getActionCode());
    vtxt->SetWidth(wide); // override default string width...
	vtxt->SetColor(NV_PACKED_COLOR(255, 255, 255, 255));
    if (m_textShadows) vtxt->SetShadow();
    // put value color back to white if based on tvar as that's a slider normally.
    vtxt->SetValueColor(NV_PC_PREDEF_WHITE);
    NvTweakVarUIProxyBase *te = new NvTweakVarUI<uint32_t>(*tvar, vtxt, tvar->getActionCode());
    te->SetReadOnly(true);
    AddElement(te);
    return tvar;
}


NvTweakVarBase* NvTweakBar::addValue(const char *name, bool &var, uint32_t actionCode/*==0*/, NvUIButton** buttonOut/*==NULL*/)
{
    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakVar<bool> *tvar = new NvTweakVar<bool>(var, name);
    trackTweakVar(tvar);
    NvUIButton *btn = MakeStdButton(name, var, NvUIButtonType::CHECK, actionCode, 1);
    AddElement(new NvTweakVarUI<bool>(*tvar, btn, btn->GetActionCode()));
	if (buttonOut)
		*buttonOut = btn;
    return tvar;
}

NvTweakVarBase* NvTweakBar::addButton(const char* name, uint32_t actionCode)
{
    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakVar<bool> *tvar = new NvTweakVar<bool>(name); // make self-referencing by not passing a value.
    trackTweakVar(tvar);
    NvUIButton *btn = MakeStdButton(name, false, NvUIButtonType::PUSH, actionCode, 1);
    AddElement(new NvTweakVarUI<bool>(*tvar, btn, btn->GetActionCode()));
    return tvar;
}

NvTweakVarBase* NvTweakBar::addValue(const char *name, float &var, float min, float max, float step/*==0*/, uint32_t actionCode/*==0*/)
{
    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakVar<float> *tvar = new NvTweakVar<float>(var, name, min, max, step);
    trackTweakVar(tvar);
    NvUISlider *sld = MakeStdSlider(name, var, min, max, step, false, actionCode);
    tvar->setActionCode(sld->GetActionCode());
    NvUIElement *te = new NvTweakVarUI<float>(*tvar, sld, sld->GetActionCode());

    addValueReadout(tvar, max);
    addPadding();
    AddElement(te);
    if (m_compactLayout)
    {
        NvUIRect tr;
        te->GetScreenRect(tr);
        te->SetOrigin(tr.left + te->GetWidth()*0.45f, tr.top-(0.4f*te->GetHeight()));
        te->SetDimensions(te->GetWidth()*0.35f, te->GetHeight());
    }
    addPadding();
    return tvar;
}

NvTweakVarBase* NvTweakBar::addValue(const char *name, uint32_t &var, uint32_t min, uint32_t max, uint32_t step/*==0*/, uint32_t actionCode/*==0*/, NvUISlider** pSliderOut /*= NULL*/, NvTweakVar<uint32_t>** pVarOut /* = NULL*/)
{
    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakVar<uint32_t> *tvar = new NvTweakVar<uint32_t>(var, name, min, max, step);
    trackTweakVar(tvar);
    NvUISlider *sld = MakeStdSlider(name, (float)var, (float)min, (float)max, (float)step, true, actionCode);
    tvar->setActionCode(sld->GetActionCode()); // link the var with the code...
    NvUIElement *te = new NvTweakVarUI<uint32_t>(*tvar, sld, sld->GetActionCode());

    addValueReadout(tvar);
    addPadding();
    AddElement(te);
    if (m_compactLayout)
    {
        NvUIRect tr;
        te->GetScreenRect(tr);
        te->SetOrigin(tr.left + te->GetWidth()*0.45f, tr.top-(0.4f*te->GetHeight()));
        te->SetDimensions(te->GetWidth()*0.35f, te->GetHeight());
    }
    addPadding();

	if (pSliderOut)
		*pSliderOut = sld;
	if (pVarOut)
		*pVarOut = tvar;
    return tvar;
}


NvTweakVarBase* NvTweakBar::addEnum(const char *name, uint32_t &var, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode/*==0*/)
{
    if (debugForceMenu || (m_compactLayout && valueCount > ENUM_COUNT_THRESH_FOR_COMPACT_MENU)) {
        return addMenu(name, var, values, valueCount, actionCode);
    }

    uint32_t minval = 0xFFFFFFFF, maxval = 0;
    for (uint32_t i=0; i<valueCount; i++)
    {
        if (minval>values[i].m_value)
            minval = values[i].m_value;
        if (maxval<values[i].m_value)
            maxval = values[i].m_value;
    }

    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakEnumVar<uint32_t> *tvar = new NvTweakEnumVar<uint32_t>(values, valueCount, var, name, minval, maxval, 0);
    trackTweakVar(tvar);
    tvar->setValLoop(true);

    // add label
    addLabel(name, false);

    // for now, inset AFTER the label.
    subgroupStart();

    // add group, currently as radiobuttons, could do as popup in future.
    uint32_t code = actionCode==0?++m_lastActionCode:actionCode; // need to stash code so all radios use the same
    for (uint32_t i=0; i<valueCount; i++)
    {
        NvUIButton *btn = MakeStdButton(values[i].m_name, (values[i]==var), NvUIButtonType::RADIO, code, i);
        AddElement(new NvTweakEnumUI<uint32_t>(*tvar, i, btn, btn->GetActionCode()));
    }

    subgroupEnd();

    return tvar;
}


NvTweakVarBase* NvTweakBar::addMenu(const char *name, uint32_t &var, const NvTweakEnum<uint32_t> values[], uint32_t valueCount, uint32_t actionCode/*==0*/)
{
    uint32_t minval = 0xFFFFFFFF, maxval = 0;
    for (uint32_t i=0; i<valueCount; i++)
    {
        if (minval>values[i].m_value)
            minval = values[i].m_value;
        if (maxval<values[i].m_value)
            maxval = values[i].m_value;
    }

    // make a NvTweakVar on the fly. this will leak currently.
    NvTweakEnumVar<uint32_t> *tvar = new NvTweakEnumVar<uint32_t>(values, valueCount, var, name, minval, maxval, 0);
    trackTweakVar(tvar);
    tvar->setValLoop(true);

    NvUIPopup *el = MakeStdPopup(name, *tvar, values, valueCount, actionCode);
    AddElement(el);
    el->SetParent(this); // point to our container so we can pop-up.

    return tvar;
}


void NvTweakBar::trackTweakVar(NvTweakVarBase *var)
{
    m_tweakVars.push_back(var);
}


void NvTweakBar::resetValues()
{
    // iterate through the tweak vars and reset them all.
    std::vector<NvTweakVarBase *>::const_iterator iter = m_tweakVars.begin();
    while (iter != m_tweakVars.end()) {
        NvTweakVarBase *var = (*iter);
        iter++;
        if (var!=NULL) var->reset();
    }

    // then force a sync.
    syncValues();
}


//=====================================================================
//=====================================================================
// NvTweakVarUI overrides HandleReaction, so we set our TweakVar appropriately
// when our proxied UI has 'triggered' a reaction.

template <>
NvUIEventResponse NvTweakVarUI<bool>::HandleReaction(const NvUIReaction& react)
{
    if (react.code && (react.code!=m_tvar.getActionCode()))
        return nvuiEventNotHandled; // not a message for us.

    if (react.flags & NvReactFlag::FORCE_UPDATE)
    {
        NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
        change.state = (m_tvar ? 1 : 0); // update to what's stored in the variable.
    }

    INHERITED::HandleReaction(react);

    NvUIEventResponse r = nvuiEventNotHandled;
    if (   (react.uid==GetUID()) // we always listen to our own sent messages.
        || (react.code && (react.code==m_tvar.getActionCode())) // we always listen to our action code
        || (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE)) ) // we listen to force-update only if NO action code
    {
        if (m_tvar != (react.state>0))
        {
            m_tvar = (react.state>0); // bool TweakVar stashed value in state in HandleReaction
            return nvuiEventHandled; // !!!!TBD TODO do we eat it here?
        }
    }

    return r;
}

template <>
NvUIEventResponse NvTweakVarUI<uint32_t>::HandleReaction(const NvUIReaction& react)
{
    if (react.code && (react.code!=m_tvar.getActionCode()))
        return nvuiEventNotHandled; // not a message for us.

    if (react.flags & NvReactFlag::FORCE_UPDATE)
    {
        NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
        change.ival = m_tvar; // update to what's stored in the variable.
    }

    INHERITED::HandleReaction(react);

    NvUIEventResponse r = nvuiEventNotHandled;
    if (   (react.uid==GetUID()) // we always listen to our own sent messages.
        || (react.code && (react.code==m_tvar.getActionCode())) // we always listen to our action code
        || (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE)) ) // we listen to force-update only if NO action code
    {
        if (m_tvar != react.ival)
        {
            m_tvar = react.ival; // uint32_t TweakVar stashed value in ival in HandleReaction
            return nvuiEventHandled; // !!!!TBD TODO do we eat it here?
        }
    }

    return r;
}

template <>
NvUIEventResponse NvTweakVarUI<float>::HandleReaction(const NvUIReaction& react)
{
    if (react.code && (react.code!=m_tvar.getActionCode()))
        return nvuiEventNotHandled; // not a message for us.

    if (react.flags & NvReactFlag::FORCE_UPDATE)
    {
        NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!
        change.fval = m_tvar; // update to what's stored in the variable.
    }

    INHERITED::HandleReaction(react);

    NvUIEventResponse r = nvuiEventNotHandled;
    if (   (react.uid==GetUID()) // we always listen to our own sent messages.
        || (react.code && (react.code==m_tvar.getActionCode())) // we always listen to our action code
        || (!react.code && (react.flags & NvReactFlag::FORCE_UPDATE)) ) // we listen to force-update only if NO action code
    {
        if (m_tvar != react.fval)
        {
            m_tvar = react.fval; // float TweakVar stashed value in fval in HandleReaction
            return nvuiEventHandled; // !!!!TBD TODO do we eat it here?
        }
    }

    return r;
}


//=====================================================================
//=====================================================================
// NvTweakEnumUI overrides certain handlers so that IF there is a reaction by the proxy,
// we override the normal value/state with our cached enum value, putting it
// into the appropriate place in the reaction for HandleReaction to pick it up.
template <>
NvUIEventResponse NvTweakEnumUI<bool>::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, hasInteract);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        // copy to state AND ival, as enums might need in ival.
        react.state = (m_enumval ? 1 : 0);
        react.ival = react.state;
    }
    return r;
}

template <>
NvUIEventResponse NvTweakEnumUI<uint32_t>::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, hasInteract);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        react.ival = m_enumval;
    }
    return r;
}

template <>
NvUIEventResponse NvTweakEnumUI<float>::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, hasInteract);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        react.fval = m_enumval;
    }
    return r;
}


template <>
NvUIEventResponse NvTweakEnumUI<bool>::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    NvUIEventResponse r = INHERITED::HandleFocusEvent(evt);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        // copy to state AND ival, as enums might need in ival.
        react.state = (m_enumval ? 1 : 0);
        react.ival = react.state;
    }
    return r;
}

template <>
NvUIEventResponse NvTweakEnumUI<uint32_t>::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    NvUIEventResponse r = INHERITED::HandleFocusEvent(evt);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        react.ival = m_enumval;
    }
    return r;
}

template <>
NvUIEventResponse NvTweakEnumUI<float>::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    NvUIEventResponse r = INHERITED::HandleFocusEvent(evt);
    if (r&nvuiEventHadReaction)
    {
        NvUIReaction &react = GetReactionEdit(false); // false to not clear it!!!
        react.fval = m_enumval;
    }
    return r;
}


// TweakEnumUI overrides handlereaction to ensure that proxied elements have their state
// triggered to 0 or 1 depending on whether the tweakvar bound is same value as our
// cached value.
template <>
NvUIEventResponse NvTweakEnumUI<uint32_t>::HandleReaction(const NvUIReaction& react)
{
    NvUIReaction &change = GetReactionEdit(false); // false to not clear it!!!        
    NvUIEventResponse r = nvuiEventNotHandled;

    if (!(change.flags & NvReactFlag::FORCE_UPDATE)) // if not forced update
        if (react.code && (react.code!=m_tvar.getActionCode())) // if not a message for us
            return r; // we don't do anything.
    
    // if it's a system|forced update, we assume value changed but button/proxy state needs update.
    if (change.flags & NvReactFlag::FORCE_UPDATE)
    {
        if (m_tvar != m_enumval) 
        { // if tweakvar value is not our enum, set state to 0/inactive
            change.state = 0;
        }
        else 
        { // tweakvar value matches our enum, so set state to 1/active
            change.state = 1;
            // in case someone wants it, fill in our value since we matched the tweakvar value..
            change.ival = m_enumval;
        }
    }
    else if (change.flags & NvReactFlag::CLEAR_STATE)
    {
        change.state = 0; // clear drawstate of all els who match our actioncode.
    }
    // else we leave reaction alone.

    // if we sent ourselves the reaction, we need to let NvTweakVarUI handle...
    if (change.uid==GetUID())
    {
        INHERITED::HandleReaction(change);
    }
    else
    {
        // if we didn't handle self-reactions, then here we need to skip over
        // what NvTweakVarUI would do, go right up to proxybase instead.
        // !!> double INHERITED <!!
        INHERITED::INHERITED::HandleReaction(change);
    }
    return r;
}
