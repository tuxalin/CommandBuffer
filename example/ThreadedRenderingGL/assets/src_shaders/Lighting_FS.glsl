//----------------------------------------------------------------------------------
// File:        gl4-kepler\DeferredShadingMSAA\assets\shaders/Lighting_NoMSAA_FS.glsl
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

layout(location = 0) in vec4 inProjPos;

layout(location = 0) out vec4 outColor;

uniform int uLightingModel;

uniform highp sampler2DRect uNormalDepth;
uniform highp sampler2DRect uDiffuseRoughness;

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
#ifdef USE_POINT_LIGHT
	float u_fLinearAttenuation;
	float u_fQuadraticAttenuation;
#else
	float u_fCausticOffset;
	float u_fCausticTiling;
#endif
};

float g_maxFogDist = 75.0f;
float g_ambientIncrease = 2.0f;
vec3 g_fogColor = vec3(0.0f, 0.0f, 0.64f);

float fogIntensity(float dist)
{
	dist *= 0.8;

	float normalizedDist = clamp((dist / g_maxFogDist), 0.0f, 1.0f);

	// Also use a square distance to maintain some color from the fish vs. the ground
	return normalizedDist * normalizedDist; 
}

void main(void)
{
    vec3 color;

    // Read G-Buffer
    vec4 normalDepth = texelFetch(uNormalDepth, ivec2(gl_FragCoord.xy));
    vec4 diffuseRoughness = texelFetch(uDiffuseRoughness, ivec2(gl_FragCoord.xy));
    vec3 mtlColor = diffuseRoughness.rgb;
	float roughness = diffuseRoughness.a;

    // Get the normal
    vec3 worldNormal = normalDepth.xyz;

    // Calculate position
    float viewLinearDepth = normalDepth.w;
    vec3 viewPos = DepthToPosition(viewLinearDepth, inProjPos, u_mInverseProjMatrix, 100.0f);
    vec3 worldPos = (u_mInverseViewMatrix * vec4(viewPos, 1.0f)).xyz;

    // Calculate to eye vector
    vec3 eyeWorldPos = (u_mInverseViewMatrix * vec4(0, 0, 0, 1)).xyz;
    vec3 toEyeVector = normalize(eyeWorldPos - worldPos);
    float normLength = length(worldNormal);
    if (normLength > 0.0)
    {
        vec3 normal = worldNormal / normLength;
#ifdef USE_POINT_LIGHT
        vec3 dirToLight = worldPos - u_vLightPosition.xyz;
		float distance = length(dirToLight);
		dirToLight = dirToLight / distance;
#else
		// directional light
		vec3 dirToLight = normalize(u_vLightPosition.xyz);
#endif
        vec3 lightAmbient = u_vLightAmbient.rgb;
        vec3 lightColor = u_vLightDiffuse.rgb;
        switch (uLightingModel)
        {
        case 1:
            color = OrenNayar(dirToLight, toEyeVector, normal, lightColor, mtlColor, roughness);
            break;
        case 0:
        default:
            color = LambertDiffuse(dirToLight, normal, lightAmbient, lightColor, mtlColor);
			color += max(0.0, GGX(dirToLight, toEyeVector, normal, roughness, 0.06));
            break;
        }

#ifdef USE_POINT_LIGHT
		float Kc = 1.0;
		float Kl = u_fLinearAttenuation;
		float Kd = u_fQuadraticAttenuation;
		float attenuation = Kc + Kl * distance + Kd * distance * distance;
		attenuation = max(1.0, attenuation);
		color /= attenuation;
#endif

#ifndef USE_POINT_LIGHT
		float fog = fogIntensity(length(viewPos.xyz));
		color.rgb = mix(color.rgb, g_fogColor * u_vLightAmbient.rgb * g_ambientIncrease, fog);
#endif

    }
    else
    {
#ifdef USE_POINT_LIGHT
		color = vec3(0.0);
#else
		// No geometry rendered, so use just the color
        color = mtlColor * u_vLightAmbient.rgb * g_ambientIncrease;
#endif
    }
	 
	outColor = vec4(color, 1.0);
}
