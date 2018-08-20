//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIVK.cpp
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
#include "../../src/NvUI/NvBitFontInternal.h"
#include "../../src/NvUI/NvUIInternal.h"
#include "NvUIVK.h"
#include "NvVkUtil/NvVkContext.h"
#include "NvUITextureRenderVK.h"

static NvUIDrawCtx s_drawCtx;

//========================================================================
int32_t NvUIGlobalInitVK()
{
	return 0;
}

//========================================================================
void NvUIRenderPrepVK()
{
	NvVkContext& vk = *s_drawCtx.mVk;
	s_drawCtx.mCmd = s_drawCtx.mVk->getMainCommandBuffer(NvVkContext::CMD_UI_FRAMEWORK);
	VkCommandBuffer cmd = s_drawCtx.mCmd;

	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };

	renderPassBeginInfo.renderPass = vk.mainRenderTarget()->copyRenderPass();
	renderPassBeginInfo.framebuffer = vk.mainRenderTarget()->frameBuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = vk.mainRenderTarget()->width();
	renderPassBeginInfo.renderArea.extent.height = vk.mainRenderTarget()->height();

	VkClearValue clearValues[2];
	clearValues[0].color.float32[0] = 0.33f;
	clearValues[0].color.float32[1] = 0.44f;
	clearValues[0].color.float32[2] = 0.66f;
	clearValues[0].color.float32[3] = 1.0f;
	clearValues[1].depthStencil.depth = 1.0f;
	clearValues[1].depthStencil.stencil = 0;

	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.clearValueCount = 2;

	vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

//========================================================================
void NvUIRenderDoneVK()
{
	vkCmdEndRenderPass(s_drawCtx.mCmd);

	s_drawCtx.mVk->submitMainCommandBuffer(NvVkContext::CMD_UI_FRAMEWORK);
}

extern void NvBitfontUseVK(NvVkContext* context);

int32_t doNothing() { return 0; }
void doNothingVoid() { /* */ }

void NvUIUseVK(NvVkContext* context) {
	s_drawCtx.mVk = context;
    NvUIRenderFactory::GlobalInit = &NvUIGlobalInitVK;
    NvUIRenderFactory::GraphicCreate = &NvUIGraphicRenderVK::Create;
    NvUIRenderFactory::GraphicFrameCreate = &NvUIGraphicFrameRenderVK::Create;
    NvUIRenderFactory::TextureRenderCreate = &NvUITextureRenderVK::Create;
    NvUIRenderFactory::GlobalRenderPrep = &NvUIRenderPrepVK;
    NvUIRenderFactory::GlobalRenderDone = &NvUIRenderDoneVK;
    NvUIRenderFactory::GlobalShutdown = &doNothingVoid;
	NvUIRenderFactory::GlobalInit();
    NvBitfontUseVK(context);
}

extern NvUIDrawCtx& NvUIVKctx() {
    return s_drawCtx;
}
