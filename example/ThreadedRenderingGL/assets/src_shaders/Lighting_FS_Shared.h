//----------------------------------------------------------------------------------
// File:        gl4-kepler\DeferredShadingMSAA\assets\shaders/Lighting_FS_Shared.h
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
precision highp float;

vec3 LambertDiffuse(vec3 dirToLight, vec3 surfaceNormal, vec3 ambientColor, vec3 lightColor, vec3 albedo)
{
    float diffuseAmount = clamp(dot(dirToLight, surfaceNormal), 0.0, 1.0);
    return (vec3(diffuseAmount) + ambientColor) * lightColor * albedo;
}


float GGX(vec3 L, vec3 V, vec3 N, float roughness, float F0)
{
	float roughSqr = roughness * roughness;
	vec3 H = normalize(L - V);
	float dotLH = max(0.0, dot(L, H));
	float dotNH = max(0.0, dot(N, H));
	float dotNL = max(0.0, dot(N, L));
	float alphaSqr = roughSqr * roughSqr;
	float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
	float D = alphaSqr / (3.141592653589793 * denom * denom);
	float F = F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
	float k = 0.5 * roughSqr;
	float k2 = k * k;
	return dotNL * D * F / (dotLH*dotLH*(1.0 - k2) + k2);
}

vec3 OrenNayar(vec3 dirToLight, vec3 dirToEye, vec3 surfaceNormal, vec3 lightColor, vec3 albedo, float roughness)
{
    float LDotN = dot(dirToLight, surfaceNormal);
    float VDotN = dot(dirToEye, surfaceNormal);

    float thetaI = acos(abs(LDotN));
    float thetaR = acos(abs(VDotN));

    float alpha, beta;
    if (thetaI > thetaR)
    {
        alpha = thetaI; // x
        beta = thetaR;
    }
    else
    {
        alpha = thetaR;
        beta = thetaI;
    }

    vec3 projLight = normalize(dirToLight - (surfaceNormal * clamp(dot(dirToLight, surfaceNormal), 0.0, 1.0)));  // dirToLight
    vec3 projEye = normalize(dirToEye - (surfaceNormal * clamp(dot(dirToEye, surfaceNormal), 0.0, 1.0)));        // approaches ||0,0,0|| , 

    float gamma = dot(projEye, projLight);

    float alphaSqr = 1.0 - roughness * roughness;

    float A = 1.0 - (0.5 * (alphaSqr / (alphaSqr + 0.57)));
    float B = 0.45 * (alphaSqr / (alphaSqr + 0.09));
    float C = clamp(sin(alpha), 0.0, 1.0) * max(0.0, tan(beta));

    float final = A + B * max(0.0, gamma) * C;

    return lightColor * albedo * final * (clamp(LDotN, 0.0, 1.0) + 0.25);
}

vec3 DepthToPosition(float depth, vec4 posProj, mat4 projInv, float farClip)
{
    vec3 posView = (projInv * posProj).xyz;
    vec3 viewRay = vec3(posView.xy * (farClip / posView.z), farClip);
    return viewRay * depth;
}


