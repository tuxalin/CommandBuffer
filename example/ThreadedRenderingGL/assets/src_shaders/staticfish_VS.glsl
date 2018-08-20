//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL\assets\src_shaders/staticfish_VS.glsl
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
#version 310 es
#extension GL_ANDROID_extension_pack_es31a : enable

precision highp float;

// INPUT
layout(location = 0) in vec3 a_vPosition; // In BaseShader
layout(location = 1) in vec3 a_vNormal;   // In BaseProjNormalShader
layout(location = 2) in vec2 a_vTexCoord; // In SceneColorShader

// Instancing Input
layout(location = 7) in vec3 a_vInstancePos;
layout(location = 8) in vec3 a_vInstanceHeading;
layout(location = 9) in float a_fInstanceAnimTime;
layout(location = 10) in uint a_iSchoolId;

// OUTPUT
out vec2 v_vTexcoord;
out vec3 v_vLightIntensity;
out vec4 v_vPosEyeSpace;
out vec4 v_vPosWorldSpace;
out vec3 v_vNormalWorldSpace;

// UNIFORMS
layout(binding=0) uniform ProjBlock {
	mat4 u_mProjectionMatrix;
	mat4 u_mInverseProjMatrix;
	mat4 u_mViewMatrix;
	mat4 u_mInverseViewMatrix;
};

layout(binding=1) uniform LightingBlock {
	vec4 u_vLightPosition;  
	vec4 u_vLightAmbient;   
	vec4 u_vLightDiffuse;   
	float u_fCausticOffset;
	float u_fCausticTiling;
};

layout(binding=2) uniform ModelBlock {
	mat4 u_mModelMatrix;
	float u_fTailStart;
};

void main()
{
    vec3 basisZ = -a_vInstanceHeading;
    vec3 basisX = normalize(cross(vec3(0.0f, 1.0f, 0.0f), basisZ));
    vec3 basisY = normalize(cross(basisZ, basisX));

    mat4 instanceXfm = mat4(vec4(basisX, 0.0f), vec4(basisY, 0.0f), vec4(basisZ, 0.0f), vec4(a_vInstancePos, 1.0f));
	mat4 modelXfm = u_mModelMatrix;
	vec4 vPosModelSpace = modelXfm * vec4(a_vPosition.xyz, 1.0);

	// Determine if this vertex is on the tail
	float tailFactor = vPosModelSpace.z - u_fTailStart;
	if (tailFactor < 0.0f)
	{
		tailFactor *= 0.05f;
	}
	float displacement = sin(a_fInstanceAnimTime) * tailFactor;
	vPosModelSpace.x += displacement;
    mat4 worldXfm = instanceXfm;
    v_vPosWorldSpace = worldXfm * vPosModelSpace;
    v_vPosEyeSpace = u_mViewMatrix * vec4(v_vPosWorldSpace.xyz, 1.0);

    gl_Position = u_mProjectionMatrix * v_vPosEyeSpace;

    v_vNormalWorldSpace = normalize(mat3(worldXfm) * mat3(modelXfm) * a_vNormal);

    vec3 lDir;
    lDir = normalize(u_vLightPosition.xyz);
    v_vLightIntensity = u_vLightAmbient.rgb;

    float NdotL = max(dot(v_vNormalWorldSpace, lDir), 0.0);
    if (NdotL > 0.0) 
    {
        // cosine (dot product) with the normal
        v_vLightIntensity += u_vLightDiffuse.rgb * NdotL;
    }
    v_vTexcoord = a_vTexCoord;
}
