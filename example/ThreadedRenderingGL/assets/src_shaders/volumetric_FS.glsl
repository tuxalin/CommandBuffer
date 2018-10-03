
#version 310 es

precision mediump float;

// INPUT
layout(location = 0) in vec4 inProjPos;

// OUTPUT
layout(location = 0) out vec4 outColor;

uniform vec3 uLightPosition;
uniform float uLightId;
uniform float uExposure;
uniform float uDecay;
uniform float uDensity;
uniform float uWeight;
uniform highp sampler2DRect uOclussionPass;

const int NUM_SAMPLES = 100;

void main()
{
	float decay= clamp(uDecay, 0.0, 1.0);
	vec2 texSize = vec2(textureSize(uOclussionPass));
	
	vec2 textCoord = inProjPos.st * 0.5 + 0.5;;
	vec2 deltaTextCoord = vec2(textCoord.st - uLightPosition.xy);
	deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * uDensity;

	vec3 color = texelFetch(uOclussionPass, ivec2(textCoord.xy * texSize)).rgb;
	float illuminationDecay = 1.0;
	for(int i=0; i < NUM_SAMPLES ; i++)
    {
		textCoord -= deltaTextCoord;

		vec4 value = texelFetch(uOclussionPass, ivec2(textCoord.xy * texSize));
		if(abs(value.a - uLightId) > 0.5) // must have same light id
			value.rgb = vec3(0.0);
		value.rgb *= illuminationDecay * uWeight;

        color += value.rgb;
		illuminationDecay *= decay;
     }
     outColor = vec4(color * uExposure, 1.0);
}